/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
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
            : gr::sync_block("reed_solomon_dec_isdbt",
                    gr::io_signature::make(1, 1, sizeof(unsigned char)*d_blocks*(d_n-d_s)),
                    gr::io_signature::make2(1, 2, sizeof(unsigned char)*d_blocks*(d_k-d_s), sizeof(float))),
            d_rs(d_p, d_m, d_gfpoly, d_n, d_k, d_t, d_s, d_blocks)
        {
            //TODO why does d_rs cannot be initialized in the body of the constructor??

            d_in = new unsigned char[d_n];
            if (d_in == NULL)
            {
                std::cout << "Cannot allocate memory" << std::endl;
                return;
            }
            memset(&d_in[0], 0, d_n);

        }

        /*
         * Our virtual destructor.
         */
        reed_solomon_dec_isdbt_impl::~reed_solomon_dec_isdbt_impl()
        {
            delete [] d_in;
        }

        int
            reed_solomon_dec_isdbt_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const unsigned char *in = (const unsigned char *) input_items[0];
                unsigned char *out = (unsigned char *) output_items[0];

                float *ber_out = (float *) output_items[1]; 
                bool ber_out_connected  = output_items.size()>=2;  

                // We receive only nonzero data
                int in_bsize = d_n - d_s;
                int out_bsize = d_k - d_s;

                //gettimeofday(&tvs, &tzs);

                for (int i = 0; i < (d_blocks * noutput_items); i++)
                {
                    //TODO - zero copy?
                    // Set first d_s symbols to zero
                    memset(&d_in[0], 0, d_s);
                    // Then copy actual data
                    memcpy(&d_in[d_s], &in[i * in_bsize], in_bsize);

                    d_rs.rs_decode(d_in, NULL, 0);

                    memcpy(&out[i * out_bsize], &d_in[d_s], out_bsize);

                    if(ber_out_connected){
                        unsigned int total_bit_errors = 0; 
                        unsigned char error; 
                        for (int B=0; B<out_bsize; B++){
                            error = out[i*noutput_items+B]^in[i*noutput_items + B]; 
                            for (int b=0; b<8; b++){
                                total_bit_errors += ((error>>b)&1); 
                            }
                            //if (total_bit_errors>0)
                            //    printf("out: %x; in: %x; total_bit_errors: %i; error: %x\n", out[i*noutput_items+B], in[i*noutput_items+B], total_bit_errors, error); 
                        }
                        ber_out[i*noutput_items] = total_bit_errors/(float)(out_bsize*8.0); 
                    }

                    //printf("Reed-Solomon: out[0]=%x\n", out[0]); 
                }

                //gettimeofday(&tve, &tze);

                //printf("reed_solomon: blocks: %i, us: %f\n", d_blocks * noutput_items,
                //(float) (tve.tv_usec - tvs.tv_usec) / (float) (d_blocks * noutput_items));

                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

