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


#ifndef INCLUDED_ISDBT_OFDM_SYM_ACQUISITION_IMPL_H
#define INCLUDED_ISDBT_OFDM_SYM_ACQUISITION_IMPL_H

#include <isdbt/ofdm_sym_acquisition.h>

namespace gr {
  namespace isdbt {

     /*!
     * \brief The block performs one-shot OFDM symbol and fractional frequency synchronization 
     * based on the classical method described by Jan-Jaap van de Beek et al. 
     * in "ML Estimation of Time and Frequency Offset in OFDM Systems" (IEEE Transactions on Signal 
     * Processing, vol. 45, no. 7, July 1997). 
     * 
     * \ingroup isdbt 
     *
     * This block receives the stream of complex baseband samples corresponding to several 
     * OFDM symbols (cyclic prefix included), and outputs a vector corresponding to the 
     * useful part of the symbol. Furthermore, coarse (or fractional) frequency synchronization 
     * is applied. Optionally, the frequency error estimate is output. 
     *
     * To improve performance, operation is separated into two stages: acquisition and tracking. In 
     * acquisition, the likelihood function is calculated for the complete symbol and CP. In tracking
     * (used after acquisition was acquired), likelihood is only calculated in a small interval around 
     * the last symbol-begin index. 
     *
     */
    class ofdm_sym_acquisition_impl : public ofdm_sym_acquisition
    {
     private:
      int d_fft_length;
      int d_cp_length;
      int d_extended_range;
      float d_snr;
      float d_rho;

      int d_index;

      int d_search_max;

      gr_complex * d_conj;
      float * d_norm;
      gr_complex * d_corr;
      gr_complex * d_gamma;
      float * d_lambda;
      float * d_arg;

      // For peak detector
      float d_threshold_factor_rise;
      float d_avg_alpha;
      float d_avg_max;
      float d_avg_min; 
      float d_phase;
      double d_phaseinc;
      int d_cp_found;
      int d_count;
      double d_nextphaseinc;
      int d_nextpos;

      int d_sym_acq_count;
      int d_sym_acq_timeout;

      int d_initial_aquisition;

      int d_freq_correction_count;
      int d_freq_correction_timeout;

      int d_cp_start;
      gr_complex * d_derot;
      int d_to_consume;
      int d_to_out;
      int d_consumed;
      int d_out;
      int d_cp_start_tmp;
      gr_complex * d_derot_tmp;
      int d_to_consume_tmp;
      int d_to_out_tmp;

      /*!
       * \brief Calculates the likelihood function, and outputs the position of its maximum. 
       *
       * Given the input, it calculates the resulting likelihood function between indices lookup_stop and lookup_start. 
       * It returns the beginning of the CP (in cp_pos), and an exponential modulated with minus the estimated frequency error (in 
       * the pointer derot). to_consume and to_out was used as indicators of whether the peak was correctly found or not. Now 
       * the return value is used (either true or false). 
       *
       */
      int ml_sync(const gr_complex * in, int lookup_start, int lookup_stop, int * cp_pos, gr_complex * derot, int * to_consume, int * to_out);
     
     /*!
      * \brief Initializes the parameters used in the peak_detect_process. 
      *
      * \param threshold_factor_rise The algorithm keeps an average of minimum and maximum value. A peak is considered valid
      * when it's bigger than avg_max - threshold_factor_rise(avg_max-avg_min). 
      * \param alpha The parameter used to update both the average maximum and minimum (exponential filter, or single-root iir). 
      *
      */ 
      int peak_detect_init(float threshold_factor_rise, float alpha);
     
      /*!
       * \brief Given datain and its length, the method return the peak position and its value in the form of an array (for 
       * "historical" reasons, since it's always a single value). 
       */ 
      int peak_detect_process(const float * datain, const int datain_length, int * peak_pos, int * peak_max);

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
      ofdm_sym_acquisition_impl(int fft_length, int cp_length, float snr);
      ~ofdm_sym_acquisition_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_OFDM_SYM_ACQUISITION_IMPL_H */

