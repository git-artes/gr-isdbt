/* -*- c++ -*- */
/*  
 * Copyright 2015
 *   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
 *   Pablo Belzarena 
 *   Gabriel Gomez Sena 
 *   Pablo Flores Guridi 
 *  Victor Gonzalez Barbone
 *
 *  Instituto de Ingenieria Electrica, Facultad de Ingenieria,
 *  Universidad de la Republica, Uruguay.
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

#include <math.h>
#include <stdio.h>

#include <gnuradio/io_signature.h>
#include "symbol_demapper_1seg_impl.h"

namespace gr {
    namespace isdbt {

        // TODO shouldn't these go somewhere else??
        const int symbol_demapper_1seg_impl::d_data_carriers_mode1 = 96; 
        const int symbol_demapper_1seg_impl::d_total_segments = 1; 

        const gr_complex symbol_demapper_1seg_impl::d_constellation_qpsk[] = {
            gr_complex(+1/sqrt(2),+1/sqrt(2)),
            gr_complex(+1/sqrt(2),-1/sqrt(2)), 
            gr_complex(-1/sqrt(2),+1/sqrt(2)),
            gr_complex(-1/sqrt(2),-1/sqrt(2))
        }; 
        
        const gr_complex symbol_demapper_1seg_impl::d_constellation_16qam[] = {
            gr_complex(3/sqrt(10),3/sqrt(10)),
            gr_complex(3/sqrt(10),1/sqrt(10)), 
            gr_complex(1/sqrt(10),3/sqrt(10)),
            gr_complex(1/sqrt(10),1/sqrt(10)),

            gr_complex(3/sqrt(10),-3/sqrt(10)),
            gr_complex(3/sqrt(10),-1/sqrt(10)), 
            gr_complex(1/sqrt(10),-3/sqrt(10)),
            gr_complex(1/sqrt(10),-1/sqrt(10)),

            gr_complex(-3/sqrt(10),3/sqrt(10)),
            gr_complex(-3/sqrt(10),1/sqrt(10)), 
            gr_complex(-1/sqrt(10),3/sqrt(10)),
            gr_complex(-1/sqrt(10),1/sqrt(10)),
            
            gr_complex(-3/sqrt(10),-3/sqrt(10)),
            gr_complex(-3/sqrt(10),-1/sqrt(10)), 
            gr_complex(-1/sqrt(10),-3/sqrt(10)),
            gr_complex(-1/sqrt(10),-1/sqrt(10))
        }; 
        
        const gr_complex symbol_demapper_1seg_impl::d_constellation_64qam[] = {
            gr_complex(7/sqrt(42),7/sqrt(42)),
            gr_complex(7/sqrt(42),5/sqrt(42)), 
            gr_complex(5/sqrt(42),7/sqrt(42)),
            gr_complex(5/sqrt(42),5/sqrt(42)),

            gr_complex(7/sqrt(42),1/sqrt(42)),
            gr_complex(7/sqrt(42),3/sqrt(42)), 
            gr_complex(5/sqrt(42),1/sqrt(42)),
            gr_complex(5/sqrt(42),3/sqrt(42)),

            gr_complex(1/sqrt(42),7/sqrt(42)),
            gr_complex(1/sqrt(42),5/sqrt(42)), 
            gr_complex(3/sqrt(42),7/sqrt(42)),
            gr_complex(3/sqrt(42),5/sqrt(42)),

            gr_complex(1/sqrt(42),1/sqrt(42)),
            gr_complex(1/sqrt(42),3/sqrt(42)), 
            gr_complex(3/sqrt(42),1/sqrt(42)),
            gr_complex(3/sqrt(42),3/sqrt(42)),

            //////////////
            gr_complex(7/sqrt(42),-7/sqrt(42)),
            gr_complex(7/sqrt(42),-5/sqrt(42)), 
            gr_complex(5/sqrt(42),-7/sqrt(42)),
            gr_complex(5/sqrt(42),-5/sqrt(42)),

            gr_complex(7/sqrt(42),-1/sqrt(42)),
            gr_complex(7/sqrt(42),-3/sqrt(42)), 
            gr_complex(5/sqrt(42),-1/sqrt(42)),
            gr_complex(5/sqrt(42),-3/sqrt(42)),

            gr_complex(1/sqrt(42),-7/sqrt(42)),
            gr_complex(1/sqrt(42),-5/sqrt(42)), 
            gr_complex(3/sqrt(42),-7/sqrt(42)),
            gr_complex(3/sqrt(42),-5/sqrt(42)),

            gr_complex(1/sqrt(42),-1/sqrt(42)),
            gr_complex(1/sqrt(42),-3/sqrt(42)), 
            gr_complex(3/sqrt(42),-1/sqrt(42)),
            gr_complex(3/sqrt(42),-3/sqrt(42)),

            //////////////
            gr_complex(-7/sqrt(42),7/sqrt(42)),
            gr_complex(-7/sqrt(42),5/sqrt(42)), 
            gr_complex(-5/sqrt(42),7/sqrt(42)),
            gr_complex(-5/sqrt(42),5/sqrt(42)),

            gr_complex(-7/sqrt(42),1/sqrt(42)),
            gr_complex(-7/sqrt(42),3/sqrt(42)), 
            gr_complex(-5/sqrt(42),1/sqrt(42)),
            gr_complex(-5/sqrt(42),3/sqrt(42)),

            gr_complex(-1/sqrt(42),7/sqrt(42)),
            gr_complex(-1/sqrt(42),5/sqrt(42)), 
            gr_complex(-3/sqrt(42),7/sqrt(42)),
            gr_complex(-3/sqrt(42),5/sqrt(42)),

            gr_complex(-1/sqrt(42),1/sqrt(42)),
            gr_complex(-1/sqrt(42),3/sqrt(42)), 
            gr_complex(-3/sqrt(42),1/sqrt(42)),
            gr_complex(-3/sqrt(42),3/sqrt(42)),

            //////////////
            gr_complex(-7/sqrt(42),-7/sqrt(42)),
            gr_complex(-7/sqrt(42),-5/sqrt(42)), 
            gr_complex(-5/sqrt(42),-7/sqrt(42)),
            gr_complex(-5/sqrt(42),-5/sqrt(42)),

            gr_complex(-7/sqrt(42),-1/sqrt(42)),
            gr_complex(-7/sqrt(42),-3/sqrt(42)), 
            gr_complex(-5/sqrt(42),-1/sqrt(42)),
            gr_complex(-5/sqrt(42),-3/sqrt(42)),

            gr_complex(-1/sqrt(42),-7/sqrt(42)),
            gr_complex(-1/sqrt(42),-5/sqrt(42)), 
            gr_complex(-3/sqrt(42),-7/sqrt(42)),
            gr_complex(-3/sqrt(42),-5/sqrt(42)),

            gr_complex(-1/sqrt(42),-1/sqrt(42)),
            gr_complex(-1/sqrt(42),-3/sqrt(42)), 
            gr_complex(-3/sqrt(42),-1/sqrt(42)),
            gr_complex(-3/sqrt(42),-3/sqrt(42)),
        }; 

        symbol_demapper_1seg::sptr
            symbol_demapper_1seg::make(int mode, int constellation_size)
            {
                return gnuradio::get_initial_sptr
                    (new symbol_demapper_1seg_impl(mode, constellation_size));
            }

        /*
         * The private constructor
         */
        symbol_demapper_1seg_impl::
            symbol_demapper_1seg_impl(int mode, int constellation_size)
                : gr::sync_block("symbol_demapper_1seg",
                    gr::io_signature::make(1, 1, 
                        sizeof(gr_complex) * d_total_segments *
                            d_data_carriers_mode1 * ((int)pow(2.0,mode-1))),
                    gr::io_signature::make(1, 1, 
                        sizeof(unsigned char) * d_total_segments *
                            d_data_carriers_mode1 * ((int)pow(2.0,mode-1))))
        {
            d_mode = mode; 
            d_const_size = constellation_size; 
            if (d_const_size==4) {
                d_constellation = d_constellation_qpsk;
            } else if (d_const_size==16) {
                d_constellation = d_constellation_16qam; 
            } else if (d_const_size==64) {
                d_constellation = d_constellation_64qam; 
            } else {
            std::cout << 
                "symbol_demapper_1seg: error in d_const_size\n"; 
            }

            d_carriers_per_segment = d_data_carriers_mode1 * 
                ((int)pow(2.0,mode-1)); 
            d_noutput = d_total_segments*d_carriers_per_segment; 
            
        }

        /*
         * Our virtual destructor.
         */
        symbol_demapper_1seg_impl::~symbol_demapper_1seg_impl()
        {
        }

        int 
            symbol_demapper_1seg_impl::find_constellation_value(gr_complex val)
            {
                float min_dist = std::norm(val - d_constellation[0]);
                int min_index = 0;

                for (int i = 0; i < d_const_size; i++)
                {
                    float dist = std::norm(val - d_constellation[i]);
                    if (dist < min_dist)
                    {
                        min_dist = dist;
                        min_index = i;
                    }
                }

                return min_index;
            }


        int
            symbol_demapper_1seg_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const gr_complex *in = (const gr_complex *) input_items[0];
                unsigned char *out = (unsigned char *) output_items[0];

                for (int i = 0; i < noutput_items; i++)
                {
                    for (int carrier = 0; carrier<d_noutput; carrier++){
                        // leave it like this for now to easily accomodate the 
                        // per-segment constellation possibility later on. 
                        out[i*d_noutput+carrier] = 
                            find_constellation_value(in[i*d_noutput+carrier]);
                    }
                }

                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

