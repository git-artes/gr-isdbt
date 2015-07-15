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

#ifndef INCLUDED_ISDBT_ENERGY_DESCRAMBLER_IMPL_H
#define INCLUDED_ISDBT_ENERGY_DESCRAMBLER_IMPL_H

#include <isdbt/energy_descrambler.h>

namespace gr {
  namespace isdbt {

    class energy_descrambler_impl : public energy_descrambler
    {
     private:
      static const int d_SYNC;
      static const int d_TSP_SIZE;

      // Register for PRBS
      int d_reg;

      // Index
      int d_index;
      // Search interval
      int d_search;

      int d_offset;


      /*! 
       * Initialize the PRBS (pseudo-random bit-sequence) generator. 
       */
      void init_prbs(); 

      /*!
       * Advance the PRBS (pseudo-random bit-sequence) generator clock steps. 
       */
      int clock_prbs(int clocks);

     public:
      energy_descrambler_impl();
      ~energy_descrambler_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_ENERGY_DESCRAMBLER_IMPL_H */

