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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "sync_and_channel_estimaton_impl.h"
#include <complex>
#include <stdio.h>
#include <gnuradio/expj.h>
#include <gnuradio/math.h>

namespace gr {
    namespace isdbt {

        int previous_symbol;

        // TMCC carriers positions for each transmission mode

        // Mode 1 (2K)
        const int sync_and_channel_estimaton_impl::tmcc_carriers_size_2k = 13;
        const int sync_and_channel_estimaton_impl::tmcc_carriers_2k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319
        };

        // Mode 2 (4K)
        const int sync_and_channel_estimaton_impl::tmcc_carriers_size_4k = 26;
        const int sync_and_channel_estimaton_impl::tmcc_carriers_4k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319, 1474, 1537, 1637,\
                1814, 1880, 1991, 2101,	2191, 2351, 2437, 2569,\
                2693, 2723
        };

        // Mode 3 (8K)
        const int sync_and_channel_estimaton_impl::tmcc_carriers_size_8k = 52;
        const int sync_and_channel_estimaton_impl::tmcc_carriers_8k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319, 1474, 1537, 1637,\
                1814, 1880, 1991, 2101, 2191, 2351, 2437, 2569,\
                2693, 2723, 2878, 2941, 3041, 3218, 3284, 3395,\
                3505, 3595, 3755, 3841, 3973, 4097, 4127, 4282,\
                4345, 4445, 4622, 4688, 4799, 4909, 4999, 5159,\
                5245, 5377, 5501, 5531
        };

        /*
         * ---------------------------------------------------------------------
         */
        void
            sync_and_channel_estimaton_impl::tmcc_positions(int fft)
            {
                /*
                 * Assing to variables tmcc_carriers and tmcc_carriers_size
                 * the corresponding values according to the transmission mode
                 */
                switch (fft)
                {
                    case 2048:
                        {
                            tmcc_carriers = tmcc_carriers_2k;
                            tmcc_carriers_size = tmcc_carriers_size_2k;
                        }
                        break;
                    case 4096:
                        {
                            tmcc_carriers = tmcc_carriers_4k;
                            tmcc_carriers_size = tmcc_carriers_size_4k;
                        }
                        break;
                    case 8192:
                        {
                            tmcc_carriers = tmcc_carriers_8k;
                            tmcc_carriers_size = tmcc_carriers_size_8k;
                        }
                        break;
                    default:
                        break;
                }
            }
        /*
         * ---------------------------------------------------------------------
         */


        /*
         * ---------------------------------------------------------------------
         */
        sync_and_channel_estimaton::sptr
            sync_and_channel_estimaton::make(int fft_length, int payload_length, int offset_max)
            {
                return gnuradio::get_initial_sptr
                    (new sync_and_channel_estimaton_impl(fft_length, payload_length, offset_max));
            }
        /*
         * ---------------------------------------------------------------------
         */


        /*
         * ---------------------------------------------------------------------
         */
        gr_complex
            sync_and_channel_estimaton_impl::get_pilot_value(int index)
            {
                //TODO - can be calculated at the beginning

                // Get the value of each pilot
                return gr_complex((float)(4 * 2 * (0.5 - d_wk[index])) / 3, 0);
            }
        /*
         * ---------------------------------------------------------------------
         */

        /*
         * ---------------------------------------------------------------------
         */
        void
            sync_and_channel_estimaton_impl::generate_prbs()
            {
                // Generate the prbs sequence for each active carrier

                // Init PRBS register with 1s (reg_prbs = 111111111111)
                unsigned int reg_prbs = (1 << 11) - 1;

                for (int k = 0; k < (active_carriers); k++)
                {
                    d_wk[k] = (char)(reg_prbs & 0x01); // Get the LSB of the register as a character
                    int new_bit = ((reg_prbs >> 2) ^ (reg_prbs >> 0)) & 0x01; // This is the bit we will add to the register as MSB
                    reg_prbs = (reg_prbs >> 1) | (new_bit << 10); // We movo all the register to the right and add the new_bit as MSB
                }
            }
        /*
         * ---------------------------------------------------------------------
         */

        /*
         * The private constructor
         */
        sync_and_channel_estimaton_impl::sync_and_channel_estimaton_impl(int fft_length,int payload_length, int offset_max)
            : gr::block("sync_and_channel_estimaton",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*fft_length),
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*payload_length)),
            d_fft_length(fft_length), active_carriers(payload_length), d_freq_offset_max(offset_max)

        {

            d_ninput = d_fft_length;
            d_noutput = active_carriers;

            // Number of zeros on the left of the IFFT
            d_zeros_on_left = int(ceil((d_fft_length - (active_carriers)) / 2.0));

            // Number of sp pilots
            sp_carriers_size = (active_carriers - 1)/12;

            // Set TMCC parameters in mode 1, 2 or 3
            tmcc_positions(d_fft_length);

            // Get some memory
            d_wk = new char[active_carriers];
            d_known_phase_diff = new float[tmcc_carriers_size];
            d_channel_gain = new gr_complex[active_carriers];

            // Generate PRBS
            generate_prbs();

            // Obtain phase diff for all tmcc pilots
            for (int i = 0; i < (tmcc_carriers_size - 1); i++)
            {
                d_known_phase_diff[i] = \
                                        std::norm(get_pilot_value(tmcc_carriers[i + 1]) - get_pilot_value(tmcc_carriers[i]));
            }

            // Allocate buffer for deroated input symbol
            derotated_in = new gr_complex[d_fft_length];
            if (derotated_in == NULL)
            {
                std::cout << "error allocating derotated_in" << std::endl;
                return;
            }
        }
        /*
         * ---------------------------------------------------------------------
         */

        /*
         * Our virtual destructor.
         */
        sync_and_channel_estimaton_impl::~sync_and_channel_estimaton_impl()
        {
        }
        /*
         * ---------------------------------------------------------------------
         */

        /*
         * ---------------------------------------------------------------------
         */
        void
            sync_and_channel_estimaton_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
            {
                /*
                 * Default implementation is:
                 *
                 *	unsigned ninputs = ninput_items_required.size ();
                 *	for(unsigned i = 0; i < ninputs; i++)
                 *	ninput_items_required[i] = noutput_items;
                 */
                int ninputs = ninput_items_required.size();
                for (int i = 0; i < ninputs; i++)
                    ninput_items_required[i] = 2*noutput_items;
            }
        /*
         * ---------------------------------------------------------------------
         */

        /*
         * process_tmcc_data function
         * post-fft frequency offset estimation
         * -----------------------------------------------------------------------------------------
         */
        void
            sync_and_channel_estimaton_impl::process_tmcc_data(const gr_complex * in)
            {
                // Look for maximum correlation for tmccs
                // in order to obtain postFFT integer frequency correction

                float max = 0;
                gr_complex sum = 0;
                int start = 0;
                gr_complex phase;

                // for d_zeros_on_left +/- d_freq_offset_max...
                for (int i = d_zeros_on_left - d_freq_offset_max; i < d_zeros_on_left + d_freq_offset_max; i++)
                {
                    sum = 0;
                    for (int j = 0; j < (tmcc_carriers_size - 1); j++)
                    {

                        if (d_known_phase_diff[j] == 0)
                            // If the phase difference between tmcc_carriers[j+1] and tmcc_carriers[j] is zero, is because we expect both to be always in phase
                            phase = in[i + tmcc_carriers[j + 1]]*conj(in[i + tmcc_carriers[j]]);
                        else
                            // If the phase difference between tmcc_carriers[j+1] and tmcc_carriers[j] is not zero, is because we expect both to be always out of phase (180 degrees)
                            phase = -in[i + tmcc_carriers[j + 1]]*conj(in[i + tmcc_carriers[j]]);
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
                d_freq_offset = start - d_zeros_on_left;

            }


        gr_complex *
            sync_and_channel_estimaton_impl::frequency_correction(const gr_complex * in, gr_complex * out)
            {
                // We get the derotated carrier's sequence, i.e. the sequence without the frequency offset
                for (int k = 0; k < d_fft_length; k++)
                {
                    out[k] = in[k + d_freq_offset];
                }
                return (out);
            }

        /*
         * process_sp_data function
         * post-fft frequency offset estimation
         * -----------------------------------------------------------------------------------------
         */

        void
            sync_and_channel_estimaton_impl::process_sp_data(const gr_complex * in)
            {
                /* First thing is to locate the scattered pilots
                */

                /*************************************************************/
                // Find out the OFDM symbol index (value 0 to 3) sent
                // in current block by correlating scattered symbols with
                // current block - result is (symbol index % 4)
                /*************************************************************/
                float max = 0;
                gr_complex sum;
                //int current_symbol = 0;
                d_previous_symbol = d_current_symbol;


                int next_sp_carrier; // The next sp carrier
                int current_sp_carrier; // The current sp carrier
                gr_complex phase;
                // sym_count (Symbol count) can take values 0 to 3, according to the positions of the scattered pilots
                for (int sym_count = 0; sym_count < 4; sym_count++)
                {
                    sum = 0;
                    // For every scattered pilot but the last one...
                    for (int i=0; i < sp_carriers_size-1; i++)
                    {
                        next_sp_carrier = 12*(i+1)+3*sym_count; // Get the position of the next sp carrier based on the value of sym_count
                        current_sp_carrier = 12*i+3*sym_count; // Get the position of the current sp carrier based on the value of sym_count

                        if (d_wk[next_sp_carrier]==d_wk[current_sp_carrier])
                            // If the phase difference between in[next_sp_carrier+d_zeros_on_left] and in[current_sp_carrier+d_zeros_on_left] is zero,
                            // is because we expect both to be always in phase
                            phase = in[next_sp_carrier+d_zeros_on_left]*conj(in[current_sp_carrier+d_zeros_on_left]);
                        else
                            // If the phase difference between in[next_sp_carrier+d_zeros_on_left] and in[current_sp_carrier+d_zeros_on_left] is not zero,
                            // is because we expect both to be always out of phase (180 degrees)
                            phase = -in[next_sp_carrier+d_zeros_on_left]*conj(in[current_sp_carrier+d_zeros_on_left]);
                        sum += phase;
                    }
                    if (abs(sum)>max)
                    {
                        // When sum is maximum is because the current symbol is of type sym_count (0, 1, 2 or 3)
                        max = abs(sum);
                        d_current_symbol = sym_count;
                    }
                }


                /*************************************************************/
                // Keep data for channel estimator
                // This method interpolates scattered measurements across one OFDM symbol
                // It does not use measurements from the previous OFDM symnbols (does not use history)
                // as it may have encountered a phase change for the current phase only
                /*************************************************************/

                // We first calculate the channel gain on the SP carriers.
                for (int i = 0; i < sp_carriers_size; i++)
                {
                    // We get each sp carrier position. We now know which is the current symbol (0, 1, 2 or 3)
                    current_sp_carrier = 12*i+3*d_current_symbol;

                    // channel gain = (sp carrier actual value)/(sp carrier expected value)
                    d_channel_gain[current_sp_carrier] = in[current_sp_carrier+d_zeros_on_left]/get_pilot_value(current_sp_carrier);

                }
                //we then calculate the gain on the CP
                d_channel_gain[active_carriers-1] = in[active_carriers-1]/get_pilot_value(active_carriers-1);

                // We then interpolate to obtain the channel gain on the rest of the carriers
                // TODO interpolation is too simple, a new method(s) should be implemented
                gr_complex tg_alpha;
                for (int i = 0; i < sp_carriers_size-1; i++)
                {
                    // Current sp carrier
                    current_sp_carrier = 12*i+3*d_current_symbol;
                    // Next sp carrier
                    next_sp_carrier = 12*(i+1)+3*d_current_symbol;
                    // Calculate tg(alpha) due to linear interpolation
                    tg_alpha = (d_channel_gain[next_sp_carrier] - d_channel_gain[current_sp_carrier]) / gr_complex(12.0, 0.0);


                    // Calculate interpolation for all intermediate values
                    for (int j = 1; j < next_sp_carrier-current_sp_carrier; j++)
                    {
                        d_channel_gain[current_sp_carrier+j] = d_channel_gain[current_sp_carrier] + tg_alpha * gr_complex(j, 0.0);
                    }

                }

                /////////////////////////////////////////////////////////////
                //take care of extreme cases: first carriers and last carriers
                /////////////////////////////////////////////////////////////
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
                current_sp_carrier = 12*(sp_carriers_size-1)+3*d_current_symbol;
                next_sp_carrier = active_carriers-1;
                tg_alpha = (d_channel_gain[next_sp_carrier] - d_channel_gain[current_sp_carrier]) / gr_complex(next_sp_carrier-current_sp_carrier, 0.0);
                for (int j = 1; j < next_sp_carrier-current_sp_carrier; j++)
                {
                    d_channel_gain[current_sp_carrier+j] = d_channel_gain[current_sp_carrier] + tg_alpha * gr_complex(j, 0.0);
                }



            }

        /*
         *
         * general_work function
         * Here is where all the signal processing is done
         * ----------------------------------------------------------------------------------------
         */

        int
            sync_and_channel_estimaton_impl::general_work (int noutput_items,
                    gr_vector_int &ninput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {

                const gr_complex *in = (const gr_complex *) input_items[0];
                gr_complex *out = (gr_complex *) output_items[0];

                /*
                 * Here starts the signal processing
                 */

                for (int i = 0; i < noutput_items; i++)
                {
                    // Process tmcc data
                    process_tmcc_data(&in[i* d_ninput]);

                    // Correct ofdm symbol integer frequency error
                    frequency_correction(&in[i* d_ninput], derotated_in);

                    // Find out the OFDM symbol index and get the d_channel_gain vector values in order to equalize the channel
                    process_sp_data(&derotated_in[i*d_ninput]);

                    // Assign the output and equalize the channel
                    for (int carrier = 0; carrier < active_carriers; carrier++)
                    {
                        out[i*d_noutput +carrier] = derotated_in[carrier+d_zeros_on_left]/d_channel_gain[carrier];
                        gr_complex salida = out[i*d_noutput+carrier];
                        if(isinf(out[i*d_noutput + carrier].real()))
                            printf("CUIDADO: out[%d]=%f+j%f: gain=%f+j%f\n",i*d_noutput+carrier,salida.real(),salida.imag(),d_channel_gain[carrier].real(),d_channel_gain[carrier].imag());
                    }


                    int diff = d_current_symbol-d_previous_symbol;
                    // If there is any symbol lost, print stuff and signal it downstream
                    if ((diff != 1) && (diff !=-3)){
                        printf("previous_symbol: %i, \n current_symbol: %i\n", d_previous_symbol, d_current_symbol);
                        /*int skip_len = diff-1;
                        //TODO there is probably a much better way to do this. 
                        if (diff<0)
                        skip_len = 3+diff;
                        if (diff==0)
                        skip_len = 3;

                        const uint64_t offset = this->nitems_written(0)+i; 
                        pmt::pmt_t key = pmt::string_to_symbol("symbol_index_skip"); 
                        pmt::pmt_t value = pmt::from_long(skip_len); 
                        this->add_item_tag(0,offset,key,value); */
                    }

                    // signal downstream the relative symbol index found here. 
                    const uint64_t offset = this->nitems_written(0)+i; 
                    pmt::pmt_t key = pmt::string_to_symbol("relative_symbol_index"); 
                    pmt::pmt_t value = pmt::from_long(d_current_symbol); 
                    this->add_item_tag(0,offset,key,value);

                }

                /*
                 * Here ends the signal processing
                 */

                // Tell runtime system how many input items we consumed on
                // each input stream.
                consume_each (noutput_items);

                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

