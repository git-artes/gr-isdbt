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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "ofdm_synchronization_impl.h"
#include <complex>
#include <math.h>
#include <gnuradio/math.h>
#include <gnuradio/expj.h>
#include <stdio.h>
#include <volk/volk.h>
//for debugging 
#include <ctime>

namespace gr {
    namespace isdbt {

        const int ofdm_synchronization_impl::d_total_segments = 13;
        const int ofdm_synchronization_impl::d_carriers_per_segment_2k = 108;  

        // TMCC carriers positions for each transmission mode
        // Mode 1 (2K)
        const int ofdm_synchronization_impl::tmcc_carriers_size_2k = 13;
        const int ofdm_synchronization_impl::tmcc_carriers_2k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319
        };
        // Mode 2 (4K)
        const int ofdm_synchronization_impl::tmcc_carriers_size_4k = 26;
        const int ofdm_synchronization_impl::tmcc_carriers_4k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319, 1474, 1537, 1637,\
                1814, 1880, 1991, 2101,	2191, 2351, 2437, 2569,\
                2693, 2723
        };
        // Mode 3 (8K)
        const int ofdm_synchronization_impl::tmcc_carriers_size_8k = 52;
        const int ofdm_synchronization_impl::tmcc_carriers_8k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319, 1474, 1537, 1637,\
                1814, 1880, 1991, 2101, 2191, 2351, 2437, 2569,\
                2693, 2723, 2878, 2941, 3041, 3218, 3284, 3395,\
                3505, 3595, 3755, 3841, 3973, 4097, 4127, 4282,\
                4345, 4445, 4622, 4688, 4799, 4909, 4999, 5159,\
                5245, 5377, 5501, 5531
        };


        ofdm_synchronization::sptr
            ofdm_synchronization::make(int mode, float cp_length, bool interpolate)
            {
                return gnuradio::get_initial_sptr
                    (new ofdm_synchronization_impl(mode, cp_length, interpolate));
            }

        /*
         * The private constructor
         */
        ofdm_synchronization_impl::ofdm_synchronization_impl(int mode, float cp_length, bool interpolate)
            : gr::block("ofdm_synchronization",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)),
                    //gr::io_signature::make(1,1,sizeof(gr_complex)*pow(2.0,10+mode))),
                    gr::io_signature::make3(1, 4, \
                            sizeof(gr_complex)*(1+d_total_segments*d_carriers_per_segment_2k*((int)pow(2.0,mode-1))),\
                            sizeof(gr_complex)*(1+d_total_segments*d_carriers_per_segment_2k*((int)pow(2.0,mode-1))),\
                            sizeof(float))

              ),
              d_fft_calculator(gr::fft::fft_complex(pow(2.0,10+mode),true,1)), 
              d_inter(gr::filter::mmse_fir_interpolator_cc())
              {
                  // TODO why must fft_complex be initialized in the constructor declaration????

                  //d_fft_length = 1+d_total_segments*d_carriers_per_segment_2k*((int)pow(2.0,mode-1)); 
                  d_fft_length = pow(2.0,10+mode); 
                  d_cp_length = (int)round(cp_length*d_fft_length); 
                  d_active_carriers = (1+d_total_segments*d_carriers_per_segment_2k*pow(2.0,mode-1)); 

                  set_relative_rate(1.0 / (double) (d_cp_length + d_fft_length));

                  //TODO set this value automatically. 
                  float d_snr = 10.0; 
                  d_snr = pow(10, d_snr / 10.0);
                  d_rho = d_snr / (d_snr + 1.0);

                  d_initial_acquired = false; 

                  //VOLK alignment as recommended by GNU Radio's Manual. It has a similar effect 
                  //than set_output_multiple(), thus we will generally get multiples of this value
                  //as noutput_items. 
                  const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
                  set_alignment(std::max(1, alignment_multiple));

                  d_gamma = new gr_complex[d_fft_length];
                  d_phi = new float[d_fft_length];
                  d_lambda = new float[d_fft_length];
                  d_derot = new gr_complex[d_cp_length + d_fft_length];
                  d_phaseinc = 0; 
                  d_nextphaseinc = 0; 
                  d_nextpos = 0; 
                  d_phase = 0; 
                  d_conj = new gr_complex[2 * d_fft_length + d_cp_length];
                  d_norm = new float[2 * d_fft_length + d_cp_length];
                  d_corr = new gr_complex[2 * d_fft_length + d_cp_length];
                  peak_detect_init(0.3, 0.9);

                  d_postfft = new gr_complex[d_fft_length];

                  //integer frequency correction part
                  d_zeros_on_left = int(ceil((d_fft_length-d_active_carriers)/2.0)); 
                  d_freq_offset_max = 10; 
                  tmcc_positions(d_fft_length); 
                  d_pilot_values = new gr_complex[d_active_carriers];
                  d_known_phase_diff = new float[d_tmcc_carriers_size];
                  generate_prbs();
                  // Obtain phase diff for all tmcc pilots
                  // TODO eliminate d_known_phase_diff
                  for (int i = 0; i < (d_tmcc_carriers_size - 1); i++)
                  {
                      d_known_phase_diff[i] = std::norm(d_pilot_values[d_tmcc_carriers[i + 1]] - d_pilot_values[d_tmcc_carriers[i]]);
                  }

                  d_sp_carriers_size = (d_active_carriers-1)/12; 
                  d_corr_sp = new float[4]; 

                  d_moved_cp = true; 
                  d_current_symbol = 0; 
                  d_peak_epsilon = 0;

                  d_channel_gain = new gr_complex[d_active_carriers]; 
                  d_channel_gain_mag_sq = new float[d_active_carriers];
                  d_ones = new float[d_active_carriers];
                  for(int i=0; i<d_active_carriers; i++)
                  {
                      d_ones[i] = 1.0;
                  }
                  d_channel_gain_inv = new gr_complex[d_active_carriers];

                  d_coeffs_linear_estimate_first = new gr_complex[11]; 
                  d_aux_linear_estimate_first = new gr_complex[11]; 
                  d_coeffs_linear_estimate_last = new gr_complex[11]; 
                  d_aux_linear_estimate_last = new gr_complex[11]; 
                  for (int i=1; i<12; i++){
                      d_coeffs_linear_estimate_first[i-1] = gr_complex(1.0-i/12.0,0.0);
                      d_coeffs_linear_estimate_last[i-1] = gr_complex(i/12.0,0.0);
                  }

                  d_previous_channel_gain = new gr_complex[d_active_carriers]; 
                  d_delta_channel_gains = new gr_complex[d_active_carriers]; 
                  d_samp_inc = 1; 
                  d_samp_phase = 0; 
                  //d_interpolated = new gr_complex[2*d_fft_length+d_cp_length]; 
                  d_interpolated = new gr_complex[d_fft_length+d_cp_length]; 
                  d_interpolate = interpolate; 
                  d_cp_start_offset = -10; 

                  float loop_bw_freq = 3.14159/100; 
                  float loop_bw_timing = 3.14159/10000; 
                  float damping = sqrtf(2.0f)/2.0f; 
                  float denom = (1.0 + 2.0*damping*loop_bw_freq + loop_bw_freq*loop_bw_freq); 
                  d_beta_freq = (4*loop_bw_freq*loop_bw_freq)/denom; 
                  d_alpha_freq = (4*damping*loop_bw_freq)/denom; 
                  denom = (1.0 + 2.0*damping*loop_bw_timing + loop_bw_timing*loop_bw_timing); 
                  d_beta_timing = (4*loop_bw_timing*loop_bw_timing)/denom; 
                  d_alpha_timing = (4*damping*loop_bw_timing)/denom; 
                  d_freq_aux = 0; 
                  d_est_freq = 0; 

              }


        /*
         * Our virtual destructor.
         */
        ofdm_synchronization_impl::~ofdm_synchronization_impl()
        {
            delete [] d_gamma; 
            delete [] d_phi; 
            delete [] d_lambda;  
            delete [] d_derot; 
            delete [] d_conj; 
            delete [] d_norm; 
            delete [] d_corr; 

            delete [] d_postfft; 

            delete [] d_pilot_values; 
            delete [] d_known_phase_diff; 

            delete [] d_channel_gain; 
            delete [] d_channel_gain_mag_sq;
            delete [] d_ones;
            delete [] d_channel_gain_inv;

            delete [] d_coeffs_linear_estimate_first; 
            delete [] d_aux_linear_estimate_first; 
            delete [] d_coeffs_linear_estimate_last; 
            delete [] d_aux_linear_estimate_last; 

            delete [] d_previous_channel_gain; 
            delete [] d_delta_channel_gains; 

            delete [] d_interpolated; 

        }

        void
            ofdm_synchronization_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
            {
                int ninputs = ninput_items_required.size ();
                // make sure we receive at least (symbol_length + fft_length)
                for (int i = 0; i < ninputs; i++)
                {
                    //ninput_items_required[i] = ( d_cp_length + d_fft_length ) * (noutput_items + 1) ;
                    ninput_items_required[i] = (int)ceil(( d_cp_length + d_fft_length ) * (noutput_items + 1) * d_samp_inc) + d_inter.ntaps() ;
                }

            }


        void
            ofdm_synchronization_impl::generate_prbs()
            {
                // Generate the prbs sequence for each active carrier

                // Init PRBS register with 1s (reg_prbs = 111111111111)
                unsigned int reg_prbs = (1 << 11) - 1;
                char aux; 
                for (int k = 0; k < (d_active_carriers); k++)
                {
                    aux = (char)(reg_prbs & 0x01); // Get the LSB of the register as a character
                    int new_bit = ((reg_prbs >> 2) ^ (reg_prbs >> 0)) & 0x01; // This is the bit we will add to the register as MSB
                    reg_prbs = (reg_prbs >> 1) | (new_bit << 10); // We movo all the register to the right and add the new_bit as MSB
                    d_pilot_values[k] = gr_complex((float)(4 * 2 * (0.5 - aux)) / 3, 0);
                }
            }

        void
            ofdm_synchronization_impl::tmcc_positions(int fft)
            {
                /*
                 * Assign to variables tmcc_carriers and tmcc_carriers_size
                 * the corresponding values according to the transmission mode
                 */
                switch (fft)
                {
                    case 2048:
                        {
                            d_tmcc_carriers = tmcc_carriers_2k;
                            d_tmcc_carriers_size = tmcc_carriers_size_2k;
                        }
                        break;
                    case 4096:
                        {
                            d_tmcc_carriers = tmcc_carriers_4k;
                            d_tmcc_carriers_size = tmcc_carriers_size_4k;
                        }
                        break;
                    case 8192:
                        {
                            d_tmcc_carriers = tmcc_carriers_8k;
                            d_tmcc_carriers_size = tmcc_carriers_size_8k;
                        }
                        break;
                    default:
                        break;
                }
            }

        int ofdm_synchronization_impl::interpolate_input(const gr_complex * in, gr_complex * out){
            int ii = 0; // input index
            int oo = 0; // output index

            double s, f; 
            int incr; 
            while(oo < d_cp_length+d_fft_length) {
                out[oo++] = d_inter.interpolate(&in[ii], d_samp_phase);

                s = d_samp_phase + d_samp_inc;
                f = floor(s);
                incr = (int)f;
                d_samp_phase = s - f;
                ii += incr;
            }

            // return how many inputs we required to generate d_cp_length+d_fft_length outputs 
            return ii;
        }


        void ofdm_synchronization_impl::advance_freq_loop(float error){
            d_freq_aux = d_freq_aux + d_beta_freq*error; 
            d_est_freq = d_est_freq + d_freq_aux + d_alpha_freq*error; 
        }

        void ofdm_synchronization_impl::advance_delta_loop(float error){
            d_delta_aux = d_delta_aux + d_beta_timing*error; 
            d_est_delta = d_est_delta + d_delta_aux + d_alpha_timing*error; 
        }

        void
            ofdm_synchronization_impl::estimate_fine_synchro(gr_complex * current_channel, gr_complex * previous_channel)
            {

                gr_complex result_1st = gr_complex(0.0, 0.0);  
                volk_32fc_x2_conjugate_dot_prod_32fc(&result_1st, &current_channel[0], &previous_channel[0], floor(d_active_carriers/2.0));
                gr_complex result_2nd = gr_complex(0.0, 0.0);  
                int low = (int)floor(d_active_carriers/2.0); 
                volk_32fc_x2_conjugate_dot_prod_32fc(&result_2nd, &current_channel[low], &previous_channel[low], d_active_carriers-low);
                float delta_est_error = 1.0/(1.0+((float)d_cp_length)/d_fft_length)/(2.0*3.14159*d_active_carriers/2.0)*std::arg(result_2nd*std::conj(result_1st)) ; 
                float freq_est_error  = (std::arg(result_1st)+std::arg(result_2nd))/2.0/(1.0+(float)d_cp_length/d_fft_length); 

                // if for any reason the CP position changed, the signal error is wrong and should not be fed to the loop. 
                if( !d_moved_cp )
                {
                    advance_delta_loop(delta_est_error);
                    advance_freq_loop(freq_est_error);
                }
                //printf("freq_est_error: %e, d_cp_start_offset:%i, d_est_freq:%f, d_freq_offset:%i, d_peak_epsilon:%e\n", freq_est_error, d_cp_start_offset, d_est_freq, d_freq_offset, d_peak_epsilon); 

                d_samp_inc = 1.0-d_est_delta; 
                d_peak_epsilon = d_est_freq;  

            }

        void ofdm_synchronization_impl::send_symbol_index_and_resync(int current_offset)
        {
            int diff = d_current_symbol-d_previous_symbol;
            // If there is any symbol lost print stuff
            if ((diff != 1) && (diff !=-3)){
                printf("previous_symbol: %i, \n current_symbol: %i\n", d_previous_symbol, d_current_symbol);
                printf("d_corr_sp = {%f, %f, %f, %f}\n",d_corr_sp[0], d_corr_sp[1], d_corr_sp[2], d_corr_sp[3]); 
                for (int carrier = 0; carrier < d_active_carriers; carrier++)
                {
                    //printf("out(%d)=%f+j*(%f); \n",i*d_noutput+carrier+1,out[i*d_noutput+carrier].real(),out[i*d_noutput+carrier].imag());
                }
                // if a symbol skip was detected, we should signal it downstream
                const uint64_t offset = this->nitems_written(0)+current_offset; 
                pmt::pmt_t key = pmt::string_to_symbol("resync"); 
                pmt::pmt_t value = pmt::string_to_symbol("generated by sync and channel estimation"); 
                this->add_item_tag(0,offset,key,value); 
            }

            // signal downstream the relative symbol index found here. 
            const uint64_t offset = this->nitems_written(0)+current_offset; 
            pmt::pmt_t key = pmt::string_to_symbol("relative_symbol_index"); 
            pmt::pmt_t value = pmt::from_long(d_current_symbol); 
            this->add_item_tag(0,offset,key,value);

        }

        void 
            ofdm_synchronization_impl::linearly_estimate_channel_taps(int current_symbol, gr_complex * channel_gain)
            {
                // This method interpolates scattered measurements across one OFDM symbol
                // It does not use measurements from the previous OFDM symbols (does not use history)
                // as it may have encountered a phase change for the current phase only
                // TODO interpolation is too simple, a new method(s) should be implemented
                    
                // Current sp carrier
                int current_sp_carrier = 3*current_symbol; 
                // Next sp carrier
                int next_sp_carrier = 12 + 3*current_symbol; 
                for (int i = 0; i < d_sp_carriers_size-1; i++)
                {

                    volk_32fc_s32fc_multiply_32fc(d_aux_linear_estimate_first, d_coeffs_linear_estimate_first, channel_gain[current_sp_carrier], 11); 
                    volk_32fc_s32fc_multiply_32fc(d_aux_linear_estimate_last, d_coeffs_linear_estimate_last, channel_gain[next_sp_carrier], 11); 
                    for (int j = 1; j < 12; j++)
                    {
                        channel_gain[current_sp_carrier+j] = d_aux_linear_estimate_first[j-1] + d_aux_linear_estimate_last[j-1];
                    }
 
                    current_sp_carrier += 12;
                    next_sp_carrier += 12;

                }
                /////////////////////////////////////////////////////////////
                //take care of extreme cases: first carriers and last carriers
                /////////////////////////////////////////////////////////////
                //I did not optimize this part since they're only a few carriers
                
                gr_complex tg_alpha; 
                if (current_symbol>0){
                    //we have not updated the gain on the first carriers
                    //we now do this with a very simple linear interpolator 
                    //TODO is this a good estimation??
                    current_sp_carrier = 3*current_symbol;
                    next_sp_carrier = 12+3*current_symbol;
                    tg_alpha = (channel_gain[next_sp_carrier] - channel_gain[current_sp_carrier]) / gr_complex(next_sp_carrier-current_sp_carrier, 0.0);
                    // Calculate interpolation for all intermediate values
                    for (int j = -current_sp_carrier; j < 0; j++)
                    {
                        channel_gain[current_sp_carrier+j] = channel_gain[current_sp_carrier] + tg_alpha * gr_complex(j, 0.0);
                    }
                }

                // now the other extreme case: the last carriers. 
                // we will use the last SP and the CP
                current_sp_carrier = 12*(d_sp_carriers_size-1)+3*current_symbol;
                next_sp_carrier = d_active_carriers-1;
                float delta_carriers =(float) next_sp_carrier-current_sp_carrier;
                for (int j = 1; j < delta_carriers; j++)
                {
                    channel_gain[current_sp_carrier+j] = channel_gain[current_sp_carrier]*gr_complex(1.0-j/delta_carriers,0.0) +  channel_gain[next_sp_carrier]*gr_complex(j/delta_carriers,0.0) ;
                }
            }

        void ofdm_synchronization_impl::calculate_channel_taps_sp(const gr_complex * in, int current_symbol, gr_complex * channel_gain)
        {
            // We first calculate the channel gain on the SP carriers.
            // We get each sp carrier position. We now know which is the current symbol (0, 1, 2 or 3)
            for (int current_sp_carrier = 3*current_symbol; current_sp_carrier<d_active_carriers-1; current_sp_carrier+=12)
            {
                channel_gain[current_sp_carrier] = in[current_sp_carrier]/d_pilot_values[current_sp_carrier];
            }
            //we then calculate the gain on the CP
            channel_gain[d_active_carriers-1] = in[d_active_carriers-1]/d_pilot_values[d_active_carriers-1];
       }


        int 
            ofdm_synchronization_impl::estimate_symbol_index(const gr_complex * in)
            {
                /*************************************************************/
                // Find out the OFDM symbol index (value 0 to 3) sent
                // in current block by correlating scattered symbols with
                // current block - result is (symbol index % 4)
                /*************************************************************/
                float max = 0;
                gr_complex sum = 0;
                int current_symbol = 0;

                int current_sp_carrier; // The current sp carrier
                // sym_count (Symbol count) can take values 0 to 3, according to the positions of the scattered pilots
                for (int sym_count = 0; sym_count < 4; sym_count++)
                {
                    sum = 0;
                    //The position of the next and current sp carrier based on the value of sym_count
                    // For every scattered pilot but the last one...
                    for (int next_sp_carrier=12+3*sym_count; next_sp_carrier < d_active_carriers-1; next_sp_carrier+=12)
                    {
                        current_sp_carrier = next_sp_carrier-12;
                        if (d_pilot_values[next_sp_carrier]==d_pilot_values[current_sp_carrier])
                        {
                            // If the phase difference between in[next_sp_carrier] and in[current_sp_carrier] is zero,
                            // is because we expect both to be always in phase
                            sum += in[next_sp_carrier]*conj(in[current_sp_carrier]);
                        }
                        else
                        {
                            // If the phase difference between in[next_sp_carrier] and in[current_sp_carrier] is not zero,
                            // is because we expect both to be always out of phase (180 degrees)
                            sum -= in[next_sp_carrier]*conj(in[current_sp_carrier]);
                        }

                        //sum += d_pilot_values[next_sp_carrier]*d_pilot_values[current_sp_carrier]*in[next_sp_carrier]*conj(in[current_sp_carrier]);
                    }
                    d_corr_sp[sym_count] = abs(sum); 
                    if (abs(sum)>max)
                    {
                        // When sum is maximum is because the current symbol is of type sym_count (0, 1, 2 or 3)
                        max = abs(sum);
                        current_symbol = sym_count;
                    }
                }
                return current_symbol; 

            }

        int
            ofdm_synchronization_impl::estimate_integer_freq_offset(const gr_complex * in)
            {
                // Look for maximum correlation for tmccs
                // in order to obtain postFFT integer frequency correction
                //
                // TODO separate in two phases: acquisition and tracking. Acquisition would simply search a bigger range. 
                // TODO use the channel taps in the correlation??

                float max = 0;
                gr_complex sum = 0;
                int start = 0;

                // for d_zeros_on_left +/- d_freq_offset_max...
                for (int i = d_zeros_on_left - d_freq_offset_max; i < d_zeros_on_left + d_freq_offset_max; i++)
                {
                    sum = 0;
                    for (int j = 0; j < (d_tmcc_carriers_size - 1); j++)
                    {

                        if (d_known_phase_diff[j] == 0)
                        {
                            // If the phase difference between tmcc_carriers[j+1] and tmcc_carriers[j] is zero, 
                            // is because we expect both to be always in phase
                            sum += in[i + d_tmcc_carriers[j + 1]]*conj(in[i + d_tmcc_carriers[j]]);
                        }
                        else
                        {
                            // If the phase difference between tmcc_carriers[j+1] and tmcc_carriers[j] is not zero, 
                            // is because we expect both to be always out of phase (180 degrees)
                            sum -= in[i + d_tmcc_carriers[j + 1]]*conj(in[i + d_tmcc_carriers[j]]);
                        }
                    }
                    if (abs(sum) > max)
                    {
                        // When sum is maximum is because in 'i' we have the first active carrier
                        max = abs(sum);
                        start = i;
                    }
                }
                // We get the integer frequency offset
                return  (start - d_zeros_on_left);
                //printf("freq offset: %d\n",d_freq_offset);

            }


        void
            ofdm_synchronization_impl::calculate_fft(gr_complex * out)
            {
                // I assume the input data has already been copied into the calculator's buffer
                //memcpy(d_fft_calculator.get_inbuf(), in, sizeof(gr_complex)*d_fft_length); 
                //calculate the FFT
                d_fft_calculator.execute(); 

                // I have to perform an fftshift
                unsigned int len = (unsigned int)(ceil(d_fft_length/2.0)); 
                memcpy(&out[0], &d_fft_calculator.get_outbuf()[len], sizeof(gr_complex)*(d_fft_length-len)); 
                memcpy(&out[d_fft_length-len], &(d_fft_calculator.get_outbuf()[0]), sizeof(gr_complex)*len);

            }

        void
            ofdm_synchronization_impl::peak_detect_init(float threshold_factor_rise, float alpha)
            {
                d_avg_alpha = alpha;
                d_threshold_factor_rise = threshold_factor_rise;
                // d_avg = 0;
                d_avg_max = - (float)INFINITY;
                d_avg_min =   (float)INFINITY;
            }

        void
            ofdm_synchronization_impl::send_sync_start()
            {
                const uint64_t offset = this->nitems_written(0);
                pmt::pmt_t key = pmt::string_to_symbol("sync_start");
                pmt::pmt_t value = pmt::from_long(1);
                this->add_item_tag(0, offset, key, value);
            }



        bool
            ofdm_synchronization_impl::peak_detect_process(const float * datain, const int datain_length, int * peak_pos)
            {
                unsigned int peak_index = 0;
                bool success = true;

                volk_32f_index_max_16u(&peak_index, &datain[0], datain_length); 

                if (datain_length>=d_fft_length){
                    float min = datain[(peak_index + d_fft_length/2) % d_fft_length];
                    if(d_avg_min==(float)INFINITY){
                        d_avg_min = min;
                    }
                    else 
                    {
                        d_avg_min = d_avg_alpha*min + (1-d_avg_alpha)*d_avg_min;
                    }

                }

                if (d_avg_max==-(float)INFINITY)
                {
                    // I initialize the d_avg with the first value. 
                    d_avg_max = datain[peak_index];
                }
                else if ( datain[ peak_index ] > d_avg_max - d_threshold_factor_rise*(d_avg_max-d_avg_min) ) 
                {
                    d_avg_max = d_avg_alpha * datain[ peak_index ] + (1 - d_avg_alpha) * d_avg_max;
                }
                else
                {
                    success = false; 
                    printf("OFDM_SYNCHRO: peak under/over average! peak %f, avg_max %f, avg_min %f\n", datain[ peak_index ], d_avg_max, d_avg_min); 
                }

                //We now check whether the peak is in the border of the search interval. This would mean that 
                //the search interval is not correct, and it should be re-set. This happens for instance when the 
                //hardware dropped some samples. 
                //Our definition of "border of the search interval" depends if the search interval is "big" or not. 
                if ( datain_length < d_fft_length )
                {
                    if ( ( peak_index == 0 ) || ( peak_index == datain_length-1 ) )
                    {
                        success = false; 
                        printf("OFDM_SYNCHRO: peak at border! peak %f, avg_max %f, avg_min %f, peak_index: %i\n", datain[ peak_index ], d_avg_max, d_avg_min, peak_index); 
                    }
                }
                else
                {
                    if ( ( peak_index < 5 ) || ( peak_index > datain_length-5 ) )
                    {
                        success = false; 
                        printf("OFDM_SYNCHRO: peak at border! peak %f, avg_max %f, avg_min %f, peak_index: %i\n", datain[ peak_index ], d_avg_max, d_avg_min, peak_index); 
                    }
                }

                *peak_pos = peak_index;
                return (success);
            }


        bool
            ofdm_synchronization_impl::ml_sync(const gr_complex * in, int lookup_start, int lookup_stop, int * cp_pos, float * peak_epsilon)
            {

                assert(lookup_start >= lookup_stop);
                assert(lookup_stop >= (d_cp_length + d_fft_length - 1));

                int low, size;

                // Calculate norm
                low = lookup_stop - (d_cp_length + d_fft_length - 1);
                size = lookup_start - low + 1;
                volk_32fc_magnitude_squared_32f(&d_norm[low], &in[low], size);

                // Calculate gamma on each point
                //TODO check these boundaries!!!!!!!
                low = lookup_stop - (d_cp_length - 1);
                //low = lookup_stop - d_cp_length - 1;
                //size = lookup_start - (lookup_stop - d_cp_length - 1) + 1;
                size = lookup_start - low + 1;
                volk_32fc_x2_multiply_conjugate_32fc(&d_corr[low - d_fft_length], &in[low], &in[low - d_fft_length], size);

                // Calculate time delay and frequency correction
                float sum_aux = 0;
                for (int i = lookup_start - 1; i >= lookup_stop; i--)
                {
                    int k = i - lookup_stop;

                    //TODO no accumulator for complexes in VOLK for the moment.    
                    volk_32fc_32f_dot_prod_32fc(&d_gamma[k], &d_corr[i-d_cp_length+1-d_fft_length], d_ones, d_cp_length);
                    volk_32f_accumulator_s32f(&d_phi[k], &d_norm[i-d_cp_length+1], d_cp_length);
                    volk_32f_accumulator_s32f(&sum_aux, &d_norm[i-d_cp_length+1-d_fft_length], d_cp_length);
                    d_phi[k] += sum_aux;
                }

                // Init lambda with gamma
                low = 0;
                size = lookup_start - lookup_stop;
                volk_32fc_magnitude_32f(&d_lambda[low], &d_gamma[low], size);

                // Calculate lambda
                low = 0;
                size = lookup_start - lookup_stop;
                volk_32f_s32f_multiply_32f(&d_phi[low], &d_phi[low], d_rho / 2.0, size);
                volk_32f_x2_subtract_32f(&d_lambda[low], &d_lambda[low], &d_phi[low], size);

                int peak = 0;
                bool found_peak = true; 

                // Find peaks of lambda
                // We have found an end of symbol at peak_pos[0] + CP + FFT
                if ((found_peak = peak_detect_process(&d_lambda[0], (lookup_start - lookup_stop), &peak)))
                {
                    *cp_pos = peak + lookup_stop;

                    // Calculate frequency correction
                    *peak_epsilon = fast_atan2f(d_gamma[peak]);
                }
                return (found_peak);
            }

        // Derotates the signal 
        void 
            ofdm_synchronization_impl::derotate(const gr_complex * in, gr_complex * out)
            {
                double sensitivity = (double)(-1) / (double)d_fft_length;
                d_phaseinc = sensitivity * d_peak_epsilon;

                gr_complex phase_increment = gr_complex(std::cos(d_phaseinc), std::sin(d_phaseinc)); 
                gr_complex phase_current = gr_complex(std::cos(d_phase), std::sin(d_phase)); 

                volk_32fc_s32fc_x2_rotator_32fc(&out[0], &in[0], phase_increment, &phase_current, d_fft_length) ; 
                d_phase = std::arg(phase_current); 
                d_phase = fmod(d_phase + d_phaseinc*d_cp_length, (float)2*M_PI);
           }


        int
            ofdm_synchronization_impl::general_work (int noutput_items,
                    gr_vector_int &ninput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const gr_complex *in = (const gr_complex *) input_items[0];
                gr_complex *out = (gr_complex *) output_items[0];
                gr_complex *out_channel_gain = (gr_complex *)output_items[1]; 
                float *out_freq_error = (float *)output_items[2]; 
                float *out_samp_error = (float *)output_items[3]; 

                bool ch_output_connected = output_items.size()>=2; 
                bool freq_error_output_connected = output_items.size()>=3; 
                bool samp_error_output_connected = output_items.size()>=4; 

                d_consumed = 0;
                d_out = 0;


                for (int i = 0; i < noutput_items ; i++) 
                {
                    int required_for_interpolation = d_cp_length + d_fft_length; 

                    if (!d_initial_acquired)
                    {
                        // If we are here it means that we have no idea where the CP may be. We thus 
                        // search it thoroughly. We also perform a coarse frequency estimation. 
                        d_initial_acquired = ml_sync(&in[d_consumed], 2 * d_fft_length + d_cp_length - 1, d_fft_length + d_cp_length - 1, &d_cp_start, &d_peak_epsilon);
                        required_for_interpolation = d_cp_length + d_fft_length; 
                        d_cp_found = d_initial_acquired; 
                        d_samp_phase = 0; 
                        d_moved_cp = true; 
                        //d_samp_inc = 1.0; 
                        //d_est_freq = d_peak_epsilon; 
                        //d_est_delta = 0; 

                    }
                    else
                    {
                        //If we are here it means that in the previous iteration we found the CP. We
                        //now thus only search near it. In fact, we use this only to check whether the 
                        //CP is still present (it may well happen that the USRP drops samples in which
                        //case the peak in the correlation is not present). We thus do not use the corresponding
                        //estimates of frequency and CP position. 
                        int cp_start_aux; 
                        float peak_epsilon_aux; 
                        d_cp_found = ml_sync(&in[d_consumed], d_cp_start + 8, std::max(d_cp_start - 8,d_cp_length+d_fft_length-1), &cp_start_aux, &peak_epsilon_aux);
                        //d_cp_start = cp_start_aux; 
                        //d_peak_epsilon = peak_epsilon_aux; 

                        if ( !d_cp_found )
                        {
                            // We may have not found the CP because the smaller search range was too small (rare, but possible, in 
                            // particular when sampling time error are present). We thus re-try with a bigger search range and 
                            // update d_cp_start. 
                            d_cp_found = ml_sync(&in[d_consumed], d_cp_start+16, std::max(d_cp_start-16,d_cp_length+d_fft_length-1), \
                                    &d_cp_start, &peak_epsilon_aux);
                            d_samp_phase = 0; 
                            d_moved_cp = true;
                        }

                    }

                    if ( d_cp_found )
                    {
                        // safe-margin. Using a too adjusted CP position may result in taking samples from the NEXT ofdm 
                        // symbol. It is better to stay on the safe-side (plus, 10 samples is nothing in this context). 
                        d_cp_start_offset = -10;  

                        /*
                           int low = d_consumed + d_cp_start + d_cp_start_offset - d_fft_length + 1 ;
                           derotate(&in[low], &d_prefft_synched[0]);
                           */

                        int low = d_cp_start + d_cp_start_offset - d_fft_length + 1 ;
                       
                        if (d_interpolate) {
                            // I interpolate the signal with the estimated sampling clock error. 
                            // The filter used as interpolator has non-causal output (why is beyond my understading). This -3
                            // solves this issue. TODO why does this happen? better solution?
                            required_for_interpolation = interpolate_input(&in[d_consumed+low-3], &d_interpolated[0]);
                            // I derotate the signal with the estimated frequency error. 
                            
                            derotate(d_interpolated, d_fft_calculator.get_inbuf());
                        }
                        else 
                        {
                            derotate(&in[d_consumed+low], d_fft_calculator.get_inbuf());
                        }

                        // I (naturally) calculate the FFT. 
                        calculate_fft(d_postfft);

                        // Calculate and correct the integer frequency error.  
                        d_freq_offset = estimate_integer_freq_offset(d_postfft);

                        d_integer_freq_derotated = &d_postfft[0] + d_freq_offset + d_zeros_on_left; 

                        //Estimate the current symbol index. Should be performed at every iteration since the USRP 
                        //may drop samples, in which case we signal it downstream. 
                        d_previous_symbol = d_current_symbol; 
                        d_current_symbol = estimate_symbol_index(d_integer_freq_derotated);
                        send_symbol_index_and_resync(i); 

                        // I keep the last symbol for fine-synchro. 
                        gr_complex * aux = d_previous_channel_gain; 
                        d_previous_channel_gain = d_channel_gain; 
                        d_channel_gain = aux; 

                        //I calculate the channel taps at the SPs...
                        calculate_channel_taps_sp(d_integer_freq_derotated, d_current_symbol, d_channel_gain);
                        
                        // and interpolate in the rest of the carriers. 
                        linearly_estimate_channel_taps(d_current_symbol, d_channel_gain);

                        // Equalization is applied. 
    
                        //I now perform a rather indirect complex division with VOLK (plus, I tried to minimize
                        // the usage of auxilary variables) 
                        // TODO a new VOLK kernel that makes complex division?
                        volk_32fc_x2_multiply_conjugate_32fc(&out[i*d_active_carriers], d_integer_freq_derotated, d_channel_gain, d_active_carriers);
                        volk_32fc_magnitude_squared_32f(d_channel_gain_mag_sq, d_channel_gain, d_active_carriers);
                        volk_32f_x2_divide_32f(d_channel_gain_mag_sq, d_ones, d_channel_gain_mag_sq, d_active_carriers);
                        volk_32fc_32f_multiply_32fc(&out[i*d_active_carriers], &out[i*d_active_carriers], d_channel_gain_mag_sq, d_active_carriers);
                        // Using what follows instead, is actually slower (seems that taking powers is worse than simply dividing).
                        //volk_32fc_s32f_power_32fc(d_channel_gain_inv, d_channel_gain, -1.0, d_active_carriers);
                        //volk_32fc_x2_multiply_32fc(&out[i*d_active_carriers], d_integer_freq_derotated, d_channel_gain_inv, d_active_carriers);

                        if (ch_output_connected){
                            // the channel taps output is connected
                            memcpy(&out_channel_gain[i*d_active_carriers], d_channel_gain, d_active_carriers*sizeof(gr_complex));
                        }

                        if(freq_error_output_connected)
                        {
                            out_freq_error[i] = d_peak_epsilon/(2*3.14159) + d_freq_offset; 
                        }
                        if(samp_error_output_connected)
                        {
                            out_samp_error[i] = d_est_delta; 
                        }

                        // If an integer frequency error was detected, I add it to the estimation, which considers 
                        // fractional errors only. 
                        d_est_freq = d_est_freq + 3.14159*2*d_freq_offset; 

                        // I update the fine timing and frequency estimations. 
                        estimate_fine_synchro(d_channel_gain, d_previous_channel_gain); 

                        d_out += 1; 

                    }
                    else
                    {
                        // Send sync_start downstream
                        send_sync_start();
                        d_initial_acquired = false;

                        // Restart with a half number so that we'll not endup with the same situation
                        // This will prevent peak_detect to not detect anything
                        d_consumed += (d_cp_length+d_fft_length)/2;
                        consume_each(d_consumed);
                        // Tell runtime system how many output items we produced.
                        // bye!
                        return (d_out);
                    }
                    d_consumed += required_for_interpolation;

                    int delta_pos = required_for_interpolation - (d_fft_length+d_cp_length);
                    d_moved_cp = (delta_pos!=0);
               }

                // Tell runtime system how many input items we consumed on
                // each input stream.
                consume_each(d_consumed);

               // Tell runtime system how many output items we produced.
                return (d_out);

            }

    } /* namespace isdbt */
} /* namespace gr */

