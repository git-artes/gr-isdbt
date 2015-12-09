/* -*- c++ -*- */
/*  
 * Copyright 2015
 *   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
 *   Pablo Belzarena 
 *   Gabriel Gomez Sena 
 *   Pablo Flores Guridi 
 *   Victor Gonzalez Barbone
 * 
 *   Instituto de Ingenieria Electrica, Facultad de Ingenieria,
 *   Universidad de la Republica, Uruguay.
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
 * 
 */

#ifndef INCLUDED_ISDBT_OFDM_SYNCHRONIZATION_1SEG_IMPL_H
#define INCLUDED_ISDBT_OFDM_SYNCHRONIZATION_1SEG_IMPL_H

#include <isdbt/ofdm_synchronization_1seg.h>
#include <gnuradio/fft/fft.h>
#include <gnuradio/filter/mmse_fir_interpolator_cc.h>

namespace gr {
  namespace isdbt {

    class ofdm_synchronization_1seg_impl : public ofdm_synchronization_1seg
    {
         private:

                static const int d_carriers_per_segment_2k; 

                int d_fft_length;
                int d_cp_length;
                int d_active_carriers; 

                // for symbol time synchronization
                int d_cp_start;
                bool d_cp_found; 
                gr_complex * d_gamma;
                float * d_phi; 
                float * d_lambda;
                float d_peak_epsilon;
                gr_complex * d_derot;
                gr_complex * d_conj;
                float * d_norm;
                gr_complex * d_corr;
                //ml sync
                float d_snr;
                float d_rho;
                //peak detection
                float d_threshold_factor_rise;
                float d_avg_alpha;
                float d_avg_max;
                float d_avg_min; 
                //phase correction
                double d_phaseinc;
                double d_nextphaseinc;
                int d_nextpos;
                float d_phase; 
                
                bool d_initial_acquired; 

                int d_consumed; 
                int d_out; 

                // the complex input (length fft_len) with pre-fft synchronization applied. 
                gr_complex * d_prefft_synched; 

                //FFT part
                gr_complex * d_postfft; 
                gr::fft::fft_complex d_fft_calculator; 

                //integer frequency correction part
                int d_zeros_on_left;
                int d_freq_offset_max;
                int d_freq_offset; 
                int d_freq_offset_disagree_count; 
                int d_freq_offset_candidate; 

                int d_tmcc_carriers_size;
                const int * d_tmcc_carriers; 
                static const int tmcc_carriers_size_2k;
                static const int tmcc_carriers_2k[];
                static const int tmcc_carriers_size_4k;
                static const int tmcc_carriers_4k[];
                static const int tmcc_carriers_size_8k;
                static const int tmcc_carriers_8k[];
                
                int d_ac_carriers_size;
                const int * d_ac_carriers; 
                static const int ac_carriers_size_2k;
                static const int ac_carriers_2k[];
                static const int ac_carriers_size_4k;
                static const int ac_carriers_4k[];
                static const int ac_carriers_size_8k;
                static const int ac_carriers_8k[];
 
                float * d_known_phase_diff_tmcc;
                float * d_known_phase_diff_ac;
                gr_complex * d_integer_freq_derotated; 

                //symbol estimation and channel equalization part
                gr_complex * d_pilot_values; 
                int d_sp_carriers_size;
                gr_complex * d_channel_gain; 
                int d_current_symbol;
                int d_previous_symbol; 
                bool d_symbol_acq;
                int d_symbol_correct_count;
                float * d_corr_sp; 
         
               // fine frequency and symbol synchro 
                gr_complex * d_previous_channel_gain; 
                gr_complex * d_delta_channel_gains; 
                gr_complex * d_interpolated; 
                gr::filter::mmse_fir_interpolator_cc d_inter; 
                float d_samp_inc;
                float d_samp_phase; 
                int d_cp_start_offset; 

                float d_alpha_freq; 
                float d_beta_freq; 
                float d_alpha_timing; 
                float d_beta_timing; 
                float d_freq_aux; 
                float d_est_freq; 
                float d_delta_aux; 
                float d_est_delta;
                bool d_moved_cp;  

                void advance_freq_loop(float error); 
                
                void advance_delta_loop(float error); 

                int interpolate_input(const gr_complex * in, gr_complex * out);  

                /*!
                 * \brief Estimates post-fft synchronization parameters. 
                 */
                void estimate_fine_synchro(gr_complex * current_channel, gr_complex * previous_channel); 

               /*!
                * Signals downstream the symbol index and (if necessary) resynching. 
                */ 
                void send_symbol_index_and_resync(int current_offset); 

                 /*!
                 * \brief Calculates the channel taps based on pilots. This is the linear very simple implementation. 
                 */
                void linearly_estimate_channel_taps(int current_symbol, gr_complex * channel_gain);

                /*!
                 * \brief Calculates the channel taps at the SPs, given the input complex baseband signal and a symbol number 
                 * (between 0 and 3). The method that integer frequency offset has been corrected.  
                 */
                void calculate_channel_taps_sp(const gr_complex * in, int current_symbol, gr_complex * channel_gain);

                /*! 
                 * Estimates the current symbol. Necessary for equalization. 
                 */ 
                int estimate_symbol_index(const gr_complex * in); 
                
                /*!
                 * Given a fft_size (i.e. a mode), this method assigns an array where the position 
                 * of the tmcc and ac pilots are indicated. 
                 */
                void tmcc_and_ac_positions(int fft);

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
                 * \brief This method is responsible for the integer frequency error estimation. 
                 */
                int estimate_integer_freq_offset(const gr_complex * in); 

                /*!
                 * \brief It calculate the FFT of in and saves it on out. Note that the fft_size is assumed given by d_fft_length. 
                 */
                void calculate_fft(const gr_complex * in, gr_complex * out); 

                /*!
                 * \brief Calculates the likelihood function, and outputs the position of its maximum. 
                 *
                 * Given the input, it calculates the resulting likelihood function between indices lookup_stop and lookup_start. 
                 * It returns the beginning of the CP (in cp_pos), and an the value of epsilon (see the paper). 
                 * to_consume and to_out was used as indicators of whether the peak was correctly found or not. Now 
                 * the return value is used (either true or false). 
                 *
                 */
                bool ml_sync(const gr_complex * in, int lookup_start, int lookup_stop, int * cp_pos, float * peak_epsilon);

                /*!
                 * \brief Initializes the parameters used in the peak_detect_process. 
                 *
                 * \param threshold_factor_rise The algorithm keeps an average of minimum and maximum value. A peak is considered valid
                 * when it's bigger than avg_max - threshold_factor_rise(avg_max-avg_min). 
                 * \param alpha The parameter used to update both the average maximum and minimum (exponential filter, or single-root iir). 
                 *
                 */ 
                void peak_detect_init(float threshold_factor_rise, float alpha);

                /*!
                 * \brief Given datain and its length, the method return the peak position and its value in the form of an array (for 
                 * "historical" reasons, since it's always a single value). 
                 */ 
                bool peak_detect_process(const float * datain, const int datain_length, int * peak_pos);

                /*!
                 * \brief Sends a tag downstream that signals that acquisition was successfully performed (or that we lost synchronization 
                 * and we have regained it). 
                 */
                void send_sync_start();

                /*!
                 * \brief Using the vector derot calculated by ml_sync (an exponential modulated with minus the estimated frequency error), 
                 * this method simply multiplies it by the input and saves it in the output. 
                 */
                void derotate(const gr_complex * in, gr_complex *out);

     public:
      ofdm_synchronization_1seg_impl(int mode, float cp_length);
      ~ofdm_synchronization_1seg_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_OFDM_SYNCHRONIZATION_1SEG_IMPL_H */

