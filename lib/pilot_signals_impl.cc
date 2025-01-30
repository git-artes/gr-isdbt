/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017, 2018, 2019, 2020, 2021
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
#include "pilot_signals_impl.h"
#include <volk/volk.h>

namespace gr {
    namespace isdbt {

        const int pilot_signals_impl::d_total_segments = 13;
        const int pilot_signals_impl::d_data_carriers_per_segment_2k = 96;
        const int pilot_signals_impl::d_carriers_per_segment_2k = 108;  

        // The segments positions
        const int pilot_signals_impl::d_segments_positions[d_total_segments] = {11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12};
        //const int pilot_signals_impl::d_segments_positions[d_total_segments] = {6, 5, 7, 4, 8, 3, 9, 2, 10, 1, 11, 0, 12};

        // TMCC carriers positions for each transmission mode
        // TODO There are some variables and methods used by both the TX and RX. I've copy-pasted them, but should
        // do something more intelligent.
        // Mode 1 (2K)
        const int pilot_signals_impl::tmcc_carriers_size_2k = 13;
        const int pilot_signals_impl::tmcc_carriers_2k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319
        };
        // Mode 2 (4K)
        const int pilot_signals_impl::tmcc_carriers_size_4k = 26;
        const int pilot_signals_impl::tmcc_carriers_4k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319, 1474, 1537, 1637,\
                1814, 1880, 1991, 2101,	2191, 2351, 2437, 2569,\
                2693, 2723
        };
        // Mode 3 (8K)
        const int pilot_signals_impl::tmcc_carriers_size_8k = 52;
        const int pilot_signals_impl::tmcc_carriers_8k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319, 1474, 1537, 1637,\
                1814, 1880, 1991, 2101, 2191, 2351, 2437, 2569,\
                2693, 2723, 2878, 2941, 3041, 3218, 3284, 3395,\
                3505, 3595, 3755, 3841, 3973, 4097, 4127, 4282,\
                4345, 4445, 4622, 4688, 4799, 4909, 4999, 5159,\
                5245, 5377, 5501, 5531
        };

        // Auxiliary Channel (AC) position for each transmission mode
        // TODO There are some variables and methods used by both the TX and RX. I've copy-pasted them, but should
        // do something more intelligent.
        // Mode 1 (2K)
        const int pilot_signals_impl::ac_carriers_size_2k = 26;
        const int pilot_signals_impl::ac_carriers_2k[] = {
            10, 28, 161, 191, 277, 316, 335, 425,\
                452, 472, 614, 640, 683, 727, 832, 853,\
                868, 953, 1012, 1061, 1088, 1144, 1195, 1277,\
                1394, 1397
        };

        // Mode 2 (4K)
        const int pilot_signals_impl::ac_carriers_size_4k = 52;
        const int pilot_signals_impl::ac_carriers_4k[] = {
            10, 28, 161, 191, 277, 316, 335, 425,\
                452, 472, 614, 640, 683, 727, 832, 853,\
                868, 953, 1012, 1061, 1088, 1144, 1195, 1277,\
                1394, 1397, 1414, 1432, 1565, 1595, 1681, 1720,\
                1739, 1829, 1856, 1876, 2018, 2044, 2087, 2131,\
                2236, 2257, 2272, 2357, 2416, 2465, 2492, 2548,\
                2599, 2681, 2798, 2801
        };

        // Mode 3 (8K)
        const int pilot_signals_impl::ac_carriers_size_8k = 104;
        const int pilot_signals_impl::ac_carriers_8k[] = {
            10, 28, 161, 191, 277, 316, 335, 425,\
                452, 472, 614, 640, 683, 727, 832, 853,\
                868, 953, 1012, 1061, 1088, 1144, 1195, 1277,\
                1394, 1397, 1414, 1432, 1565, 1595, 1681, 1720,\
                1739, 1829, 1856, 1876, 2018, 2044, 2087, 2131,\
                2236, 2257, 2272, 2357, 2416, 2465, 2492, 2548,\
                2599, 2681, 2798, 2801, 2818, 2836, 2969, 2999,\
                3085, 3124, 3143, 3233, 3260, 3280, 3422, 3448,\
                3491, 3535, 3640, 3661, 3676, 3761, 3820, 3869,\
                3896, 3952, 4003, 4085, 4202, 4205, 4222, 4240,\
                4373, 4403, 4489, 4528, 4547, 4637, 4664, 4684,\
                4826, 4852, 4895, 4939, 5044, 5065, 5080, 5165,\
                5224, 5273, 5300, 5356, 5407, 5489, 5606, 5609 };


        pilot_signals::sptr
            pilot_signals::make(int mode)
            {
                return gnuradio::get_initial_sptr
                    (new pilot_signals_impl(mode));
            }

        /*
         * The private constructor
         */
        pilot_signals_impl::pilot_signals_impl(int mode)
            : gr::sync_block("pilot_signals",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*(d_total_segments*d_data_carriers_per_segment_2k*((int)pow(2.0,mode-1)))),
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*((int)pow(2.0,10+mode))))
        {

            d_fft_length = pow(2.0,10+mode); 
            d_active_carriers = (1+d_total_segments*d_carriers_per_segment_2k*pow(2.0,mode-1)); 
            d_data_carriers_size = d_total_segments*d_data_carriers_per_segment_2k*pow(2.0,mode-1); 


            //VOLK alignment as recommended by GNU Radio's Manual. It has a similar effect 
            //than set_output_multiple(), thus we will generally get multiples of this value
            //as noutput_items. 
            const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
            set_alignment(std::max(1, alignment_multiple));

            d_zeros_on_left = int(ceil((d_fft_length-d_active_carriers)/2.0)); 

            // I generate the pilot's value. It will be used for setting the SP, and performing the XOR in the TMCC carriers. 
            d_pilot_values = new gr_complex[d_active_carriers];
            generate_prbs();

            //  assign the TMCC carrier positions based on the mode
            tmcc_and_ac_positions(d_fft_length); 

            d_carriers_per_segment = (d_active_carriers-1)/d_total_segments;
            d_data_carriers_per_segment = d_data_carriers_size/d_total_segments;

            d_sp_carriers_size = (d_active_carriers-1)/12; 

            d_current_symbol = 0;

            // I pre-calculate the data carriers positions. It will save us many unnecesary computations
            d_data_carriers_out = new int[4*d_data_carriers_size];
            data_carriers_position();

        }

        /*
         * Our virtual destructor.
         */
        pilot_signals_impl::~pilot_signals_impl()
        {
            delete [] d_pilot_values; 
            delete [] d_data_carriers_out; 
        }

        void
            pilot_signals_impl::generate_prbs()
            {
                // Generate the prbs sequence for each active carrier

                // Init PRBS register with 1s (reg_prbs = 111111111111)
                unsigned int reg_prbs = (1 << 11) - 1;
                char aux; 
                for (int k = 0; k < (d_active_carriers); k++)
                {
                    aux = (char)(reg_prbs & 0x01); // Get the LSB of the register as a character
                    int new_bit = ((reg_prbs >> 2) ^ (reg_prbs >> 0)) & 0x01; // This is the bit we will add to the register as MSB
                    reg_prbs = (reg_prbs >> 1) | (new_bit << 10); // We move all the register to the right and add the new_bit as MSB
                    d_pilot_values[k] = gr_complex((float)(4 * 2 * (0.5 - aux)) / 3, 0);
                }
                //printf("d_pilot_value[%i]: %f\n", d_active_carriers-1, d_pilot_values[d_active_carriers-1].real());
            }

        void
            pilot_signals_impl::tmcc_and_ac_positions(int fft)
            {
                /*
                 * Assign to variables ac_carriers, ac_carriers_size, tmcc_carriers and tmcc_carriers_size
                 * the corresponding values according to the transmission mode
                 */
                switch (fft)
                {
                    case 2048:
                        {
                            d_tmcc_carriers = tmcc_carriers_2k;
                            d_tmcc_carriers_size = tmcc_carriers_size_2k;
                            d_ac_carriers = ac_carriers_2k;
                            d_ac_carriers_size = ac_carriers_size_2k;
                        }
                        break;
                    case 4096:
                        {
                            d_tmcc_carriers = tmcc_carriers_4k;
                            d_tmcc_carriers_size = tmcc_carriers_size_4k;
                            d_ac_carriers = ac_carriers_4k;
                            d_ac_carriers_size = ac_carriers_size_4k;
                        }
                        break;
                    case 8192:
                        {
                            d_tmcc_carriers = tmcc_carriers_8k;
                            d_tmcc_carriers_size = tmcc_carriers_size_8k;
                            d_ac_carriers = ac_carriers_8k;
                            d_ac_carriers_size = ac_carriers_size_8k;
                        }
                        break;
                    default:
                        break;
                }
            }

        void
            pilot_signals_impl::data_carriers_position()
            {
                /*
                 * Assign to variables d_data_carriers_out the position of data carriers for the 4 possible symbols
                 * This is inspired from tmcc_decoder_impl.cc. It's not exactly the same method. 
                 */

                for (int symbol_index = 0; symbol_index<4; symbol_index++){
                    int spilot_index = 0; 
                    int ac_pilot_index = 0; 
                    int tmcc_pilot_index = 0; 
                    int carrier_in = 0; 


                    for (int carrier_out = 0; carrier_out < (d_active_carriers - 1);carrier_out++) {

                        if (carrier_out == (12 * spilot_index + 3 * (symbol_index % 4))) 
                        {
                            // the current carrier is an scattered pilot
                            spilot_index++;
                        } 
                        else if ((carrier_out == d_ac_carriers[ac_pilot_index]))
                        {
                            // the current carrier is an AC pilot
                            ac_pilot_index++;
                        } 
                        else if ((carrier_out == d_tmcc_carriers[tmcc_pilot_index])) 
                        {
                            // the current carrier is a tmcc pilot
                            tmcc_pilot_index++;
                        } 
                        else 
                        {
                            // carrier_out is thus a data carrier. 
                            d_data_carriers_out[symbol_index*d_data_carriers_size + d_segments_positions[carrier_in/d_data_carriers_per_segment]*d_data_carriers_per_segment + (carrier_in%d_data_carriers_per_segment)] = d_zeros_on_left + carrier_out; 

                            //d_data_carriers_out[symbol_index*d_data_carriers_size + carrier_in] = carrier_out; 
                            carrier_in++;
                        }
                    }
                }


            }



        int
            pilot_signals_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const gr_complex *in = (const gr_complex *) input_items[0];
                gr_complex *out = (gr_complex *) output_items[0];

                int carrier = 0;

                for (int i=0; i < noutput_items; i++) {

                    // fill with the first zeros
                    for (carrier = 0; carrier < d_zeros_on_left; carrier++) {
                        out[i*d_fft_length+carrier] = 0;
                    }

                    // copy the input at the right carriers
                    for (carrier = 0 ; carrier < d_data_carriers_size; carrier++) {
                        //out[i*d_fft_length+carrier] = in[i*d_active_carriers+carrier-d_zeros_on_left];

                        out[i*d_fft_length + d_data_carriers_out[d_data_carriers_size*d_current_symbol + carrier]] = in[i*d_data_carriers_size + carrier]; 
                    }

                    // fill with the last zeros
                    for (carrier = d_zeros_on_left+d_active_carriers ; carrier < d_fft_length; carrier++) {
                        out[i*d_fft_length+carrier] = 0;
                    }

                    // I now set the scattered pilots 
                    for (int current_sp_carrier = 3*d_current_symbol; current_sp_carrier < d_active_carriers-1; current_sp_carrier+=12) {
                        out[i*d_fft_length+d_zeros_on_left+current_sp_carrier] = d_pilot_values[current_sp_carrier]; 
                    }

                    // I now set the continual pilot
                    out[i*d_fft_length+d_zeros_on_left+d_active_carriers-1] = d_pilot_values[d_active_carriers-1]; 

                    // I now assign the tmcc carriers with the PRBS
                    for (int current_tmcc_carrier = 0; current_tmcc_carrier < d_tmcc_carriers_size; current_tmcc_carrier++) {
                        out[i*d_fft_length+d_zeros_on_left+d_tmcc_carriers[current_tmcc_carrier]] = d_pilot_values[d_tmcc_carriers[current_tmcc_carrier]]; 
                    }

                    // I now assign the ac carriers with the PRBS
                    for (int current_ac_carrier = 0; current_ac_carrier < d_ac_carriers_size; current_ac_carrier++) {
                        out[i*d_fft_length+d_zeros_on_left+d_ac_carriers[current_ac_carrier]] = d_pilot_values[d_ac_carriers[current_ac_carrier]]; 
                    }


                    d_current_symbol = (d_current_symbol + 1) % 4;
                }


                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */


