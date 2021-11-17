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
#include "time_deinterleaver_impl.h"
#include <stdio.h>

namespace gr {
    namespace isdbt {


        // TODO shouldn't these be defined somewhere else??
        const int time_deinterleaver_impl::d_data_carriers_mode1 = 96; 
        const int time_deinterleaver_impl::d_total_segments = 13; 

        time_deinterleaver::sptr
            time_deinterleaver::make(int mode, int segments_A, int length_A, int segments_B, int length_B, int segments_C, int length_C)
            {
                return gnuradio::get_initial_sptr
                    (new time_deinterleaver_impl(mode, segments_A, length_A, segments_B, length_B, segments_C, length_C));
            }

        /*
         * The private constructor
         */
        time_deinterleaver_impl::time_deinterleaver_impl(int mode, int segments_A, int length_A, int segments_B, int length_B, int segments_C, int length_C)
            : gr::sync_block("time_deinterleaver",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*d_total_segments*d_data_carriers_mode1*((int)pow(2.0,mode-1))),
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*d_total_segments*d_data_carriers_mode1*((int)pow(2.0,mode-1))))
        {
            d_mode = mode; 
            d_I_A = length_A; 
            d_I_B = length_B; 
            d_I_C = length_C; 

            // I check if the total segments are what they should
            assert((segments_A + segments_B + segments_C) == d_total_segments);

            d_nsegments_A = segments_A; 
            d_nsegments_B = segments_B; 
            d_nsegments_C = segments_C; 

            d_carriers_per_segment = d_data_carriers_mode1*((int)pow(2.0,mode-1)); 
            d_noutput = d_total_segments*d_carriers_per_segment; 
            int mi = 0;

            for (int segment=0; segment<d_nsegments_A; segment++)
            {
                for (int carrier = 0; carrier<d_carriers_per_segment; carrier++)
               {
                    mi = (5*carrier) % d_data_carriers_mode1; 
                    //d_shift.push_back(new std::deque<gr_complex>(d_I*(d_data_carriers_mode1-1-mi),0)); 
                    d_shift.push_back(new boost::circular_buffer<gr_complex>(d_I_A*(d_data_carriers_mode1-1-mi)+1,0)); 
                }
            }
            for (int segment=0; segment<d_nsegments_B; segment++)
            {
                for (int carrier = 0; carrier<d_carriers_per_segment; carrier++)
               {
                    mi = (5*carrier) % d_data_carriers_mode1; 
                    //d_shift.push_back(new std::deque<gr_complex>(d_I*(d_data_carriers_mode1-1-mi),0)); 
                    d_shift.push_back(new boost::circular_buffer<gr_complex>(d_I_B*(d_data_carriers_mode1-1-mi)+1,0)); 
                }
            }
            for (int segment=0; segment<d_nsegments_C; segment++)
            {
                for (int carrier = 0; carrier<d_carriers_per_segment; carrier++)
               {
                    mi = (5*carrier) % d_data_carriers_mode1; 
                    //d_shift.push_back(new std::deque<gr_complex>(d_I*(d_data_carriers_mode1-1-mi),0)); 
                    d_shift.push_back(new boost::circular_buffer<gr_complex>(d_I_C*(d_data_carriers_mode1-1-mi)+1,0)); 
                }
            }
        }

        /*
         * Our virtual destructor.
         */
        time_deinterleaver_impl::~time_deinterleaver_impl()
        {
            for (unsigned int i=0; i<d_shift.size();i++){
                delete d_shift.back();
                d_shift.pop_back();
            }
        }

        int
            time_deinterleaver_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const gr_complex *in = (const gr_complex *) input_items[0];
                gr_complex *out = (gr_complex *) output_items[0];

                // TODO CHECK the tag propagation policy for the frame 
                // beginnning. 

                for (int i=0; i<noutput_items; i++)
                {
                    for (int carrier=0; carrier<d_noutput; carrier++)
                    {
                        // a simple FIFO queue performs the interleaving. 
                        // The "difficult" part is setting the correct sizes 
                        // for each queue. 
                        d_shift[carrier]->push_back(in[i*d_noutput + carrier]);
                        out[i*d_noutput + carrier] = d_shift[carrier]->front();
                        //d_shift[carrier]->pop_front(); 
                    }
                }
                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

