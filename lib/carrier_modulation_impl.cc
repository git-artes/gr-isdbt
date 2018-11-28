/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017, 2018
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
#include "carrier_modulation_impl.h"

namespace gr {
    namespace isdbt {

        // shouldn't these go somewhere else??
        const int carrier_modulation_impl::d_data_carriers_mode1 = 96; 

        const int carrier_modulation_impl::d_delay_qpsk[2] = {120, 0}; 
        const int carrier_modulation_impl::d_delay_16qam[4] = {120, 80, 40, 0}; 
        const int carrier_modulation_impl::d_delay_64qam[6] = {120, 96, 72, 48, 24, 0}; 

        const gr_complex carrier_modulation_impl::d_symbols_qpsk[4] = {
            gr_complex(1,1), gr_complex(1,-1), gr_complex(-1,1), gr_complex(-1,-1) 
        };

        const gr_complex carrier_modulation_impl::d_symbols_16qam[16] = {
            gr_complex(3,3), gr_complex(3,1), gr_complex(1,3), gr_complex(1,1),\
                gr_complex(3,-3), gr_complex(3,-1), gr_complex(1,-3), gr_complex(1,-1),\
                gr_complex(-3,3), gr_complex(-3,1), gr_complex(-1,3), gr_complex(-1,1),\
                gr_complex(-3,-3), gr_complex(-3,-1), gr_complex(-1,-3), gr_complex(-1,-1)
        };

        const gr_complex carrier_modulation_impl::d_symbols_64qam[64] = {
            gr_complex(7,7), gr_complex(7,5), gr_complex(5,7), gr_complex(5,5),\
                gr_complex(7,1), gr_complex(7,3), gr_complex(5,1), gr_complex(5,3),\
                gr_complex(1,7), gr_complex(1,5), gr_complex(3,7), gr_complex(3,5),\
                gr_complex(1,1), gr_complex(1,3), gr_complex(3,1), gr_complex(3,3),\
                gr_complex(7,-7), gr_complex(7,-5), gr_complex(5,-7), gr_complex(5,-5),\
                gr_complex(7,-1), gr_complex(7,-3), gr_complex(5,-1), gr_complex(5,-3),\
                gr_complex(1,-7), gr_complex(1,-5), gr_complex(3,-7), gr_complex(3,-5),\
                gr_complex(1,-1), gr_complex(1,-3), gr_complex(3,-1), gr_complex(3,-3),\
                gr_complex(-7,7), gr_complex(-7,5), gr_complex(-5,7), gr_complex(-5,5),\
                gr_complex(-7,1), gr_complex(-7,3), gr_complex(-5,1), gr_complex(-5,3),\
                gr_complex(-1,7), gr_complex(-1,5), gr_complex(-3,7), gr_complex(-3,5),\
                gr_complex(-1,1), gr_complex(-1,3), gr_complex(-3,1), gr_complex(-3,3),\
                gr_complex(-7,-7), gr_complex(-7,-5), gr_complex(-5,-7), gr_complex(-5,-5),\
                gr_complex(-7,-1), gr_complex(-7,-3), gr_complex(-5,-1), gr_complex(-5,-3),\
                gr_complex(-1,-7), gr_complex(-1,-5), gr_complex(-3,-7), gr_complex(-3,-5),\
                gr_complex(-1,-1), gr_complex(-1,-3), gr_complex(-3,-1), gr_complex(-3,-3)
        };


        carrier_modulation::sptr
            carrier_modulation::make(int mode, int segments, int constellation_size)
            {
                return gnuradio::get_initial_sptr
                    (new carrier_modulation_impl(mode, segments, constellation_size));
            }

        /*
         * The private constructor
         */
        carrier_modulation_impl::carrier_modulation_impl(int mode, int segments, int constellation_size)
            : gr::sync_decimator("carrier_modulation",
                    gr::io_signature::make(1, 1, sizeof(unsigned char)),
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*segments*d_data_carriers_mode1*((int)pow(2.0, mode-1))), 
                    segments * d_data_carriers_mode1 * ((int)pow(2.0, mode-1))
                    )
        {

            d_mode = mode; 
            d_const_size = constellation_size;

            d_num_bits = log2(constellation_size);

            d_nsegments = segments;
            d_carriers_per_segment = d_data_carriers_mode1*((int)pow(2.0, mode-1)); 

            d_total_carriers = d_nsegments * d_carriers_per_segment; 

            // normalization factor for the constellation
            gr_complex norm_factor = 1;
            // extra delay required by the standard (so-called delay-adjustement)
            int extra_delay = compute_extra_delay(d_mode, d_const_size, d_nsegments);

            d_symbols = new gr_complex[d_const_size]; 
            d_delay = new int[d_num_bits]; 
            if (d_const_size==4) {
                norm_factor = sqrt(2); 
                for (int i=0; i<d_const_size; i++) {
                    d_symbols[i] = d_symbols_qpsk[i]/norm_factor;
                }
                for (int i=0; i<d_num_bits; i++) {
                    d_delay[i] = d_delay_qpsk[i]+extra_delay;
                }

            } else if (d_const_size==16) {
                norm_factor = sqrt(10); 
                for (int i=0; i<d_const_size; i++) {
                    d_symbols[i] = d_symbols_16qam[i]/norm_factor;
                }
                for (int i=0; i<d_num_bits; i++) {
                    d_delay[i] = d_delay_16qam[i]+extra_delay;
                }
            } else if (d_const_size==64) {
                norm_factor = sqrt(42); 
                for (int i=0; i<d_const_size; i++) {
                    d_symbols[i] = d_symbols_64qam[i]/norm_factor;
                }
                for (int i=0; i<d_num_bits; i++) {
                    d_delay[i] = d_delay_64qam[i]+extra_delay;
                }
            } else {
                printf("carrier_modulation: error in d_const_size = %i \n",d_const_size); 
            }

            d_shift = new boost::circular_buffer<unsigned char>(120+extra_delay+1,0);


        }

        /*
         * Our virtual destructor.
         */
        carrier_modulation_impl::~carrier_modulation_impl()
        {
            delete [] d_symbols;
            delete [] d_delay;
            delete d_shift;

        }

        int carrier_modulation_impl::compute_extra_delay(int mode, int const_size, int nsegments) 
        {
            int extra_delay = 0; 
            if (d_const_size==4) {
                if (d_mode == 1){
                    extra_delay = 384*nsegments - 240;
                } else if (d_mode == 2) {
                    extra_delay = 768*nsegments - 240;
                } else if (d_mode ==3) {
                    extra_delay = 1536*nsegments - 240;
                }
            } else if (d_const_size==16) {
                if (d_mode == 1){
                    extra_delay = 768*nsegments - 480;
                } else if (d_mode == 2) {
                    extra_delay = 1536*nsegments - 480;
                } else if (d_mode ==3) {
                    extra_delay = 3072*nsegments - 480;
                }
            } else if (d_const_size==64) {
                if (d_mode == 1){
                    extra_delay = 1152*nsegments - 720;
                } else if (d_mode == 2) {
                    extra_delay = 2304*nsegments - 720;
                } else if (d_mode ==3) {
                    extra_delay = 4608*nsegments - 720;
                }
            }

            // the delay in the table is on bits. We are getting at the input bit-words (i.e. 2 bits in QPSK). Thus the actual 
            // delay we'll use is scaled accordingly. 
            return extra_delay/d_num_bits;

        }


        int
            carrier_modulation_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const unsigned char *in = (const unsigned char *) input_items[0];
                gr_complex *out = (gr_complex *) output_items[0];

                unsigned char aux; 
                unsigned char mask; 
                for(int i=0; i<noutput_items; i++) {

                    for (int carrier = 0; carrier<d_total_carriers; carrier++)
                    {
                        // add new input symbol at beginning of container
                        // Older symbols are at bigger indexes for consistency
                        // with the d_delay implementation. 
                        d_shift-> push_front(in[i*d_total_carriers + carrier]); 
                        // Initialize aux variables to construct output
                        aux = 0; 
                        mask = 1; 

                        for (int b=0; b<d_num_bits; b++){
                            // Least significant bits more delayed in interleaver.
                            aux |= d_shift->at(d_delay[b]) & mask; 
                            mask = mask << 1;  

                        }

                        d_shift->pop_back();
                        out[i*d_total_carriers+carrier] = d_symbols[aux]; 

                    }

                }

                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

