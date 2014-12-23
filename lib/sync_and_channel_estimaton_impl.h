/* -*- c++ -*- */
/* 
 * Copyright 2014 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_ISDBT_SYNC_AND_CHANNEL_ESTIMATON_IMPL_H
#define INCLUDED_ISDBT_SYNC_AND_CHANNEL_ESTIMATON_IMPL_H

#include <isdbt/sync_and_channel_estimaton.h>

namespace gr {
  namespace isdbt {

    class sync_and_channel_estimaton_impl : public sync_and_channel_estimaton
    {
     private:

        //In and Out data length
        int d_ninput;
        int d_noutput;

        //FFT length
        int d_fft_length;

        // Number of zeros on the left of the IFFT
        int d_zeros_on_left;

        // Max frecuency offset to be corrected
        int d_freq_offset_max;

        // Number of continual pilots
        int tmcc_carriers_size;

        int active_carriers;

        // PRPS generator data buffer
        char d_wk[5617];

        float d_known_phase_diff[52-1];

        // It will be initialized after process_tmcc_data function
        int d_freq_offset;

        // Variable to keep corrected OFDM symbol
        // It will be initialized after process_tmcc_data function
        gr_complex * derotated_in;

        int is_sync_start(int nitems);

        gr_complex get_tmcc_value(int tmcc);

        // Generate PRBS
        void generate_prbs();

        void process_tmcc_data(const gr_complex * in);

        gr_complex * frequency_correction(const gr_complex * in, gr_complex * out);

     public:
      sync_and_channel_estimaton_impl();
      ~sync_and_channel_estimaton_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_SYNC_AND_CHANNEL_ESTIMATON_IMPL_H */

