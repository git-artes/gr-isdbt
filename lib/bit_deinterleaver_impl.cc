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

#include <math.h>
#include <stdio.h>
#include <gnuradio/io_signature.h>
#include "bit_deinterleaver_impl.h"

namespace gr {
  namespace isdbt {

      // shouldn't these go somewhere else??
    const int bit_deinterleaver_impl::d_data_carriers_mode1 = 96; 
    const int bit_deinterleaver_impl::d_total_segments = 1; 

    const int bit_deinterleaver_impl::d_delay_qpsk[2] = {0, 120}; 
    const int bit_deinterleaver_impl::d_delay_16qam[4] = {0, 40, 80, 120}; 
    const int bit_deinterleaver_impl::d_delay_64qam[6] = {0, 24, 48, 72, 96, 120}; 


    bit_deinterleaver::sptr
    bit_deinterleaver::make(int mode, int constellation)
    {
      return gnuradio::get_initial_sptr
        (new bit_deinterleaver_impl(mode, constellation));
    }

    /*
     * The private constructor
     */
    bit_deinterleaver_impl::bit_deinterleaver_impl(int mode, int constellation_size)
      : gr::sync_block("bit_deinterleaver",
              gr::io_signature::make(1, 1, sizeof(unsigned char)*d_total_segments*d_data_carriers_mode1*((int)pow(2.0,mode-1))),
              gr::io_signature::make(1, 1, sizeof(unsigned char)*d_total_segments*d_data_carriers_mode1*((int)pow(2.0,mode-1))))
    {
        d_mode = mode; 
        d_const_size = constellation_size; 
        d_num_bits = log2(constellation_size); 
        d_carriers_per_segment = d_total_segments*d_data_carriers_mode1*((int)pow(2.0,mode-1)); 
        d_noutput = d_total_segments*d_carriers_per_segment; 

        for (int segment = 0; segment<d_total_segments; segment++){
            for (int carrier = 0; carrier < d_carriers_per_segment; carrier++){
                d_shift.push_back(new std::deque<unsigned char>(120,0)); 
            }
        }

        if(d_const_size==4){
            d_delay = d_delay_qpsk; 
        }
        else if (d_const_size==16){
            d_delay = d_delay_16qam; 
        }
        else{
            d_delay = d_delay_64qam; 
        }

    }

    /*
     * Our virtual destructor.
     */
    bit_deinterleaver_impl::~bit_deinterleaver_impl()
    {
    }

    int
    bit_deinterleaver_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];

        unsigned char aux = 0; 
        unsigned char mask = 1; 


        /*std::vector<tag_t> tags; 
        const uint64_t nread = this->nitems_read(0); 
        this->get_tags_in_range(tags,0,nread,nread+noutput_items,pmt::string_to_symbol("frame_end")); 
        if(tags.size()){
            printf("FRAME END!!!!!!!!!!!!!!!!!!!"); 
        }*/

        //printf("bit_deinterleaver: noutput_items: %d, \n", noutput_items);
        for (int i=0; i<noutput_items; i++)
        {
            for (int carrier = 0; carrier<d_noutput; carrier++)
            {
                // I add the new input symbol at the beginning of the container
                // Older symbols are at bigger indexes, so as to be consistent with 
                // the d_delay implementation. 
                d_shift[carrier]->push_front(in[i*d_noutput + carrier]); 
                // Initialize the auxiliary variables to construct the output
                aux = 0; 
                mask = 1; 
                for (int b=0; b<d_num_bits; b++){
                    // The least significant bits are more delayed in the interleaver, so I now
                    // have to delay more the most significant ones
                    aux |= (*d_shift[carrier])[d_delay[b]] & mask; 
                    //aux |= d_shift[carrier]->at(d_delay[b]) & mask; 
                   mask = mask << 1;  
                }
                out[i*d_noutput+carrier] = aux; 
                d_shift[carrier]->pop_back(); 
                //printf("bit_deinterlever: out: %x\n",aux);
            }

        }


        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace isdbt */
} /* namespace gr */

