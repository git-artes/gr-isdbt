/* -*- c++ -*- */
/*  
 * Copyright 2015
 *   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
 *   Pablo Belzarena 
 *   Gabriel Gomez Sena 
 *   Pablo Flores Guridi 
 *  Victor Gonzalez Barbone
 *
 *  Instituto de Ingenieria Electrica, Facultad de Ingenieria,
 *  Universidad de la Republica, Uruguay.
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

#ifndef INCLUDED_ISDBT_SYNC_AND_CHANNEL_ESTIMATION_IMPL_H
#define INCLUDED_ISDBT_SYNC_AND_CHANNEL_ESTIMATION_IMPL_H

#include <isdbt/sync_and_channel_estimation.h>
#include <deque>
#include <boost/circular_buffer.hpp>

namespace gr {
    namespace isdbt {

        class sync_and_channel_estimation_impl : public sync_and_channel_estimation
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

                gr_complex * d_aux_linear_estimate_first; 
                gr_complex * d_aux_linear_estimate_last; 
                gr_complex * d_coeffs_linear_estimate_first; 
                gr_complex * d_coeffs_linear_estimate_last;

                float * d_channel_gain_mag_sq; 
                float * d_ones; 

                // It will be initialized after process_tmcc_data function
                int d_freq_offset;
                int d_freq_offset1;
                int d_freq_offset2;
                int d_freq_offset3;
                int d_freq_offset4;

                // Variable to keep corrected OFDM symbol
                // It will be initialized after process_tmcc_data function
                const gr_complex * d_derotated_in;

                // Past channel taps at the SPs. Used for interpolation (bigger means newer).
                gr_complex * d_channel_gain1; 
                gr_complex * d_channel_gain2; 
                gr_complex * d_channel_gain3; 
                gr_complex * d_channel_gain4; 

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
                 * \brief This method is responsible for the integer frequency error estimation. 
                 *
                 * To achieve this, for every possible value of frequency offset it will calculate a certain correlation 
                 * function, and calculate its maximum (see our webpage for further info on the algorithm). The correction 
                 * is left to a separate function. 
                 */
                int estimate_integer_freq_offset(const gr_complex * in);

//                gr_complex * frequency_correction(const gr_complex * in, gr_complex * out);

                /*!
                 * \brief Calculates the relative symbol index based on where are the SPs. 
                 *
                 * Similarly to the TMCC case, this method considers the four possible scattered pilot dispositions 
                 * and calculates a certain correlation (see our webpage for further info on the algorithm). 
                 */
                int estimate_symbol_index(const gr_complex * in);

                /*!
                 * \brief Calculates the channel taps at the SPs, given the input complex baseband signal and a symbol number 
                 * (between 0 and 3). The method that integer frequency offset has been corrected.  
                 */
                void calculate_channel_taps_sp(const gr_complex * in, int current_symbol);
                
                /*!
                 * \brief Calculates the channel taps based on pilots. This is the linear very simple implementation. 
                 */
                void linearly_estimate_channel_taps();

                /*!
                 * \brief Calculates the channel taps based on pilots. This is the quadratic simple implementation. 
                 */
                void quadratically_estimate_channel_taps();

                void linear_frequency_interpolation(); 
                
                void linear_time_interpolation(const gr_complex * in, int current_symbol); 


            public:
                sync_and_channel_estimation_impl(int fft_length, int payload_length, int offset_max);
                ~sync_and_channel_estimation_impl();

                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
        };

    } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_SYNC_AND_CHANNEL_estimation_IMPL_H */

