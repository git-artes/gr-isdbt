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
#include <deque>

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

                // to keep track of the SP correllation
                float * d_corr_sp; 

                static const int tmcc_carriers_size_2k;
                static const int tmcc_carriers_2k[];
                static const int tmcc_carriers_size_4k;
                static const int tmcc_carriers_4k[];
                static const int tmcc_carriers_size_8k;
                static const int tmcc_carriers_8k[];

                int d_tmcc_carriers_size;
                const int * d_tmcc_carriers;

                // Number of sp pilots
                int d_sp_carriers_size;

                int d_active_carriers;

                // PRPS generator data buffer
                char * d_wk;
                // the value of a hypothetical pilot for all possible carriers
                gr_complex * d_pilot_values; 

                float * d_known_phase_diff;

                gr_complex * d_channel_gain; 

                // It will be initialized after process_tmcc_data function
                int d_freq_offset;

                // Variable to keep corrected OFDM symbol
                // It will be initialized after process_tmcc_data function
                const gr_complex * d_derotated_in;

                // indicates the symbol relative index calculated from the processing the SPs
                int d_current_symbol;
                int d_previous_symbol; 

                /*!
                 * Given a carrier index, this method returns the value that a pilot would have in 
                 * such position. 
                 */
                gr_complex get_pilot_value(int index);

                /*!
                 * This method constructs the pseudo-random sequence wk used by the standard. 
                 */
                void generate_prbs();

                /*!
                 * Given a fft_size (i.e. a mode), this method assigns an array where the position 
                 * of the tmcc pilots are indicated. 
                 */
                void tmcc_positions(int fft);

                /*!
                 * \brief This method is responsible for the integer frequency correction. 
                 *
                 * To achieve this, for every possible value of frequency offset it will calculate a certain correlation 
                 * function, and calculate its maximum (see our webpage for further info on the algorithm). The estimated 
                 * frequency offset is stored in d_freq_offset. The correction is left to a separate function. 
                 */
                void process_tmcc_data(const gr_complex * in);

//                gr_complex * frequency_correction(const gr_complex * in, gr_complex * out);

                /*!
                 * \brief Calculates the relative symbol index based on where are the SPs and then 
                 * estimates the channel taps for the current symbol in the SPs positions. 
                 *
                 * Similarly to the TMCC case, this method considers the four possible scattered pilot dispositions 
                 * and calculates a certain correlation (see our webpage for further info on the algorithm). Once the 
                 * SP are located channel taps are calculated for the current scattered pilots.  
                 */
                void process_sp_data(const gr_complex * in);
                
                /*!
                 * \brief Calculates the channel taps based on pilots. This is the linear very simple implementation. 
                 */
                void linearly_estimate_channel_taps();

                /*!
                 * \brief Calculates the channel taps based on pilots. This is the quadratic simple implementation. 
                 */
                void quadratically_estimate_channel_taps();


            public:
                sync_and_channel_estimaton_impl(int fft_length, int payload_length, int offset_max);
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

