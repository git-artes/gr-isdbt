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
#include "bit_deinterleaver_streamed_impl.h"

namespace gr {
  namespace isdbt {

    // shouldn't these go somewhere else??
    const int bit_deinterleaver_streamed_impl::
        d_data_carriers_mode1 = 96; 
    const int bit_deinterleaver_streamed_impl::
        d_total_segments = 1; 
    const int bit_deinterleaver_streamed_impl::
        d_total_layers = 1; 

    const int bit_deinterleaver_streamed_impl::
        d_delay_qpsk[2] = {0, 120}; 
    const int bit_deinterleaver_streamed_impl::
        d_delay_16qam[4] = {0, 40, 80, 120}; 
    const int bit_deinterleaver_streamed_impl::
        d_delay_64qam[6] = {0, 24, 48, 72, 96, 120}; 
    
    bit_deinterleaver_streamed::sptr
        bit_deinterleaver_streamed::make(int mode, int constellation)
        {
          return gnuradio::get_initial_sptr
            (new bit_deinterleaver_streamed_impl(mode, constellation));
        }

    /*
     * The private constructor
     */
    bit_deinterleaver_streamed_impl::
        bit_deinterleaver_streamed_impl(int mode, int constellation_size)
            : gr::sync_interpolator("bit_deinterleaver_streamed",
              gr::io_signature::make(1, 1, 
                  sizeof(unsigned char) * d_total_segments *
                      d_data_carriers_mode1 * ((int)pow(2.0, mode-1))),
              gr::io_signature::make(1, 1, 
                  sizeof(unsigned char)),
              d_total_segments * d_data_carriers_mode1 * 
                  ((int)pow(2.0, mode-1)))
    {
        d_mode = mode; 
        d_const_size = constellation_size; 
        d_num_bits = log2(constellation_size); 
        d_carriers_per_segment = d_data_carriers_mode1 * 
            ((int)pow(2.0, mode-1)); 
        d_noutput = d_total_segments * d_carriers_per_segment; 

        for (int layer = 0; layer<d_total_layers; layer++){
            d_shift.push_back(new std::deque<unsigned char>(120,0)); 
        }

        if (d_const_size==4) {
            d_delay = d_delay_qpsk; 
        } else if (d_const_size==16) {
            d_delay = d_delay_16qam; 
        } else if (d_const_size==64) {
            d_delay = d_delay_64qam; 
        } else {
            std::cout << 
                "bit_deinterleaver_streamed: error in d_const_size\n"; 
        }
    }

    /*
     * Our virtual destructor.
     */
    bit_deinterleaver_streamed_impl::~bit_deinterleaver_streamed_impl()
    {
    }

    /*
     * Our work function.
     */
    int
    bit_deinterleaver_streamed_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const unsigned char *in = (const unsigned char*) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];

        // Do <+signal processing+>
        unsigned char aux = 0; 
        unsigned char mask = 1; 

        for (int i=0; i<noutput_items/d_noutput; i++)
        {
            for (int layer=0; layer<d_total_layers; layer++)
            {
                for (int carrier = 0; carrier<d_noutput; carrier++)
                {
                    // add new input symbol at beginning of container
                    // Older symbols are at bigger indexes for consistency
                    // with the d_delay implementation. 
                    d_shift[layer]-> 
                        push_front(in[i*d_noutput + carrier]); 
                    // Initialize aux variables to construct output
                    aux = 0; 
                    mask = 1; 
                    for (int b=0; b<d_num_bits; b++){
                        // Least significant bits more delayed in interleaver,
                        // so now delay more the most significant bits
                        aux |= (*d_shift[layer])[d_delay[b]] & mask; 
                        //aux |= d_shift[layer]->at(d_delay[b]) & mask; 
                        mask = mask << 1;  
                    }
                    out[i*d_noutput+carrier] = aux; 
                    d_shift[layer]->pop_back(); 
                    //printf("bit_deinterlever: out: %x\n",aux);
                }
            }
        }
        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace isdbt */
} /* namespace gr */

