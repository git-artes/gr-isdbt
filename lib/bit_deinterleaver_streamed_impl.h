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

#ifndef INCLUDED_ISDBT_BIT_DEINTERLEAVER_STREAMED_IMPL_H
#define INCLUDED_ISDBT_BIT_DEINTERLEAVER_STREAMED_IMPL_H

#include <isdbt/bit_deinterleaver_streamed.h>

namespace gr {
  namespace isdbt {

    class bit_deinterleaver_streamed_impl : public bit_deinterleaver_streamed
    {
     private:
         static const int d_data_carriers_mode1; 
         static const int d_total_segments; 
         static const int d_total_layers; 

         static const int d_delay_qpsk[]; 
         static const int d_delay_16qam[]; 
         static const int d_delay_64qam[]; 
         
         int d_mode; 
         int d_const_size; 
         int d_num_bits; 
         int d_carriers_per_segment; 
         int d_noutput; 
         const int * d_delay; 

         //Here are the symbols. To create an output I have to check
         //several symbols back. Where and how many depends on the 
         //constellation size. 
         std::vector<std::deque<unsigned char> *> d_shift; 


     public:
      bit_deinterleaver_streamed_impl(int mode, int constellation);
      ~bit_deinterleaver_streamed_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_BIT_DEINTERLEAVER_STREAMED_IMPL_H */

