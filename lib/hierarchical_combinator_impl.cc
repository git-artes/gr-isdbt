/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017, 2018
 *   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
 *   Javier Hernández
 *   Santiago Castro
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
#include "hierarchical_combinator_impl.h"

namespace gr {
    namespace isdbt {

        // TODO shouldn't these go somewhere else??
        const int hierarchical_combinator_impl::d_data_carriers_mode1 = 96; 
        const int hierarchical_combinator_impl::d_total_segments = 13; 

        hierarchical_combinator::sptr
            hierarchical_combinator::make(int mode, int segments_A, int segments_B, int segments_C)
            {
                return gnuradio::get_initial_sptr
                    (new hierarchical_combinator_impl(mode, segments_A, segments_B, segments_C));
            }

        /*
         * The private constructor
         */
        hierarchical_combinator_impl::hierarchical_combinator_impl(int mode, int segments_A, int segments_B, int segments_C)
            : gr::sync_block("hierarchical_combinator",
                    
                    gr::io_signature::make3(1, 3, 
                        sizeof(gr_complex)*segments_A*d_data_carriers_mode1 * ((int)pow(2.0,mode-1)), 
                        sizeof(gr_complex)*(segments_B?segments_B:1)*d_data_carriers_mode1 * ((int)pow(2.0,mode-1)), 
                        sizeof(gr_complex)*(segments_C?segments_C:1)*d_data_carriers_mode1 * ((int)pow(2.0,mode-1))
                        ),

                    gr::io_signature::make(1, 1, sizeof(gr_complex)*d_total_segments*d_data_carriers_mode1 * ((int)pow(2.0,mode-1))))
        {
            d_mode = mode;
            d_nsegments_A = segments_A;
            d_nsegments_B = segments_B;
            d_nsegments_C = segments_C;

			// We check if the total segments are what the should
			assert(segments_A + segments_B + segments_C == d_total_segments);
            
            d_carriers_per_segment = d_data_carriers_mode1*((int)pow(2.0,mode-1)); 

            d_noutput = d_total_segments*d_carriers_per_segment;
		   	d_ninput_A = segments_A*d_carriers_per_segment;
		   	d_ninput_B = segments_B*d_carriers_per_segment;
		   	d_ninput_C = segments_C*d_carriers_per_segment;
        }

        /*
         * Our virtual destructor.
         */
        hierarchical_combinator_impl::~hierarchical_combinator_impl()
        {
        }

        // TODO it exits with exit code -15. Check why
        int
            hierarchical_combinator_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const gr_complex *in_A = (const gr_complex *) input_items[0];
                const gr_complex *in_B = (const gr_complex *) input_items[1];
                const gr_complex *in_C = (const gr_complex *) input_items[2];
                gr_complex *out = (gr_complex *) output_items[0];

                bool in_B_connected = input_items.size()>=2;
                bool in_C_connected = input_items.size()>=3;		

                int size_complex = sizeof(gr_complex);

                for (int i = 0; i<noutput_items; i++)
                {
                    //Layer A segments to output
                    //printf("Layer A, in index: %i\n", output*d_segments_a*d_active_carriers);
                    memcpy(out + i*d_noutput, in_A + i*d_ninput_A, d_ninput_A*size_complex);

                    //Layer B segments to output
                    if (in_B_connected)
                    {
                        //printf("Layer B, in index: %i\n", output*d_segments_b*d_active_carriers);
                        memcpy(out + i*d_noutput + d_ninput_A, in_B + i*d_ninput_B, d_ninput_B*size_complex);
                    }

                    //Layer C segments to output
                    if (in_C_connected)
                    {
                        //printf("Layer C, in index: %i\n", output*d_segments_c*d_active_carriers);
                        memcpy(out + i*d_noutput + d_ninput_A + d_ninput_B, in_C + i*d_ninput_C, d_ninput_C*size_complex);
                    }      
                }

                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

