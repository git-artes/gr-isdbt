/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017, 2018
 *   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
 *   Javier Hernández
 *   Santiago Castro
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
#include "tmcc_encoder_impl.h"

namespace gr {
    namespace isdbt {

        const int tmcc_encoder_impl::d_total_segments = 13; 
        const int tmcc_encoder_impl::d_carriers_per_segment_2k = 108;

        //Note that lsb bits are indexed by 0 in the bitset. Thus, all
        //bit-words may seem at first as reversed. 
        const std::bitset<3> tmcc_encoder_impl::d_mod_scheme_qpsk = 0b100;
        const std::bitset<3> tmcc_encoder_impl::d_mod_scheme_16qam = 0b010;
        const std::bitset<3> tmcc_encoder_impl::d_mod_scheme_64qam = 0b110;
        const std::bitset<3> tmcc_encoder_impl::d_mod_scheme_unused = 0b111;

        const std::bitset<3> tmcc_encoder_impl::d_conv_code_12 = 0b000;
        const std::bitset<3> tmcc_encoder_impl::d_conv_code_23 = 0b100;
        const std::bitset<3> tmcc_encoder_impl::d_conv_code_34 = 0b010;
        const std::bitset<3> tmcc_encoder_impl::d_conv_code_56 = 0b110;
        const std::bitset<3> tmcc_encoder_impl::d_conv_code_78 = 0b001;
        const std::bitset<3> tmcc_encoder_impl::d_conv_code_unused = 0b111;

        const std::bitset<4> tmcc_encoder_impl::d_nsegs[] = {
            0b1000, 0b0100, 0b1100, 0b0010, 0b1010, 0b0110, 0b1110, 0b0001, 0b1001, 0b0101, 0b1101, 0b0011, 0b1011
        };


        const std::bitset<3> tmcc_encoder_impl::d_int_length_0 = 0b000;
        const std::bitset<3> tmcc_encoder_impl::d_int_length_421 = 0b100;
        const std::bitset<3> tmcc_encoder_impl::d_int_length_842 = 0b010;
        const std::bitset<3> tmcc_encoder_impl::d_int_length_1684 = 0b110;

        // Mode 1 (2K)
        const int tmcc_encoder_impl::tmcc_carriers_size_2k = 13;
        const int tmcc_encoder_impl::tmcc_carriers_2k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319
        };
        // Mode 2 (4K)
        const int tmcc_encoder_impl::tmcc_carriers_size_4k = 26;
        const int tmcc_encoder_impl::tmcc_carriers_4k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319, 1474, 1537, 1637,\
                1814, 1880, 1991, 2101,	2191, 2351, 2437, 2569,\
                2693, 2723
        };
        // Mode 3 (8K)
        const int tmcc_encoder_impl::tmcc_carriers_size_8k = 52;
        const int tmcc_encoder_impl::tmcc_carriers_8k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319, 1474, 1537, 1637,\
                1814, 1880, 1991, 2101, 2191, 2351, 2437, 2569,\
                2693, 2723, 2878, 2941, 3041, 3218, 3284, 3395,\
                3505, 3595, 3755, 3841, 3973, 4097, 4127, 4282,\
                4345, 4445, 4622, 4688, 4799, 4909, 4999, 5159,\
                5245, 5377, 5501, 5531
        };


        tmcc_encoder::sptr
            tmcc_encoder::make(int mode, bool one_seg_present, int mod_scheme_A, int mod_scheme_B, int mod_scheme_C, int conv_code_A, int conv_code_B, int conv_code_C, int int_length_A, int int_length_B, int int_length_C, int nsegs_A, int nsegs_B, int nsegs_C)
            {
                return gnuradio::get_initial_sptr
                    (new tmcc_encoder_impl(mode, one_seg_present, mod_scheme_A, mod_scheme_B, mod_scheme_C, conv_code_A, conv_code_B, conv_code_C, int_length_A, int_length_B, int_length_C, nsegs_A, nsegs_B, nsegs_C));
            }

        /*
         * The private constructor
         */
        tmcc_encoder_impl::tmcc_encoder_impl(int mode, bool one_seg_present, int mod_scheme_A, int mod_scheme_B, int mod_scheme_C, int conv_code_A, int conv_code_B, int conv_code_C, int int_length_A, int int_length_B, int int_length_C, int nsegs_A, int nsegs_B, int nsegs_C)
            : gr::sync_block("tmcc_encoder",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*((int)pow(2.0,10+mode))),
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*((int)pow(2.0,10+mode))))
        {

            d_mode = mode; 
            d_one_seg_present = one_seg_present;
            d_mod_scheme_A = mod_scheme_A; 
            d_mod_scheme_B = mod_scheme_B; 
            d_mod_scheme_C = mod_scheme_C; 
            d_conv_code_A = conv_code_A; 
            d_conv_code_B = conv_code_B; 
            d_conv_code_C = conv_code_C; 
            d_int_length_A = int_length_A;  
            d_int_length_B = int_length_B;  
            d_int_length_C = int_length_C; 
            d_nsegs_A = nsegs_A; 
            d_nsegs_B = nsegs_B; 
            d_nsegs_C = nsegs_C; 

            d_fft_length = pow(2.0,10+mode); 
            // I calculate the tmcc word, which is assigned to d_tmcc_word
            calculate_tmcc_word();
            std::bitset<82> parity = compute_parity();
            bitwise_copy_into(d_tmcc_word, parity, 122);

            tmcc_carrier_positions();

            d_symbol_count = 0;
            d_active_carriers = (1+d_total_segments*d_carriers_per_segment_2k*pow(2.0,mode-1)); 
            d_zeros_on_left = int(ceil((d_fft_length-d_active_carriers)/2.0)); 

        }

        /*
         * Our virtual destructor.
         */
        tmcc_encoder_impl::~tmcc_encoder_impl()
        {
        }


        template<size_t N1, size_t N2>
        void tmcc_encoder_impl::bitwise_copy_into(std::bitset<N1>& b_destination, const std::bitset<N2>& b_origin, size_t pos)
        {
            for(size_t k = pos; k < pos + b_origin.size(); k++) {
                //b_destination[k] = b_origin[k];
                b_destination.set(k, b_origin.test(k-pos));
            } 
        }

        void tmcc_encoder_impl::calculate_tmcc_word()
        {

            // The first bit is always 1 (it should be the reference and given by w_i)
            d_tmcc_word.set(0);

            // I first assign the sync word at the beginning of the TMCC 
            //Note that lsb bits are indexed by 0 in the bitset. Thus, the
            //sync word (and the rest of the bit-words) may seem at first as reversed. 
            d_tmcc_sync_word = 0b0111011110101100;
            d_tmcc_sync_word.flip();
            //d_tmcc_sync_word = 0b1100101000010001;
            //for(int i=0; i<d_tmcc_word.size(); i++)
            //{
            //    d_tmcc_word.set(i,d_tmcc_sync_word.test(i));
            //}

            bitwise_copy_into(d_tmcc_word, d_tmcc_sync_word, 1);

            //Segment type identifier
            //Our implementation supports coherent modulation only
            d_tmcc_word.set(17);
            d_tmcc_word.set(18);
            d_tmcc_word.set(19);

            //////////////////////////////////////////////
            //////////TMCC information bits
            /////////////////////////////////////////////

            //system id: 00 for terrestrial (this case)
            d_tmcc_word.reset(20);
            d_tmcc_word.reset(21);

            //Transmission parameters switching. 
            //on-line parameter changing is (currently) not supported on this transmitter. Thus, all ones
            d_tmcc_word.set(22);
            d_tmcc_word.set(23);
            d_tmcc_word.set(24); 
            d_tmcc_word.set(25); 

            // Emergency-Alarm broadcasting. For now, we leave it as "no alarm". 
            d_tmcc_word.reset(26); 

            // Partial-reception flag
            // depends on current config
            d_tmcc_word.set(27,d_one_seg_present);


            //////////////////////////////////////////////
            // Now I configure the modulation schemes for all three layers
            // a long and boring code follows. More boring code follows after that... just boredom
            //////////////////////////////////////////////
            if (d_mod_scheme_A == 4 && d_nsegs_A > 0)
            {
                //current
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_qpsk, 28);
                //future (unimplemented as of now)
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_qpsk, 68);
            } 
            else if (d_mod_scheme_A == 16 && d_nsegs_A > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_16qam, 28);
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_16qam, 68);
            }
            else if (d_mod_scheme_A == 64 && d_nsegs_A > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_64qam, 28);
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_64qam, 68);
            }
            else
            {
                // I assume it is not used
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_unused, 28);
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_unused, 68);
            }

            if (d_mod_scheme_B == 4 && d_nsegs_B > 0)
            {
                //current
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_qpsk, 41);
                //future (unimplemented as of now)
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_qpsk, 81);
            } 
            else if (d_mod_scheme_B == 16 && d_nsegs_B > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_16qam, 41);
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_16qam, 81);
            }
            else if (d_mod_scheme_B == 64 && d_nsegs_B > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_64qam, 41);
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_64qam, 81);
            }
            else
            {
                // I assume it is not used
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_unused, 41);
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_unused, 81);
            }

            if (d_mod_scheme_C == 4 && d_nsegs_C > 0)
            {
                //current
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_qpsk, 54);
                //future (unimplemented as of now)
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_qpsk, 94);
            } 
            else if (d_mod_scheme_C == 16 && d_nsegs_C > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_16qam, 54);
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_16qam, 94);
            }
            else if (d_mod_scheme_C == 64 && d_nsegs_C > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_64qam, 54);
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_64qam, 94);
            }
            else
            {
                // I assume it is not used
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_unused, 54);
                bitwise_copy_into(d_tmcc_word, d_mod_scheme_unused, 94);
            }

            //////////////////////////////////////////////
            // now the encoding rate
            //////////////////////////////////////////////
            if (d_conv_code_A == 0 && d_nsegs_A > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_12, 31);
                bitwise_copy_into(d_tmcc_word, d_conv_code_12, 71);
            }
            else if(d_conv_code_A == 1 && d_nsegs_A > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_23, 31);
                bitwise_copy_into(d_tmcc_word, d_conv_code_23, 71);
            }
            else if(d_conv_code_A == 2 && d_nsegs_A > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_34, 31);
                bitwise_copy_into(d_tmcc_word, d_conv_code_34, 71);
            }
            else if(d_conv_code_A == 3 && d_nsegs_A > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_56, 31);
                bitwise_copy_into(d_tmcc_word, d_conv_code_56, 71);
            }
            else if(d_conv_code_A == 4 && d_nsegs_A > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_78, 31);
                bitwise_copy_into(d_tmcc_word, d_conv_code_78, 71);
            }
            else
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_unused, 31);
                bitwise_copy_into(d_tmcc_word, d_conv_code_unused, 71);
            }

            if (d_conv_code_B == 0 && d_nsegs_B > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_12, 44);
                bitwise_copy_into(d_tmcc_word, d_conv_code_12, 84);
            }
            else if(d_conv_code_B == 1 && d_nsegs_B > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_23, 44);
                bitwise_copy_into(d_tmcc_word, d_conv_code_23, 84);
            }
            else if(d_conv_code_B == 2 && d_nsegs_B > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_34, 44);
                bitwise_copy_into(d_tmcc_word, d_conv_code_34, 84);
            }
            else if(d_conv_code_B == 3 && d_nsegs_B > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_56, 44);
                bitwise_copy_into(d_tmcc_word, d_conv_code_56, 84);
            }
            else if(d_conv_code_B == 4 && d_nsegs_B > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_78, 44);
                bitwise_copy_into(d_tmcc_word, d_conv_code_78, 84);
            }
            else
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_unused, 44);
                bitwise_copy_into(d_tmcc_word, d_conv_code_unused, 84);
            }

            if (d_conv_code_C == 0 && d_nsegs_C > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_12, 57);
                bitwise_copy_into(d_tmcc_word, d_conv_code_12, 97);
            }
            else if(d_conv_code_C == 1 && d_nsegs_C > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_23, 57);
                bitwise_copy_into(d_tmcc_word, d_conv_code_23, 97);
            }
            else if(d_conv_code_C == 2 && d_nsegs_C > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_34, 57);
                bitwise_copy_into(d_tmcc_word, d_conv_code_34, 97);
            }
            else if(d_conv_code_C == 3 && d_nsegs_C > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_56, 57);
                bitwise_copy_into(d_tmcc_word, d_conv_code_56, 97);
            }
            else if(d_conv_code_C == 4 && d_nsegs_C > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_78, 57);
                bitwise_copy_into(d_tmcc_word, d_conv_code_78, 97);
            }
            else
            {
                bitwise_copy_into(d_tmcc_word, d_conv_code_unused, 57);
                bitwise_copy_into(d_tmcc_word, d_conv_code_unused, 97);
            }

            //////////////////////////////////////////////
            //// now the interleaver length
            //// in this case it depends on the length and
            //// the mode
            //////////////////////////////////////////////

            if(d_int_length_A == 0 && d_nsegs_A > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_int_length_0, 34); 
                bitwise_copy_into(d_tmcc_word, d_int_length_0, 74); 
            }
            else if((d_mode == 1 && d_int_length_A == 4 && d_nsegs_A > 0) || 
                    (d_mode == 2 && d_int_length_A == 2 && d_nsegs_A > 0) || 
                    (d_mode == 3 && d_int_length_A == 1 && d_nsegs_A > 0) )
            {
                bitwise_copy_into(d_tmcc_word, d_int_length_421, 34); 
                bitwise_copy_into(d_tmcc_word, d_int_length_421, 74); 
            }
            else if((d_mode == 1 && d_int_length_A == 8 && d_nsegs_A > 0) || 
                    (d_mode == 2 && d_int_length_A == 4 && d_nsegs_A > 0) || 
                    (d_mode == 3 && d_int_length_A == 2 && d_nsegs_A > 0) )
            {
                bitwise_copy_into(d_tmcc_word, d_int_length_842, 34); 
                bitwise_copy_into(d_tmcc_word, d_int_length_842, 74); 
            }
            else if((d_mode == 1 && d_int_length_A == 16 && d_nsegs_A > 0) || 
                    (d_mode == 2 && d_int_length_A == 8 && d_nsegs_A > 0) || 
                    (d_mode == 3 && d_int_length_A == 4 && d_nsegs_A > 0) )
            {
                bitwise_copy_into(d_tmcc_word, d_int_length_1684, 34); 
                bitwise_copy_into(d_tmcc_word, d_int_length_1684, 74); 
            }
            else
            {
                // I assume is unused
                bitwise_copy_into(d_tmcc_word, std::bitset<3>(0b111), 34); 
                bitwise_copy_into(d_tmcc_word, std::bitset<3>(0b111), 74); 
            }

            if(d_int_length_B == 0 && d_nsegs_B > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_int_length_0, 47); 
                bitwise_copy_into(d_tmcc_word, d_int_length_0, 87); 
            }
            else if((d_mode == 1 && d_int_length_B == 4 && d_nsegs_B > 0) || 
                    (d_mode == 2 && d_int_length_B == 2 && d_nsegs_B > 0) || 
                    (d_mode == 3 && d_int_length_B == 1 && d_nsegs_B > 0) )
            {
                bitwise_copy_into(d_tmcc_word, d_int_length_421, 47); 
                bitwise_copy_into(d_tmcc_word, d_int_length_421, 87); 
            }
            else if((d_mode == 1 && d_int_length_B == 8 && d_nsegs_B > 0) || 
                    (d_mode == 2 && d_int_length_B == 4 && d_nsegs_B > 0) || 
                    (d_mode == 3 && d_int_length_B == 2 && d_nsegs_B > 0) )
            {
                bitwise_copy_into(d_tmcc_word, d_int_length_842, 47); 
                bitwise_copy_into(d_tmcc_word, d_int_length_842, 87); 
            }
            else if((d_mode == 1 && d_int_length_B == 16 && d_nsegs_B > 0) || 
                    (d_mode == 2 && d_int_length_B == 8 && d_nsegs_B > 0) || 
                    (d_mode == 3 && d_int_length_B == 4 && d_nsegs_B > 0) )
            {
                bitwise_copy_into(d_tmcc_word, d_int_length_1684, 47); 
                bitwise_copy_into(d_tmcc_word, d_int_length_1684, 87); 
            }
            else
            {
                // unused
                bitwise_copy_into(d_tmcc_word, std::bitset<3>(0b111), 47); 
                bitwise_copy_into(d_tmcc_word, std::bitset<3>(0b111), 87); 
            }

            if(d_int_length_C == 0 && d_nsegs_C > 0)
            {
                bitwise_copy_into(d_tmcc_word, d_int_length_0, 60); 
                bitwise_copy_into(d_tmcc_word, d_int_length_0, 87); 
            }
            else if((d_mode == 1 && d_int_length_C == 4 && d_nsegs_C > 0) || 
                    (d_mode == 2 && d_int_length_C == 2 && d_nsegs_C > 0) || 
                    (d_mode == 3 && d_int_length_C == 1 && d_nsegs_C > 0) )
            {
                bitwise_copy_into(d_tmcc_word, d_int_length_421, 60); 
                bitwise_copy_into(d_tmcc_word, d_int_length_421, 100); 
            }
            else if((d_mode == 1 && d_int_length_C == 8 && d_nsegs_C > 0) || 
                    (d_mode == 2 && d_int_length_C == 4 && d_nsegs_C > 0) || 
                    (d_mode == 3 && d_int_length_C == 2 && d_nsegs_C > 0) )
            {
                bitwise_copy_into(d_tmcc_word, d_int_length_842, 60); 
                bitwise_copy_into(d_tmcc_word, d_int_length_842, 100); 
            }
            else if((d_mode == 1 && d_int_length_C == 16 && d_nsegs_C > 0) || 
                    (d_mode == 2 && d_int_length_C == 8 && d_nsegs_C > 0) || 
                    (d_mode == 3 && d_int_length_C == 4 && d_nsegs_C > 0) )
            {
                bitwise_copy_into(d_tmcc_word, d_int_length_1684, 60); 
                bitwise_copy_into(d_tmcc_word, d_int_length_1684, 100); 
            }
            else
            {
                // unused
                bitwise_copy_into(d_tmcc_word, std::bitset<3>(0b111), 60); 
                bitwise_copy_into(d_tmcc_word, std::bitset<3>(0b111), 100); 
            }

            //////////////////////////////////////////////
            //// now the number of segments
            //////////////////////////////////////////////

            if (d_nsegs_A > 0 && d_nsegs_A <= 13) {
                bitwise_copy_into(d_tmcc_word, d_nsegs[d_nsegs_A-1], 37); 
                bitwise_copy_into(d_tmcc_word, d_nsegs[d_nsegs_A-1], 77); 
            }
            else
            {
                // I'll assume it's not used
                bitwise_copy_into(d_tmcc_word, std::bitset<4>(0b1111), 37); 
                bitwise_copy_into(d_tmcc_word, std::bitset<4>(0b1111), 77); 
            }

            if (d_nsegs_B > 0 && d_nsegs_B <= 13) {
                bitwise_copy_into(d_tmcc_word, d_nsegs[d_nsegs_B-1], 50); 
                bitwise_copy_into(d_tmcc_word, d_nsegs[d_nsegs_B-1], 90); 
            }
            else
            {
                // I'll assume it's not used
                bitwise_copy_into(d_tmcc_word, std::bitset<4>(0b1111), 50); 
                bitwise_copy_into(d_tmcc_word, std::bitset<4>(0b1111), 90); 
            }

            if (d_nsegs_C > 0 && d_nsegs_C <= 13) {
                bitwise_copy_into(d_tmcc_word, d_nsegs[d_nsegs_C-1], 63); 
                bitwise_copy_into(d_tmcc_word, d_nsegs[d_nsegs_C-1], 103); 
            }
            else
            {
                // I'll assume it's not used
                bitwise_copy_into(d_tmcc_word, std::bitset<4>(0b1111), 63); 
                bitwise_copy_into(d_tmcc_word, std::bitset<4>(0b1111), 103); 
            }


            ////////////////////////////////////////////////
            //// Phase correction (all in 1s)
            ///////////////////////////////////////////////

            bitwise_copy_into(d_tmcc_word, std::bitset<3>(0b111), 107);
            
            ////////////////////////////////////////////////
            //// Reserved bits (all in 1s)
            ///////////////////////////////////////////////
            bitwise_copy_into(d_tmcc_word, std::bitset<12>(0b111111111111), 110);


        }
        int tmcc_encoder_impl::get_degree(std::bitset<204> r, int max)
        {
            // Given a binary expression of a GF(2) polynomial, returns the degree < max
            int degree = 0;

            while ((r.any()) && (degree < max))
            {
                degree++;
                r >>= 1;
            }

            // If not found, show an appropiated message
            if (degree == max)
            {
                printf("[TMCC encoder] Parity error: degree max = %d\n", degree);
            }
            return (degree - 1);
        }

        std::bitset<82> tmcc_encoder_impl::compute_parity()
        {
            /* Fills the bits 123 to 203 with parity for a given TMCC word */
            std::bitset<204> temp_word, g, m, r, one, aux;
            g.reset();
            aux.reset();
            m.reset();
            r.reset();
            one.reset();
            one.set(0);

            int n = 184; 
            int k = 102; 


            /* Bits B20 to B121 of TMCC information are error-correction coded by means of the shortened code (184,102)
               of the difference cyclic code (273, 191).
               Generating polynomial of the (273, 191) code: 
               g(x) = x^82 + x^77 + x^76 + x^71 + x^67 + x^66 + x^56 + x^52 + x^48 + x^40 + x^36 + x^34 + x^24 + x^22 + x^18 + x^10 + x^4 + 1
               */

            // First, we initialize a variable with the polynomial g(x)
            g = (one << 82) | (one << 77) | (one << 76) | (one << 71) | (one << 67) | (one << 66) | (one << 56) | (one << 52) | (one << 48) | (one << 40) | (one << 36) | (one << 34) | (one << 24) | (one << 22) | (one << 18) | (one << 10) | (one << 4) | (one);
            
            
            int degree_g = this->get_degree(g, 84);
           
            // Load the TMCCword bits to code to the m variable
            for (int i = 20; i<122; i++)
            {
                d_tmcc_word.test(i)?m.set(121 - i):m.reset(121 - i);
                //m.set(121 - i,d_tmcc_word.test(i));
            }


            // Now we should compute the x^(n-k).m(x) polynomial
            m = m << (n - k);

            // Once we have the p(x) and x^(n-k)m(x) the next step is know the difference of degrees
            int degree_m = this->get_degree(m, n - k + 102 +1);
            
            r = (g << (degree_m - degree_g)) ^ m;

            int degree_r = this->get_degree(r, n-k+102);

            while (degree_r >= degree_g)
            {
                r = (g << (degree_r - degree_g)) ^ r;
                degree_r = this->get_degree(r, n-k+102 +1);

            }

            std::bitset<82> parity ; 
            // Here we have r(x), the parity of the message
            // Write the parity on the TMCCword
            for (int j=122; j<204; j++)
            {
                aux = r >> (j-122) & one;
                //aux.test(0)?parity.set(203+122-j):parity.reset(203+122-j);  //TODO: write the indexes in a simpler way
                aux.test(0)?parity.set(203-j):parity.reset(203-j);  //TODO: write the indexes in a simpler way
            }

            return parity;
           // *TMCCword = temp_word;
        }

        void tmcc_encoder_impl::tmcc_carrier_positions()
            {
                /*
                 * Assign to variables ac_carriers, ac_carriers_size, tmcc_carriers and tmcc_carriers_size
                 * the corresponding values according to the transmission mode
                 */
                switch (d_fft_length)
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


        int
            tmcc_encoder_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const gr_complex *in = (const gr_complex *) input_items[0];
                gr_complex *out = (gr_complex *) output_items[0];


                for (int i=0; i < noutput_items; i++){

                    // I first copy the input on the output
                    memcpy(out + i*d_fft_length, in + i*d_fft_length, d_fft_length*sizeof(gr_complex)); 

                    // I now assign the tmcc carriers (PRBS should be set in the previous block)
                    if (d_symbol_count == 0)
                    {
                        // the first bit is the reference, and thus differential encoding is not used
                        // I also have to invert the sync word
                        d_tmcc_sync_word.flip();
                        bitwise_copy_into(d_tmcc_word, d_tmcc_sync_word, 1);
                        d_last_bit_sent = d_tmcc_word.test(0);
                    }
                    else
                    {
                        // I apply differential encoding
                        d_last_bit_sent = d_last_bit_sent^d_tmcc_word.test(d_symbol_count);
                    }
                    d_tmcc_pilot = gr_complex(d_last_bit_sent ? 1 : -1, 0);
                    
                    for (int current_tmcc_carrier = 0; current_tmcc_carrier < d_tmcc_carriers_size; current_tmcc_carrier++) {
                        out[i*d_fft_length+d_zeros_on_left+d_tmcc_carriers[current_tmcc_carrier]] = in[i*d_fft_length+d_zeros_on_left + d_tmcc_carriers[current_tmcc_carrier]]*d_tmcc_pilot;
                    }

                    d_symbol_count = (d_symbol_count + 1) % d_tmcc_word.size();
                }

                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

