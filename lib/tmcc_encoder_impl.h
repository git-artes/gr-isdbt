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

#ifndef INCLUDED_ISDBT_TMCC_ENCODER_IMPL_H
#define INCLUDED_ISDBT_TMCC_ENCODER_IMPL_H

#include <gnuradio/isdbt/tmcc_encoder.h>
#include <bitset>

namespace gr {
  namespace isdbt {

        class tmcc_encoder_impl : public tmcc_encoder
        {
            private:

                // Number of segments
                static const int d_total_segments; 
                // Total number of active carriers per segment in 2k mode (i'll calculate it for the corresponding mode later)
                static const int d_carriers_per_segment_2k; 

                static const std::bitset<3> d_mod_scheme_qpsk;
                static const std::bitset<3> d_mod_scheme_16qam;
                static const std::bitset<3> d_mod_scheme_64qam;
                static const std::bitset<3> d_mod_scheme_unused;

                static const std::bitset<3> d_conv_code_12;
                static const std::bitset<3> d_conv_code_23;
                static const std::bitset<3> d_conv_code_34;
                static const std::bitset<3> d_conv_code_56;
                static const std::bitset<3> d_conv_code_78;
                static const std::bitset<3> d_conv_code_unused;
                
                static const std::bitset<4> d_nsegs[];

                static const std::bitset<3> d_int_length_0;
                static const std::bitset<3> d_int_length_421;
                static const std::bitset<3> d_int_length_842;
                static const std::bitset<3> d_int_length_1684;

                // Number of carriers per segment
                int d_carriers_per_segment;

                int d_fft_length;
                int d_active_carriers;
                int d_mode;

                // the configuration for which I'll calculate the TMCC word
                int d_one_seg_present;
                int d_mod_scheme_A; 
                int d_mod_scheme_B; 
                int d_mod_scheme_C; 
                int d_conv_code_A; 
                int d_conv_code_B; 
                int d_conv_code_C; 
                int d_int_length_A; 
                int d_int_length_B; 
                int d_int_length_C; 
                int d_nsegs_A; 
                int d_nsegs_B; 
                int d_nsegs_C; 

                // the actual tmcc word 
                std::bitset<16> d_tmcc_sync_word;
                std::bitset<204> d_tmcc_word;

                int d_zeros_on_left;
                // how many symbols where sent (between 0 and 203, and thus knowing what bit of the tmcc should be sent)
                int d_symbol_count;
                // the actual complex representation of the tmcc pilot
                gr_complex d_tmcc_pilot;
                // the last sent bit
                bool d_last_bit_sent;

                int d_tmcc_carriers_size;
                const int * d_tmcc_carriers; 
                static const int tmcc_carriers_size_2k;
                static const int tmcc_carriers_2k[];
                static const int tmcc_carriers_size_4k;
                static const int tmcc_carriers_4k[];
                static const int tmcc_carriers_size_8k;
                static const int tmcc_carriers_8k[];


                // given the current config, calculate and assign the corresponding TMCC word
                void calculate_tmcc_word();

                // An auxiliary function that copies slices of bitsets
                template<size_t N1, size_t N2>
                    void bitwise_copy_into(std::bitset<N1>& b_destination, const std::bitset<N2>& b_origin, size_t pos);

                // given the current TMCC word, it computes the corresponding parity
                std::bitset<82> compute_parity();

                // given the current mode, it calculates the tmcc carrier positions
                void tmcc_carrier_positions();
            
                // Given a binary expression of a GF(2) polynomial, returns the degree < max
                int get_degree(std::bitset<204> r, int max);

            public:
                tmcc_encoder_impl(int mode, bool one_seg_present, int mod_scheme_A, int mod_scheme_B, int mod_scheme_C, int conv_code_A, int conv_code_B, int conv_code_C, int int_length_A, int int_length_B, int int_length_C, int nsegs_A, int nsegs_B, int nsegs_C);
                ~tmcc_encoder_impl();

                // Where all the action really happens
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
        };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_TMCC_ENCODER_IMPL_H */

