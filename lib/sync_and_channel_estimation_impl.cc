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
 
 
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "sync_and_channel_estimation_impl.h"
#include <complex>
#include <stdio.h>
#include <gnuradio/expj.h>
#include <gnuradio/math.h>
#include <volk/volk.h>

namespace gr {
    namespace isdbt {

        int previous_symbol;

        // TMCC carriers positions for each transmission mode

        // Mode 1 (2K)
        const int sync_and_channel_estimation_impl::tmcc_carriers_size_2k = 13;
        const int sync_and_channel_estimation_impl::tmcc_carriers_2k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319
        };

        // Mode 2 (4K)
        const int sync_and_channel_estimation_impl::tmcc_carriers_size_4k = 26;
        const int sync_and_channel_estimation_impl::tmcc_carriers_4k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319, 1474, 1537, 1637,\
                1814, 1880, 1991, 2101,	2191, 2351, 2437, 2569,\
                2693, 2723
        };

        // Mode 3 (8K)
        const int sync_and_channel_estimation_impl::tmcc_carriers_size_8k = 52;
        const int sync_and_channel_estimation_impl::tmcc_carriers_8k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319, 1474, 1537, 1637,\
                1814, 1880, 1991, 2101, 2191, 2351, 2437, 2569,\
                2693, 2723, 2878, 2941, 3041, 3218, 3284, 3395,\
                3505, 3595, 3755, 3841, 3973, 4097, 4127, 4282,\
                4345, 4445, 4622, 4688, 4799, 4909, 4999, 5159,\
                5245, 5377, 5501, 5531
        };

       void
            sync_and_channel_estimation_impl::tmcc_positions(int fft)
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

       sync_and_channel_estimation::sptr
            sync_and_channel_estimation::make(int fft_length, int payload_length, int offset_max)
            {
                return gnuradio::get_initial_sptr
                    (new sync_and_channel_estimation_impl(fft_length, payload_length, offset_max));
            }
        
       gr_complex
            sync_and_channel_estimation_impl::get_pilot_value(int index)
            {
                //TODO - can be calculated at the beginning

                // Get the value of each pilot
                return gr_complex((float)(4 * 2 * (0.5 - d_wk[index])) / 3, 0);
            }

       void
            sync_and_channel_estimation_impl::generate_prbs()
            {
                // Generate the prbs sequence for each active carrier

                // Init PRBS register with 1s (reg_prbs = 111111111111)
                unsigned int reg_prbs = (1 << 11) - 1;

                for (int k = 0; k < (d_active_carriers); k++)
                {
                    d_wk[k] = (char)(reg_prbs & 0x01); // Get the LSB of the register as a character
                    int new_bit = ((reg_prbs >> 2) ^ (reg_prbs >> 0)) & 0x01; // This is the bit we will add to the register as MSB
                    reg_prbs = (reg_prbs >> 1) | (new_bit << 10); // We movo all the register to the right and add the new_bit as MSB
                    d_pilot_values[k] = gr_complex((float)(4 * 2 * (0.5 - d_wk[k])) / 3, 0);
                }
            }

        //TODO the parameters in this constructor are counter-intuitive and may lead to segmentation faults if not carefully 
        //assigned. Correct to a more intuitive one where we only indicate the mode. 
        sync_and_channel_estimation_impl::sync_and_channel_estimation_impl(int fft_length,int payload_length, int offset_max)
            : gr::sync_block("sync_and_channel_estimation",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*fft_length),
                    gr::io_signature::make(1, 2, sizeof(gr_complex)*payload_length)),
            d_fft_length(fft_length), d_active_carriers(payload_length), d_freq_offset_max(offset_max)

        {

            // a test to check if we can use time-interpolation for channel estimation
            set_history(5); 
            
            //VOLK alignment as recommended by GNU Radio's Manual. It has a similar effect 
            //than set_output_multiple(), thus we will generally get multiples of this value
            //as noutput_items. 
            const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
            set_alignment(std::max(1, alignment_multiple));


            d_ninput = d_fft_length;
            d_noutput = d_active_carriers;
            d_freq_offset = 0; 
            d_freq_offset1 = 0; 
            d_freq_offset2 = 0; 
            d_freq_offset3 = 0; 
            d_freq_offset4 = 0; 

            // Number of zeros on the left of the IFFT
            d_zeros_on_left = int(ceil((d_fft_length - (d_active_carriers)) / 2.0));

            // Number of sp pilots
            d_sp_carriers_size = (d_active_carriers - 1)/12;

            // Set TMCC parameters in mode 1, 2 or 3
            tmcc_positions(d_fft_length);

            // Get some memory
            d_wk = new char[d_active_carriers];
            d_pilot_values = new gr_complex[d_active_carriers];
            d_known_phase_diff = new float[d_tmcc_carriers_size];
            d_corr_sp = new float[4];
            d_channel_gain = new gr_complex[d_active_carriers];

            // Generate PRBS
            generate_prbs();

            // Obtain phase diff for all tmcc pilots
            for (int i = 0; i < (d_tmcc_carriers_size - 1); i++)
            {
                //d_known_phase_diff[i] = std::norm(get_pilot_value(tmcc_carriers[i + 1]) - get_pilot_value(tmcc_carriers[i]));
                d_known_phase_diff[i] = std::norm(d_pilot_values[d_tmcc_carriers[i + 1]] - d_pilot_values[d_tmcc_carriers[i]]);
            }

            d_channel_gain1 = new gr_complex[d_active_carriers]; 
            d_channel_gain2 = new gr_complex[d_active_carriers]; 
            d_channel_gain3 = new gr_complex[d_active_carriers]; 
            d_channel_gain4 = new gr_complex[d_active_carriers]; 

            d_coeffs_linear_estimate_first = new gr_complex[11]; 
            d_aux_linear_estimate_first = new gr_complex[11]; 
            d_coeffs_linear_estimate_last = new gr_complex[11]; 
            d_aux_linear_estimate_last = new gr_complex[11]; 
            for (int i=1; i<12; i++){
                d_coeffs_linear_estimate_first[i-1] = gr_complex(1.0-i/12.0,0.0);
                d_coeffs_linear_estimate_last[i-1] = gr_complex(i/12.0,0.0);
            }

            d_ones = new float[d_active_carriers];
            for(int i=0; i<d_active_carriers; i++)
            {
                d_ones[i] = 1.0;
            }
            
            d_channel_gain_mag_sq = new float[d_active_carriers];

            // Allocate buffer for deroated input symbol
            d_derotated_in = new gr_complex[d_fft_length];
            if (d_derotated_in == NULL)
            {
                std::cout << "error allocating d_derotated_in" << std::endl;
                return;
            }
        }

        sync_and_channel_estimation_impl::~sync_and_channel_estimation_impl()
        {
        }

        /*
         * post-fft frequency offset estimation
         * -----------------------------------------------------------------------------------------
         */
        int
            sync_and_channel_estimation_impl::estimate_integer_freq_offset(const gr_complex * in)
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
                //printf("freq offset: %d\n",d_freq_o
            
            }


        /*
         * process_sp_data function
         * post-fft frequency offset estimation
         * -----------------------------------------------------------------------------------------
         */

        int 
            sync_and_channel_estimation_impl::estimate_symbol_index(const gr_complex * in)
            {

                /*************************************************************/
                // Find out the OFDM symbol index (value 0 to 3) sent
                // in current block by correlating scattered symbols with
                // current block - result is (symbol index % 4)
                /*************************************************************/
                float max = 0;
                gr_complex sum;
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

        void sync_and_channel_estimation_impl::calculate_channel_taps_sp(const gr_complex * in, int current_symbol){
            // We first calculate the channel gain on the SP carriers.
            // We get each sp carrier position. We now know which is the current symbol (0, 1, 2 or 3)
            for (int current_sp_carrier = 3*current_symbol; current_sp_carrier<d_active_carriers-1; current_sp_carrier+=12)
            {
                d_channel_gain[current_sp_carrier] = in[current_sp_carrier]/d_pilot_values[current_sp_carrier];
            }
            //we then calculate the gain on the CP
            d_channel_gain[d_active_carriers-1] = in[d_active_carriers-1]/d_pilot_values[d_active_carriers-1];
 
        }

        void 
            sync_and_channel_estimation_impl::linearly_estimate_channel_taps()
            {
                // This method interpolates scattered measurements across one OFDM symbol
                // It does not use measurements from the previous OFDM symbols (does not use history)
                // as it may have encountered a phase change for the current phase only
                // TODO interpolation is too simple, a new method(s) should be implemented
                    
                // Current sp carrier
                int current_sp_carrier = 3*d_current_symbol; 
                // Next sp carrier
                int next_sp_carrier = 12 + 3*d_current_symbol; 
                for (int i = 0; i < d_sp_carriers_size-1; i++)
                {

                    volk_32fc_s32fc_multiply_32fc(d_aux_linear_estimate_first, d_coeffs_linear_estimate_first, d_channel_gain[current_sp_carrier], 11); 
                    volk_32fc_s32fc_multiply_32fc(d_aux_linear_estimate_last, d_coeffs_linear_estimate_last, d_channel_gain[next_sp_carrier], 11); 
                    for (int j = 1; j < 12; j++)
                    {
                        d_channel_gain[current_sp_carrier+j] = d_aux_linear_estimate_first[j-1] + d_aux_linear_estimate_last[j-1];
                    }
 
                    current_sp_carrier += 12;
                    next_sp_carrier += 12;

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

        void 
            sync_and_channel_estimation_impl::linear_frequency_interpolation()
            {
                // This method interpolates scattered measurements across one OFDM symbol
                int current_sp_carrier = 0; 
                int next_sp_carrier = 0; 
                for (int i = 0; i < d_sp_carriers_size*4-1; i++)
                {
                    // Current sp carrier
                    current_sp_carrier = 3*i;
                    // Next sp carrier
                    next_sp_carrier = 3*(i+1);
                    
                    float delta_phase = std::arg(d_channel_gain[next_sp_carrier]*std::conj(d_channel_gain[current_sp_carrier])); 
                    float base_phase = std::arg(d_channel_gain[current_sp_carrier]); 
                    float abs_current = std::abs(d_channel_gain[current_sp_carrier]); 
                    float abs_next = std::abs(d_channel_gain[next_sp_carrier]); 


                    // Calculate interpolation for all intermediate values
                    for (int j = 1; j < 3 ; j++)
                    {
                        d_channel_gain[current_sp_carrier+j] = d_channel_gain[current_sp_carrier]*gr_complex(1.0-j/3.0,0.0) +  d_channel_gain[next_sp_carrier]*gr_complex(j/3.0,0.0) ;
                        //std::complex<double> aux = std::polar((1.0-j/3.0)*abs_current+(j/3.0)*abs_next,base_phase+(1.0-j/3.0)*delta_phase); 
                        //d_channel_gain[current_sp_carrier+j] = gr_complex(aux.real(), aux.imag()); 
                              
                    }

                }


                // now the extreme case: the last carrier. 
                // we will use the last SP and the CP
                current_sp_carrier = 12*(d_sp_carriers_size-1)+3*3;
                next_sp_carrier = d_active_carriers-1;
                float delta_carriers =(float) next_sp_carrier-current_sp_carrier;
                for (int j = 1; j < delta_carriers+1; j++)
                {
                    d_channel_gain[current_sp_carrier+j] = d_channel_gain[current_sp_carrier]*gr_complex(1.0-j/delta_carriers,0.0) +  d_channel_gain[next_sp_carrier]*gr_complex(j/delta_carriers,0.0) ;
                }

            }

        void sync_and_channel_estimation_impl::linear_time_interpolation(const gr_complex * in, int current_symbol){
            int current_sp_carrier = 0; 
            // We first calculate the channel gain on the SP carriers.
            for (int i = 0; i < d_sp_carriers_size; i++)
            {
                // We get each sp carrier position.
                current_sp_carrier = 12*i+3*current_symbol;

                // channel gain = (sp carrier actual value)/(sp carrier expected value)
                d_channel_gain4[current_sp_carrier] = in[current_sp_carrier+d_zeros_on_left]/d_pilot_values[current_sp_carrier];

                //I now interpolate in-between
                /*d_channel_gain3[current_sp_carrier] = gr_complex(0.25,0)*d_channel_gain[current_sp_carrier] \
                                                      + gr_complex(0.75,0)*d_channel_gain4[current_sp_carrier]; 
                d_channel_gain2[current_sp_carrier] = gr_complex(0.5,0)*d_channel_gain[current_sp_carrier] \
                                                      + gr_complex(0.5,0)*d_channel_gain4[current_sp_carrier]; 
                d_channel_gain1[current_sp_carrier] = gr_complex(0.75,0)*d_channel_gain[current_sp_carrier] \
                                                      + gr_complex(0.25,0)*d_channel_gain4[current_sp_carrier]; */
                
                float delta_phase = std::arg(d_channel_gain4[current_sp_carrier]*std::conj(d_channel_gain[current_sp_carrier])); 
                float base_phase = std::arg(d_channel_gain[current_sp_carrier]); 
                float abs = std::abs(d_channel_gain[current_sp_carrier]); 
                float abs4 = std::abs(d_channel_gain4[current_sp_carrier]); 

                std::complex<double> aux = std::polar(0.25*abs+0.75*abs4,base_phase+0.25*delta_phase); 
                d_channel_gain3[current_sp_carrier] = gr_complex(aux.real(), aux.imag()); 
                aux = std::polar(0.5*abs+0.5*abs4,base_phase+0.5*delta_phase); 
                d_channel_gain2[current_sp_carrier] = aux; 
                aux = std::polar(0.75*abs+0.25*abs4,base_phase+0.75*delta_phase); 
                d_channel_gain1[current_sp_carrier] = aux; 
 


                        /*printf("chan_gain2[%i]=%f+j%f, chan_gain[%i]=%f+j%f, chan_gain4[%i]=%f+j%f\n",\
                                current_sp_carrier, d_channel_gain2[current_sp_carrier].real(),d_channel_gain2[current_sp_carrier].imag(),\
                                current_sp_carrier, d_channel_gain[current_sp_carrier].real(),d_channel_gain[current_sp_carrier].imag(),\
                                current_sp_carrier, d_channel_gain4[current_sp_carrier].real(),d_channel_gain4[current_sp_carrier].imag()); */

            }
            //we then calculate the gain on the CP
            d_channel_gain4[d_active_carriers-1] = in[d_active_carriers-1+d_zeros_on_left]/d_pilot_values[d_active_carriers-1];
/*
            memcpy(d_channel_gain, d_channel_gain1,(d_active_carriers)*sizeof(gr_complex)); 
            memcpy(d_channel_gain1, d_channel_gain2,(d_active_carriers)*sizeof(gr_complex)); 
            memcpy(d_channel_gain2, d_channel_gain3,(d_active_carriers)*sizeof(gr_complex)); 
            memcpy(d_channel_gain3, d_channel_gain4,(d_active_carriers)*sizeof(gr_complex)); 
            */
            // an auxiliary variable to perform the swap between gains. 
            gr_complex * aux = d_channel_gain; 
            d_channel_gain = d_channel_gain1; 
            d_channel_gain1 = d_channel_gain2; 
            d_channel_gain2 = d_channel_gain3; 
            d_channel_gain3 = d_channel_gain4; 
            d_channel_gain4 = aux; 
        }

        void 
            sync_and_channel_estimation_impl::quadratically_estimate_channel_taps()
            {
                // This method interpolates scattered measurements across one OFDM symbol
                // It does not use measurements from the previous OFDM symnbols (does not use history)
                // as it may have encountered a phase change for the current phase only
                gr_complex y0 = 0; 
                gr_complex y1 = 0; 
                gr_complex y2 = 0; 
                int current_sp_carrier; 
                for (int i = 0; i < d_sp_carriers_size-2; i++)
                {
                    current_sp_carrier = 12*i+3*d_current_symbol; 
                    y0 = d_channel_gain[current_sp_carrier];
                    y1 = d_channel_gain[current_sp_carrier+12];
                    y2 = d_channel_gain[current_sp_carrier+24];

                    // Calculate interpolation for all intermediate values
                    for (int j = 1; j < 12; j++)
                    {
                        d_channel_gain[current_sp_carrier+j] = y0*gr_complex((j-12.0)*(j-24.0)/288.0,0.0) + \
                                                               -y1*gr_complex(j*(j-24.0)/144.0 , 0.0) + \
                                                               y2*gr_complex(j*(j-12.0)/288.0 , 0.0); 
                    }

                }

                /////////////////////////////////////////////////////////////
                //take care of extreme cases: first carriers and last carriers
                /////////////////////////////////////////////////////////////
                if (d_current_symbol>0){
                    //we have not updated the gain on the first carriers
                    current_sp_carrier = 3*d_current_symbol; 
                    y0 = d_channel_gain[current_sp_carrier];
                    y1 = d_channel_gain[current_sp_carrier+12];
                    y2 = d_channel_gain[current_sp_carrier+24];

                    // Calculate interpolation for all intermediate values
                    for (int j = -current_sp_carrier; j < 0; j++)
                    {
                        d_channel_gain[current_sp_carrier+j] = y0*gr_complex((j-12.0)*(j-24.0)/288.0,0.0) + \
                                                               -y1*gr_complex(j*(j-24.0)/144.0 , 0.0) + \
                                                               y2*gr_complex(j*(j-12.0)/288.0 , 0.0); 
                    }
                }

                // now the other extreme case: the last carriers. 
                // we will use the two last SP and the CP
                current_sp_carrier = 12*(d_sp_carriers_size-2)+3*d_current_symbol;
                int next_sp_carrier = d_active_carriers-1;
                y0 = d_channel_gain[current_sp_carrier];
                y1 = d_channel_gain[current_sp_carrier+12];
                y2 = d_channel_gain[d_active_carriers-1];
                float delta_carriers =(float) next_sp_carrier-current_sp_carrier-12;
                    // Calculate interpolation for all intermediate values
                    for (int j = 1; j < 12+delta_carriers; j++)
                    {
                        d_channel_gain[current_sp_carrier+j] = y0*gr_complex((j-12.0)*(j-(12.0+delta_carriers))/(12.0*(12.0+delta_carriers)),0.0) + \
                                                               -y1*gr_complex(j*(j-(12.0+delta_carriers))/(12.0*delta_carriers) , 0.0) + \
                                                               y2*gr_complex(j*(j-12.0)/((12.0+delta_carriers)*(delta_carriers)) , 0.0); 
                    }
                

                //printf("delta_carriers: %f, current_sp_carriers: %i, next_sp_carriers: %i\n",delta_carriers, current_sp_carrier, next_sp_carrier);
                //printf("ch_taps[%i]: %f+j%f, ch_taps[%i]: %f+j%f\n", current_sp_carrier, d_channel_gain[current_sp_carrier].real(),d_channel_gain[current_sp_carrier].imag(),next_sp_carrier, d_channel_gain[next_sp_carrier].real(),d_channel_gain[next_sp_carrier].imag());

            }


        /*
         *
         * general_work function
         * Here is where all the signal processing is done
         * ----------------------------------------------------------------------------------------
         */

        int
            sync_and_channel_estimation_impl::work (int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {

                const gr_complex *in = (const gr_complex *) input_items[0];
                gr_complex *out = (gr_complex *) output_items[0];
                gr_complex *out_channel_gain = (gr_complex *) output_items[1];

                bool ch_output_connected = output_items.size()>=2;

                /*
                 * Here starts the signal processing
                 */

                for (int i = 0; i < noutput_items; i++)
                {
                    
                    // Process tmcc data
                    d_freq_offset = estimate_integer_freq_offset(&in[i* d_ninput]);
                    
                    // Correct ofdm symbol integer frequency error
                    d_derotated_in = &in[i*d_ninput] + d_freq_offset + d_zeros_on_left;
                   
                    // Find out the OFDM symbol index and get the d_channel_gain vector values in order to equalize the channel
                    d_previous_symbol = d_current_symbol; 
                    d_current_symbol = estimate_symbol_index(d_derotated_in);
                    
                    // I calculate the channel taps at the SPs
                    calculate_channel_taps_sp(d_derotated_in, d_current_symbol);
                    // and interpolate in the rest of the carriers
                    linearly_estimate_channel_taps();
                    //quadratically_estimate_channel_taps();
                    
                    // Equalization is applied
                    //I now perform a rather indirect complex division with VOLK (plus, I tried to minimize
                    // the usage of auxilary variables) 
                    // TODO a new VOLK kernel that makes complex division?
                    volk_32fc_x2_multiply_conjugate_32fc(&out[i*d_active_carriers], d_derotated_in, d_channel_gain, d_active_carriers);
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
                        const uint64_t offset = this->nitems_written(0)+i; 
                        pmt::pmt_t key = pmt::string_to_symbol("resync"); 
                        pmt::pmt_t value = pmt::string_to_symbol("generated by sync and channel estimation"); 
                        this->add_item_tag(0,offset,key,value); 
                    }

                    // signal downstream the relative symbol index found here. 
                    const uint64_t offset = this->nitems_written(0)+i; 
                    pmt::pmt_t key = pmt::string_to_symbol("relative_symbol_index"); 
                    pmt::pmt_t value = pmt::from_long(d_current_symbol); 
                    this->add_item_tag(0,offset,key,value);

                }

                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

