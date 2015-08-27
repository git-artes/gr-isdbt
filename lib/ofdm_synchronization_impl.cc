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
            ofdm_synchronization::make(int mode, float cp_length)
            {
                return gnuradio::get_initial_sptr
                    (new ofdm_synchronization_impl(mode, cp_length));
            }

        /*
         * The private constructor
         */
        ofdm_synchronization_impl::ofdm_synchronization_impl(int mode, float cp_length)
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

            d_prefft_synched = new gr_complex[d_fft_length]; 
            d_postfft = new gr_complex[d_fft_length];
            //d_prefft_synched = (gr_complex *)fftwf_malloc(sizeof(gr_complex)*d_fft_length); 
            //d_postfft_synched = (gr_complex *)fftwf_malloc(sizeof(gr_complex)*d_fft_length); 
            
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

            d_channel_gain = new gr_complex[d_active_carriers]; 

            d_previous_channel_gain = new gr_complex[d_active_carriers]; 
            d_delta_channel_gains = new gr_complex[d_active_carriers]; 
            d_timing_offset = 0; 
            d_samp_inc = 1; 
            d_samp_phase = 0; 
            d_last_samp_phase = 0; 
            //d_interpolated = new gr_complex[2*d_fft_length+d_cp_length]; 
            d_interpolated = new gr_complex[d_fft_length+d_cp_length]; 
            d_cp_start_offset = 0; 
            d_fine_freq = 0; 

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

            delete [] d_prefft_synched; 
            delete [] d_postfft; 

            delete [] d_pilot_values; 
            delete [] d_known_phase_diff; 

            delete [] d_channel_gain; 

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
            //int required_for_interpolating_cp_and_fft = 0; 
            //d_samp_phase = d_last_samp_phase; 
            float expected_phase = d_samp_phase + ((float)d_cp_length+d_fft_length)*(d_samp_inc-1.0); 
            while(oo < d_cp_length+d_fft_length) {
                out[oo++] = d_inter.interpolate(&in[ii], d_samp_phase);
                //printf("int[%i]=%f+j%f; out[%i]=%f+j%f, ntaps=%i, s=%f, f=%f, incr=%i, d_samp_phase=%f, ii=%i\n",ii, in[ii].real(), in[ii].imag(), oo-1, out[oo-1].real(), out[oo-1].imag(), d_inter.ntaps(), s, f, incr, d_samp_phase, ii); 

                s = d_samp_phase + d_samp_inc;
                f = floor(s);
                incr = (int)f;
                d_samp_phase = s - f;
                ii += incr;
            }
           
            // return how many inputs we required to generate d_cp_length+d_fft_length outputs 
            //return required_for_interpolating_cp_and_fft;
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
            ofdm_synchronization_impl::estimate_fine_synchro()
            {
                gr_complex delta_curr_channel;  
                volk_32fc_x2_conjugate_dot_prod_32fc(&delta_curr_channel, &d_channel_gain[1], &d_channel_gain[0], d_active_carriers-1);
                d_timing_offset = d_fft_length/(2*3.14159)*std::arg(delta_curr_channel); 
                double fractional_offset, cp_start_offset;  
                fractional_offset = modf(-d_timing_offset, &cp_start_offset); 
                d_cp_start_offset += cp_start_offset; 
               
                gr_complex result_1st;  
                volk_32fc_x2_conjugate_dot_prod_32fc(&result_1st, &d_channel_gain[0], &d_previous_channel_gain[0], floor(d_active_carriers/2.0));
                gr_complex result_2nd;  
                int low = (int)ceil(d_active_carriers/2.0); 
                volk_32fc_x2_conjugate_dot_prod_32fc(&result_2nd, &d_channel_gain[low], &d_previous_channel_gain[low], d_active_carriers-low+1);
                float delta_est_error = 1.0/(1.0+((float)d_cp_length)/d_fft_length)/(2.0*3.14159*d_active_carriers/2.0)*std::arg(result_2nd*std::conj(result_1st)) ; 
                float freq_est_error  = (std::arg(result_1st)+std::arg(result_2nd))/2.0/(1.0+(float)d_cp_length/d_fft_length); 
               
                if((int)cp_start_offset==0 && !d_moved_cp)
                {
                    advance_delta_loop(delta_est_error);
                    advance_freq_loop(freq_est_error);
                }
                else
                {
                   // delta_est_error = 0; 
                }
                    //printf("freq_est_error: %e, d_cp_start_offset:%i, d_est_freq:%f, d_freq_offset:%i, d_peak_epsilon:%e\n", freq_est_error, d_cp_start_offset, d_est_freq, d_freq_offset, d_peak_epsilon); 

                d_moved_cp = ((int)cp_start_offset!=0); 

                d_samp_inc = 1.0-d_est_delta; 
                d_peak_epsilon = d_est_freq;  
               
                
            }

        void
            ofdm_synchronization_impl::estimate_fine_freq()
            {
                
                gr_complex result;
                volk_32fc_x2_conjugate_dot_prod_32fc(&result, &d_channel_gain[0], &d_previous_channel_gain[0], d_active_carriers);
                //d_fine_freq  = 1.0/(1+(float)d_cp_length/d_fft_length)*std::arg(result); 
                d_fine_freq  = std::arg(result)/(1.0+(float)d_cp_length/d_fft_length); 

                if(!d_moved_cp)
                {
                    printf("d_fine_freq: %e, d_cp_start_offset:%i, d_est_freq:%f\n", d_fine_freq, d_cp_start_offset, d_est_freq); 
                    advance_freq_loop(d_fine_freq);
                }

                //advance_freq_loop(d_fine_freq+d_freq_offset*2*3.14159);
                //advance_freq_loop(d_fine_freq);
                d_peak_epsilon = d_est_freq;  
               
                //printf("d_cp_start_offset: %i, peak_epsilon: %f, fine_freq: %f, arg: %f, fast_atan=%f, integer_freq=%i, total_freq_correction: %f, d_est_freq: %f\n", d_cp_start_offset, d_peak_epsilon, d_fine_freq, std::arg(result), fast_atan2f(result), d_freq_offset, -d_peak_epsilon/(2*3.14159)-d_freq_offset, d_est_freq/(2*3.14159));
                //printf("total_freq_correction: %f, d_freq_offset: %i, d_est_freq: %f, error: %f\n", -d_peak_epsilon/(2*3.14159)-d_freq_offset, d_freq_offset, d_est_freq/(2*3.14159), d_fine_freq/(2*3.14159));
 
            }

        void
            ofdm_synchronization_impl::estimate_fine_timing()
            {
                gr_complex delta_curr_channel;  
                volk_32fc_x2_conjugate_dot_prod_32fc(&delta_curr_channel, &d_channel_gain[1], &d_channel_gain[0], d_active_carriers-1);
                d_timing_offset = d_fft_length/(2*3.14159)*std::arg(delta_curr_channel); 
                double fractional_offset, cp_start_offset;  
                fractional_offset = modf(-d_timing_offset, &cp_start_offset); 
                d_cp_start_offset += cp_start_offset; 
               
                gr_complex result_1st;  
                volk_32fc_x2_conjugate_dot_prod_32fc(&result_1st, &d_channel_gain[0], &d_previous_channel_gain[0], floor(d_active_carriers/2.0));
                gr_complex result_2nd;  
                int low = (int)ceil(d_active_carriers/2.0); 
                volk_32fc_x2_conjugate_dot_prod_32fc(&result_2nd, &d_channel_gain[low], &d_previous_channel_gain[low], d_active_carriers-low+1);
                float delta_est_error = 1.0/(1.0+((float)d_cp_length)/d_fft_length)/(2.0*3.14159*d_active_carriers/2.0)*std::arg(result_2nd*std::conj(result_1st)) ; 
               
                //if (!d_moved_cp)
                if((int)cp_start_offset==0 && !d_moved_cp)
                {
                    advance_delta_loop(delta_est_error);
                }
                else
                {
                   // delta_est_error = 0; 
                }

                //advance_delta_loop(delta_est_error);
                d_moved_cp = ((int)cp_start_offset!=0); 
                //advance_delta_loop(delta_phase/(d_fft_length+d_cp_length));
                
                d_samp_inc = 1.0-d_est_delta; 
                //d_samp_inc = 1-3.67e-6; 
               
                //printf("cp_start_est: %i, corrective_timing_offset: %f, frac_offset=%f, cp_start_offset=%i, d_samp_phase=%f, delta_phase: %f, d_samp_inc=%f, d_avg_samp_inc=%e\n", d_cp_start, -d_timing_offset, fractional_offset, d_cp_start_offset, d_samp_phase, delta_phase, d_samp_inc, d_avg_samp_inc);
               //printf("d_samp_phase=%f, delta_est_error = %e, d_est_delta=%e, d_cp_start_offset=%i, cp_start_offset: %i\n", d_samp_phase, delta_est_error, d_est_delta, d_cp_start_offset, (int)cp_start_offset);
                
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
            ofdm_synchronization_impl::linearly_estimate_channel_taps()
            {
                // This method interpolates scattered measurements across one OFDM symbol
                // It does not use measurements from the previous OFDM symnbols (does not use history)
                // as it may have encountered a phase change for the current phase only
                // TODO interpolation is too simple, a new method(s) should be implemented
                int current_sp_carrier = 0; 
                int next_sp_carrier = 0; 
                for (int i = 0; i < d_sp_carriers_size-1; i++)
                {
                    // Current sp carrier
                    current_sp_carrier = 12*i+3*d_current_symbol;
                    // Next sp carrier
                    next_sp_carrier = 12*(i+1)+3*d_current_symbol;

                    // Calculate interpolation for all intermediate values
                    for (int j = 1; j < next_sp_carrier-current_sp_carrier; j++)
                    {
                        d_channel_gain[current_sp_carrier+j] = d_channel_gain[current_sp_carrier]*gr_complex(1.0-j/12.0,0.0) +  d_channel_gain[next_sp_carrier]*gr_complex(j/12.0,0.0) ;
                    }

                }
                /////////////////////////////////////////////////////////////
                //take care of extreme cases: first carriers and last carriers
                /////////////////////////////////////////////////////////////
                gr_complex tg_alpha; 
                if (d_current_symbol>0){
                    //we have not updated the gain on the first carriers
                    //we now do this with a very simple linear interpolator 
                    //TODO is this a good estimation??
                    current_sp_carrier = 3*d_current_symbol;
                    next_sp_carrier = 12+3*d_current_symbol;
                    tg_alpha = (d_channel_gain[next_sp_carrier] - d_channel_gain[current_sp_carrier]) / gr_complex(next_sp_carrier-current_sp_carrier, 0.0);
                    // Calculate interpolation for all intermediate values
                    for (int j = -current_sp_carrier; j < 0; j++)
                    {
                        d_channel_gain[current_sp_carrier+j] = d_channel_gain[current_sp_carrier] + tg_alpha * gr_complex(j, 0.0);
                    }
                }

                // now the other extreme case: the last carriers. 
                // we will use the last SP and the CP
                current_sp_carrier = 12*(d_sp_carriers_size-1)+3*d_current_symbol;
                next_sp_carrier = d_active_carriers-1;
                float delta_carriers =(float) next_sp_carrier-current_sp_carrier;
                for (int j = 1; j < delta_carriers; j++)
                {
                    d_channel_gain[current_sp_carrier+j] = d_channel_gain[current_sp_carrier]*gr_complex(1.0-j/delta_carriers,0.0) +  d_channel_gain[next_sp_carrier]*gr_complex(j/delta_carriers,0.0) ;
                }
            }

        void ofdm_synchronization_impl::calculate_channel_taps_sp(const gr_complex * in, int current_symbol)
        {
            int current_sp_carrier = 0; 
            // We first calculate the channel gain on the SP carriers.
            for (int i = 0; i < d_sp_carriers_size; i++)
            {
                // We get each sp carrier position. We now know which is the current symbol (0, 1, 2 or 3)
                current_sp_carrier = 12*i+3*current_symbol;
                // channel gain = (sp carrier actual value)/(sp carrier expected value)
                d_channel_gain[current_sp_carrier] = in[current_sp_carrier+d_zeros_on_left]/d_pilot_values[current_sp_carrier];
            }
            //we then calculate the gain on the CP
            d_channel_gain[d_active_carriers-1] = in[d_active_carriers-1+d_zeros_on_left]/d_pilot_values[d_active_carriers-1];
            /*printf("CP chann_gain = %f+j%f; arg|CP chann_gain|= %f, CP in=%f+j%f\n",d_channel_gain[d_active_carriers-1].real(), d_channel_gain[d_active_carriers-1].imag(), \
                    std::arg(d_channel_gain[d_active_carriers-1]), \
                    in[d_active_carriers-1+d_zeros_on_left].real(),\
                    in[d_active_carriers-1+d_zeros_on_left].imag()\
                    );*/
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
                gr_complex sum;
                int current_symbol = 0;

                int next_sp_carrier; // The next sp carrier
                int current_sp_carrier; // The current sp carrier
                gr_complex phase;
                // sym_count (Symbol count) can take values 0 to 3, according to the positions of the scattered pilots
                for (int sym_count = 0; sym_count < 4; sym_count++)
                {
                    sum = 0;
                    // For every scattered pilot but the last one...
                    for (int i=0; i < d_sp_carriers_size-1; i++)
                    {
                        // Get the position of the next and current sp carrier based on the value of sym_count
                        next_sp_carrier = 12*(i+1)+3*sym_count; 
                        current_sp_carrier = 12*i+3*sym_count; 

                        if (d_pilot_values[next_sp_carrier]==d_pilot_values[current_sp_carrier])
                            // If the phase difference between in[next_sp_carrier+d_zeros_on_left] and in[current_sp_carrier+d_zeros_on_left] is zero,
                            // is because we expect both to be always in phase
                            phase = in[next_sp_carrier+d_zeros_on_left]*conj(in[current_sp_carrier+d_zeros_on_left]);
                        else
                            // If the phase difference between in[next_sp_carrier+d_zeros_on_left] and in[current_sp_carrier+d_zeros_on_left] is not zero,
                            // is because we expect both to be always out of phase (180 degrees)
                            phase = -in[next_sp_carrier+d_zeros_on_left]*conj(in[current_sp_carrier+d_zeros_on_left]);
                        sum += phase;
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
                gr_complex phase;

                // for d_zeros_on_left +/- d_freq_offset_max...
                for (int i = d_zeros_on_left - d_freq_offset_max; i < d_zeros_on_left + d_freq_offset_max; i++)
                {
                    sum = 0;
                    for (int j = 0; j < (d_tmcc_carriers_size - 1); j++)
                    {

                        if (d_known_phase_diff[j] == 0)
                            // If the phase difference between tmcc_carriers[j+1] and tmcc_carriers[j] is zero, is because we expect both to be always in phase
                            phase = in[i + d_tmcc_carriers[j + 1]]*conj(in[i + d_tmcc_carriers[j]]);
                        else
                            // If the phase difference between tmcc_carriers[j+1] and tmcc_carriers[j] is not zero, is because we expect both to be always out of phase (180 degrees)
                            phase = -in[i + d_tmcc_carriers[j + 1]]*conj(in[i + d_tmcc_carriers[j]]);
                        sum +=phase;
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
            ofdm_synchronization_impl::calculate_fft(const gr_complex * in, gr_complex * out)
            {
                // copy the input into the calculator's buffer
                memcpy(d_fft_calculator.get_inbuf(), in, sizeof(gr_complex)*d_fft_length); 
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
                    printf("OFDM_SYNCHRO: peak under/over average! peak %f, avg_max %f, avg_min %f\n", datain[ peak_index ], d_avg_max, d_avg_min); }

                //We now check whether the peak is in the border of the search interval. This would mean that 
                //the search interval is not correct, and it should be re-set. This happens for instance when the 
                //hardware dropped some samples. 
                //Our definition of "border of the search interval" depends if the search interval is "big" or not. 
                if ( datain_length < d_fft_length )
                {
                    if ( ( peak_index == 0 ) || ( peak_index == datain_length-1 ) )
                    {
                        success = false; 
                    }
                }
                else
                {
                    if ( ( peak_index < 5 ) || ( peak_index > datain_length-5 ) )
                    {
                        success = false; 
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
                size = lookup_start - (lookup_stop - (d_cp_length + d_fft_length - 1)) + 1;
                volk_32fc_magnitude_squared_32f(&d_norm[low], &in[low], size);

                // Calculate gamma on each point
                //TODO check these boundaries!!!!!!!
                low = lookup_stop - (d_cp_length - 1);
                //low = lookup_stop - d_cp_length - 1;
                //size = lookup_start - (lookup_stop - d_cp_length - 1) + 1;
                size = lookup_start - low + 1;
                volk_32fc_x2_multiply_conjugate_32fc(&d_corr[low - d_fft_length], &in[low], &in[low - d_fft_length], size);

                // Calculate time delay and frequency correction
                // This looks like spaghetti code but it is fast
                for (int i = lookup_start - 1; i >= lookup_stop; i--)
                {
                    int k = i - lookup_stop;

                    d_phi[k] = 0.0;
                    d_gamma[k] = 0.0;

                    // Moving sum for calculating gamma and phi
                    for (int j = 0; j < d_cp_length; j++)
                    {
                        // Calculate gamma and store it
                        d_gamma[k] += d_corr[i - j - d_fft_length];
                        // Calculate phi and store it
                        d_phi[k] += d_norm[i - j] + d_norm[i - j - d_fft_length];
                    }
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

                int peak;
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
            /*            for (int i = 0; i < d_fft_length; i++)
                          {
            // if (i == d_nextpos)
            //     d_phaseinc = d_nextphaseinc;
            // We are interested only in fft_length
            d_phase += d_phaseinc;

            while (d_phase > (float)M_PI)
            d_phase -= (float)(2.0 * M_PI);
            while (d_phase < (float)(-M_PI))
            d_phase += (float)(2.0 * M_PI);

            d_derot[i] = gr_expj(d_phase);
            }

            //d_nextphaseinc = sensitivity * (d_peak_epsilon);
            //d_nextpos = d_cp_start - (d_cp_length+d_fft_length); 

            volk_32fc_x2_multiply_32fc(&out[0], &d_derot[0], &in[0], d_fft_length);
            for (int i = 0; i < d_cp_length; i++)
            {
            d_phase += d_phaseinc;
            while (d_phase > (float)M_PI)
            d_phase -= (float)(2.0 * M_PI);
            while (d_phase < (float)(-M_PI))
            d_phase += (float)(2.0 * M_PI);
            }
            */
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
                //int required_for_interpolation = interpolate_input(&in[d_consumed], &d_interpolated[0]); 
                int required_for_interpolation = d_cp_length + d_fft_length; 
                d_avg_samp_inc = 0; 
                //memcpy(&d_interpolated[0], &in[d_consumed], sizeof(gr_complex)*(2*d_fft_length+d_cp_length)); 
                //int required_for_interpolation = d_cp_length + d_fft_length; 
                
                // This is initial aquisition of symbol start
                // It is also calle coarse frequency correction
                // TODO - make a FSM
                if (!d_initial_acquired)
                {
                    // If we are here it means that we have no idea where the CP may be. We thus 
                    // search it thoroughly
                    d_initial_acquired = ml_sync(&in[d_consumed], 2 * d_fft_length + d_cp_length - 1, d_fft_length + d_cp_length - 1, &d_cp_start, &d_peak_epsilon);
                    //d_initial_acquired = ml_sync(&d_interpolated[0], 2 * d_fft_length + d_cp_length - 1, d_fft_length + d_cp_length - 1, &d_cp_start, &d_peak_epsilon);
                    required_for_interpolation = d_cp_length + d_fft_length; 
                    d_cp_found = d_initial_acquired; 
                    d_cp_start_offset = 0; 
                    d_samp_phase = 0; 
                    //d_samp_inc = 1.0; 
                    d_est_freq = d_peak_epsilon; 
                    //d_est_delta = 0; 

                }
                else
                {
                    //If we are here it means that in the previous iteration we found the CP. We
                    //now thus only search near it. 
                    //d_cp_found = ml_sync(&in[d_consumed], d_cp_start + 8, std::max(d_cp_start - 8,d_cp_length+d_fft_length-1), \
                    //        &d_cp_start, &d_peak_epsilon);
                    int cp_start_aux; 
                    float peak_epsilon_aux; 
                    //d_cp_found = ml_sync(&d_interpolated[0], d_cp_start+d_cp_start_offset + 8, std::max(d_cp_start+d_cp_start_offset - 8,d_cp_length+d_fft_length-1), &cp_start_aux, &peak_epsilon_aux);
                    d_cp_found = ml_sync(&in[d_consumed], d_cp_start+d_cp_start_offset + 8, std::max(d_cp_start+d_cp_start_offset - 8,d_cp_length+d_fft_length-1), &cp_start_aux, &peak_epsilon_aux);
                    printf("peak_epsilon_aux: %f, peak_epsilon: %f\n",peak_epsilon_aux/(2*3.14159), d_peak_epsilon/(2*3.14159)); 
                  
                    if ( !d_cp_found )
                    {
                        // We may have not found the CP because the smaller search range was too small (rare, but possible). 
                        // We re-try with the whole search range. 
                        d_cp_found = ml_sync(&in[d_consumed], 2 * d_fft_length + d_cp_length - 1, d_fft_length + d_cp_length - 1, \
                                &d_cp_start, &d_peak_epsilon);
                        d_cp_start_offset = 0; 
                        d_samp_phase = 0; 
                        d_est_freq = d_peak_epsilon; 
                    }

                }

                if ( d_cp_found )
                {
                    //int low = d_consumed + d_cp_start - d_fft_length + 1 ;
                    //derotate(&in[low], &d_prefft_synched[0]);
                    
                    //int low = d_cp_start - d_fft_length + 1 ;
                    int low = d_cp_start + d_cp_start_offset - d_fft_length + 1 ;

                    //required_for_interpolation = interpolate_input(&in[d_consumed], &d_interpolated[0]); 
                    //derotate(&d_interpolated[low], &d_prefft_synched[0]);
                    required_for_interpolation = interpolate_input(&in[d_consumed+low], &d_interpolated[0]); 
                    //printf("low: %i, fin: %i\n", d_consumed+low, d_consumed+low+required_for_interpolation); 
                    derotate(&d_interpolated[0], &d_prefft_synched[0]);
                        //for (int ii=0; ii<10; ii++)
                        //    printf("pre_fft[%i]=%f+j%f\n",ii+i*d_fft_length, d_prefft_synched[ii].real(), d_prefft_synched[ii].imag()); 
                    calculate_fft(&d_prefft_synched[0], &d_postfft[0]);
                    //to test only the symbol timing, uncomment the next line
                    //memcpy(&out[i*d_fft_length], &d_prefft_synched[0], sizeof(gr_complex)*d_fft_length); 
                    
                    d_freq_offset = estimate_integer_freq_offset(&d_postfft[0]);
                    d_integer_freq_derotated = &d_postfft[0] + d_freq_offset; 
                    d_previous_symbol = d_current_symbol; 
                    d_current_symbol = estimate_symbol_index(d_integer_freq_derotated);
                    send_symbol_index_and_resync(i); 
 
                    gr_complex * aux = d_previous_channel_gain; 
                    d_previous_channel_gain = d_channel_gain; 
                    d_channel_gain = aux; 

                    calculate_channel_taps_sp(d_integer_freq_derotated, d_current_symbol);
                    linearly_estimate_channel_taps();


                    for (int carrier = 0; carrier < d_active_carriers; carrier++)
                    {
                        out[i*d_active_carriers +carrier] = d_integer_freq_derotated[carrier+d_zeros_on_left]/d_channel_gain[carrier];
                        if (ch_output_connected){
                            // the channel taps output is connected
                            out_channel_gain[i*d_active_carriers + carrier] = d_channel_gain[carrier]; 
                        }
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
                    //estimate_fine_freq(); 
                    //estimate_fine_timing(); 

                    estimate_fine_synchro(); 

                    //memcpy(&out[i*d_fft_length], &d_postfft[0], sizeof(gr_complex)*d_fft_length); 
                }
                else
                {
                    // Send sync_start downstream
                    send_sync_start();
                    d_initial_acquired = false;

                    // Restart wit a half number so that we'll not endup with the same situation
                    // This will prevent peak_detect to not detect anything
                    d_consumed += (d_cp_length+d_fft_length)/2;
                    consume_each(d_consumed);
                    // Tell runtime system how many output items we produced.
                    // bye!
                    return (d_out);
                }
                //d_consumed += d_cp_length+d_fft_length;
                d_consumed += required_for_interpolation;
                //printf("required_for_interpolation: %i, cp+fft:%i, cp_offset:%i\n",required_for_interpolation, d_fft_length+d_cp_length, d_consumed - (d_fft_length+d_cp_length)*noutput_items);
                d_out += 1; 
            }

            // Tell runtime system how many input items we consumed on
            // each input stream.
            consume_each(d_consumed);

            d_cp_start_offset += d_consumed - (d_fft_length+d_cp_length)*noutput_items;
            if (d_cp_start_offset!=0)
            {
                d_moved_cp = true; 
            }

            // Tell runtime system how many output items we produced.
            return (d_out);

        }

} /* namespace isdbt */
} /* namespace gr */

