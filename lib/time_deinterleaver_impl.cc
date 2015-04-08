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
#include "time_deinterleaver_impl.h"

namespace gr {
    namespace isdbt {


        // TODO shouldn't these be defined somewhere else??
        const int time_deinterleaver_impl::d_data_carriers_mode1 = 96; 
        const int time_deinterleaver_impl::d_total_segments = 1; 

        time_deinterleaver::sptr
            time_deinterleaver::make(int mode, int length)
            {
                return gnuradio::get_initial_sptr
                    (new time_deinterleaver_impl(mode, length));
            }

        /*
         * The private constructor
         */
        time_deinterleaver_impl::time_deinterleaver_impl(int mode, int length)
            : gr::sync_block("time_deinterleaver",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*d_total_segments*d_data_carriers_mode1*((int)pow(2.0,mode-1))),
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*d_total_segments*d_data_carriers_mode1*((int)pow(2.0,mode-1))))
        {
            d_mode = mode; 
            //TODO the length of the interleaver may change from segment to segment. This should be corrected...
            d_I = length; 
            d_carriers_per_segment = d_data_carriers_mode1*((int)pow(2.0,mode-1)); 
            d_noutput = d_total_segments*d_carriers_per_segment; 
            int mi = 0;

            for (int segment=0; segment<d_total_segments; segment++)
            {
                for (int carrier = 0; carrier<d_carriers_per_segment; carrier++)
               {
                    mi = (5*carrier) % d_data_carriers_mode1; 
                    d_shift.push_back(new std::deque<gr_complex>(d_I*(d_data_carriers_mode1-1-mi),0)); 
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
                        d_shift[carrier]->pop_front(); 
                    }
                }
                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

