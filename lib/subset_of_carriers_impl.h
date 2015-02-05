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

#ifndef INCLUDED_ISDBT_SUBSET_OF_CARRIERS_IMPL_H
#define INCLUDED_ISDBT_SUBSET_OF_CARRIERS_IMPL_H

#include <isdbt/subset_of_carriers.h>

namespace gr {
  namespace isdbt {

    class subset_of_carriers_impl : public subset_of_carriers
    {
     private:
         int d_noutput; 
         int d_total; 
         int d_first; 
         int d_last; 

     public:
      subset_of_carriers_impl(int total, int first, int last);
      ~subset_of_carriers_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_SUBSET_OF_CARRIERS_IMPL_H */

