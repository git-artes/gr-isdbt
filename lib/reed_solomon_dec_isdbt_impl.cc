/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017, 2018, 2019, 2020, 2021
 *   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
 *   Pablo Belzarena 
 *   Gabriel Gomez Sena 
 *   Pablo Flores Guridi 
 *   Victor Gonzalez Barbone
 *
 *   Instituto de Ingenieria Electrica, Facultad de Ingenieria,
 *   Universidad de la Republica, Uruguay.
 *  
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *  
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "reed_solomon_dec_isdbt_impl.h"

#include <stdio.h>

namespace gr {
    namespace isdbt {

        const int reed_solomon_dec_isdbt_impl::d_p = 2;
        const int reed_solomon_dec_isdbt_impl::d_m = 8;
        const int reed_solomon_dec_isdbt_impl::d_gfpoly = 0x11d;
        const int reed_solomon_dec_isdbt_impl::d_n = 255;
        const int reed_solomon_dec_isdbt_impl::d_k = 239;
        const int reed_solomon_dec_isdbt_impl::d_t = 8;
        const int reed_solomon_dec_isdbt_impl::d_s = 51;
        const int reed_solomon_dec_isdbt_impl::d_blocks = 1;

        static const int d_rs_init_symsize =     8;
        static const int d_rs_init_fcr     =     0;   // first consecutive root
        static const int d_rs_init_prim    =     1;   // primitive is 1 (alpha)
        static const int d_N = (1 << d_rs_init_symsize) - 1; // 255

        reed_solomon_dec_isdbt::sptr
            reed_solomon_dec_isdbt::make()
            {
                return gnuradio::get_initial_sptr
                    (new reed_solomon_dec_isdbt_impl());
            }

        /*
         * The private constructor
         */
        reed_solomon_dec_isdbt_impl::reed_solomon_dec_isdbt_impl()
            : gr::block("reed_solomon_dec_isdbt",
                    gr::io_signature::make(1, 1, sizeof(unsigned char)*d_blocks*(d_n-d_s)),
                    gr::io_signature::make2(1, 2, sizeof(unsigned char)*d_blocks*(d_k-d_s), sizeof(float)))
        {
            
            d_rs = init_rs_char(d_rs_init_symsize, d_gfpoly, d_rs_init_fcr, d_rs_init_prim, (d_n - d_k));
            if (d_rs == NULL) {
                GR_LOG_FATAL(d_logger, "[GR-ISDBT] Reed-Solomon Decoder, cannot allocate memory for d_rs.");
                throw std::bad_alloc();
            }

            d_last_ber_out = 0.5;
            d_alpha_avg = 0.001; 

        }

        /*
         * Our virtual destructor.
         */
        reed_solomon_dec_isdbt_impl::~reed_solomon_dec_isdbt_impl()
        {
            free_rs_char(d_rs);
        }
        
        void
            reed_solomon_dec_isdbt_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
            {
                unsigned ninputs = ninput_items_required.size();
                for (unsigned int i = 0; i < ninputs; i++) {
                    ninput_items_required[i] = noutput_items;
                }
            }

        int
            reed_solomon_dec_isdbt_impl::decode (unsigned char &out, const unsigned char &in)
            {
                unsigned char tmp[d_N];
                int ncorrections;

                // add missing prefix zero padding to message
                memset(tmp, 0, d_s);
                memcpy(&tmp[d_s], &in, (d_n - d_s));

                // correct message...
                ncorrections = decode_rs_char(d_rs, tmp, 0, 0);

                // copy corrected message to output, skipping prefix zero padding
                memcpy (&out, &tmp[d_s], (d_k - d_s));

                return ncorrections;
            }


        int
            reed_solomon_dec_isdbt_impl::general_work(int noutput_items,
		            gr_vector_int &ninput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const unsigned char *in = (const unsigned char *) input_items[0];
                unsigned char *out = (unsigned char *) output_items[0];

                float *ber_out = (float *) output_items[1]; 
                bool ber_out_connected  = output_items.size()>=2;  

                int j = 0; 
                int k = 0; 

                int nerrors_corrected = 0;

                for (int i = 0; i < (d_blocks * noutput_items); i++)
                {
                    nerrors_corrected = decode(out[k], in[j]);

                    // if nerrors_corrected=-1 it means that the decoder gave up on the word
                    if(nerrors_corrected==-1)
                    {
                        // In order to generate a useable TS, we will not ouput these TSP.  
                        //printf("RS: impossible to correct\n");
                        // We also reset the BER average. 
                        d_last_ber_out = 0.5; 

                        consume_each(i+1); 
                        return i; 
                    }
                    else 
                    {
                        //printf("RS: possible to correct. RS_status: %i\n", rs_status);
                    }

                    j += (d_n - d_s);
                    k += (d_k - d_s);

                    if(ber_out_connected)
                    {
                            /*
                            unsigned int total_bit_errors = 0; 
                            unsigned char error; 
                            for (int B=0; B<out_bsize; B++){
                                error = out[i*out_bsize+B]^in[i*in_bsize + B]; 
                                for (int b=0; b<8; b++){
                                    total_bit_errors += ((error>>b)&1); 
                                }
                                //if (rs_status>0)
                                    //printf("B: %i; out: %x; in: %x; total_bit_errors: %i; error: %x\n", B, out[i*noutput_items+B], in[i*noutput_items+B], total_bit_errors, error); 
                            }
                            d_new_ber = total_bit_errors/(float)(out_bsize*8.0); 
                            */

                            //rs_status is the number of byte errors detected by the decoder (see reed_solomon.cc). It may 
                            //well happen that if the error is too big, it corrects bytes in the stuffed 0's at the beginning. Thus
                            //the method above (now commented) is not reliable as a BER indicator since these 0's are not part of 
                            //the output. Although the result is actual the byte error rate (and not the bit error rate), we preferred
                            //the method below that uses the rs_status, since at least it indicates errors in this case.  
                            d_new_ber = nerrors_corrected/(float)d_k;
                            ber_out[i] = d_last_ber_out; 
                            d_last_ber_out = d_alpha_avg*d_new_ber + (1-d_alpha_avg)*d_last_ber_out;
                            
                            //ber_out[i] = d_alpha_avg*d_new_ber + (1-d_alpha_avg)*d_last_ber_out;
                            //d_last_ber_out = ber_out[i]; 
                            
                    }

                    //printf("Reed-Solomon: out[0]=%x\n", out[0]); 
                }

                //printf("reed_solomon: blocks: %i, us: %f\n", d_blocks * noutput_items,

                // Tell runtime system how many output items we produced.
                consume_each(noutput_items); 
                return noutput_items;
                    
            }

    } /* namespace isdbt */
} /* namespace gr */


