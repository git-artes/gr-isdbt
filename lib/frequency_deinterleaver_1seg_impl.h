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

#ifndef INCLUDED_ISDBT_FREQUENCY_DEINTERLEAVER_1SEG_IMPL_H
#define INCLUDED_ISDBT_FREQUENCY_DEINTERLEAVER_1SEG_IMPL_H

#include <isdbt/frequency_deinterleaver_1seg.h>

namespace gr {
  namespace isdbt {

    class frequency_deinterleaver_1seg_impl : public frequency_deinterleaver_1seg
    {
     private:
         int d_mode; 
         int d_carriers_per_segment; 
         int d_noutput; 

         static const int d_data_carriers_mode1; 
         static const int d_total_segments; 

         static const int d_random_perm_mode1[]; 
         static const int d_random_perm_mode2[]; 
         static const int d_random_perm_mode3[]; 

         /*The current random permutation matrix*/
         const int * d_random_perm; 

         /*
          * It simply derandomizes the carriers of each segment as specified by the standard. 
          */
         gr_complex * derandomize(const gr_complex * random, gr_complex * not_random); 
         

     public:
      frequency_deinterleaver_1seg_impl(int mode);
      ~frequency_deinterleaver_1seg_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_FREQUENCY_DEINTERLEAVER_1SEG_IMPL_H */

