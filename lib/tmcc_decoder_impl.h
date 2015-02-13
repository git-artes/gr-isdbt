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

#ifndef INCLUDED_ISDBT_TMCC_DECODER_IMPL_H
#define INCLUDED_ISDBT_TMCC_DECODER_IMPL_H

#include <isdbt/tmcc_decoder.h>

namespace gr {
  namespace isdbt {

    class tmcc_decoder_impl : public tmcc_decoder
    {
     private:
    	/*ATTRIBUTES*/

    	// Number of tmcc pilots
    	int tmcc_carriers_size;

    	// Number of active carriers
    	int active_carriers;

    	// Number of segments
    	int d_number_of_segments;

    	// Number of carriers per segment
        int d_carriers_per_segment;

        // Number of data carriers per segment
        int d_data_carriers_per_segment;

    	// Number of symbols per frame
    	int d_symbols_per_frame;


        // Keeps the rcv TMCC data, is a FIFO
        std::deque<char> d_rcv_tmcc_data;
        // Keeps the TMCC sync sequence
        std::deque<char> d_tmcc_sync_evenv;
        std::deque<char> d_tmcc_sync_oddv;

        // Keep TMCC carriers values from previous symbol
        gr_complex * d_prev_tmcc_symbol;

        // See if the symbol index is known
        int d_symbol_index_known;

        // Symbol Index
        int d_symbol_index;

        int number_symbol;
        /*METHODS*/

        // TMCC data processing metod
        int process_tmcc_data(const gr_complex * in);

        // Method used to determinate if a particular carrier is a SP
        int is_sp_carrier(int carrier);


     public:
      tmcc_decoder_impl();
      ~tmcc_decoder_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_TMCC_DECODER_IMPL_H */

