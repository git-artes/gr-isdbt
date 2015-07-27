/* -*- c++ -*- */
/*  
 * Copyright 2013,2014,2015 <Bogdan Diaconescu, yo3iiu@yo3iiu.ro>.
 * Copyright 2015, minor modifications
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


#ifndef INCLUDED_ISDBT_OFDM_SYM_ACQUISITION_DVBT_IMPL_H
#define INCLUDED_ISDBT_OFDM_SYM_ACQUISITION_DVBT_IMPL_H

#include <isdbt/ofdm_sym_acquisition_dvbt.h>

namespace gr {
  namespace isdbt {

    class ofdm_sym_acquisition_dvbt_impl : public ofdm_sym_acquisition_dvbt
    {
     private:
      // Nothing to declare in this block.
      int d_fft_length;
      int d_cp_length;
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
      float d_threshold_factor_fall;
      float d_avg_alpha;
      float d_avg;
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

      int ml_sync(const gr_complex * in, int lookup_start, int lookup_stop, int * cp_pos, gr_complex * derot, int * to_consume, int * to_out);
      int cp_sync(const gr_complex * in, int * cp_pos, gr_complex * derot, int * to_consume, int * to_out);
      
      int peak_detect_init(float threshold_factor_rise, float threshold_factor_fall, int look_ahead, float alpha);
      
      int peak_detect_process(const float * datain, const int datain_length, int * peak_pos, int * peak_max);

      void send_sync_start();

     public:
      ofdm_sym_acquisition_dvbt_impl(int fft_length, int cp_length, float snr);
      ~ofdm_sym_acquisition_dvbt_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_OFDM_SYM_ACQUISITION_DVBT_IMPL_H */

