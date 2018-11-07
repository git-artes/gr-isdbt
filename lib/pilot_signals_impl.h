/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017, 2018
 *   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
 *   Santiago Castro
 *   Javier Hernandez
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

#ifndef INCLUDED_ISDBT_PILOT_SIGNALS_IMPL_H
#define INCLUDED_ISDBT_PILOT_SIGNALS_IMPL_H

#include <isdbt/pilot_signals.h>

namespace gr {
    namespace isdbt {

        class pilot_signals_impl : public pilot_signals
        {
            private:
                static const int d_total_segments; 
                static const int d_carriers_per_segment_2k; 
                static const int d_data_carriers_per_segment_2k;

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
                static const int ac_carriers_2k[];
                static const int ac_carriers_size_2k;
                static const int ac_carriers_4k[];
                static const int ac_carriers_size_4k;
                static const int ac_carriers_8k[];
                static const int ac_carriers_size_8k;


                int d_fft_length;
                int d_active_carriers; 
                int d_data_carriers_size;
                int * d_data_carriers;
                
                int d_zeros_on_left;
                
                gr_complex * d_pilot_values; 
                int d_sp_carriers_size;

                /*! 
                 * The current symbol (0, 1, 2 or 3). 
                 */
                int d_current_symbol;

                /*!
                 * This method constructs the pseudo-random sequence wk used by the standard. It also allocates the
                 * corresponding values to the pilot values array. 
                 */
                void generate_prbs();

                /*!
                 * Given a fft_size (i.e. a mode), this method assigns an array where the position 
                 * of the tmcc pilots and auxiliary channels are indicated. 
                 */
                void tmcc_and_ac_positions(int fft);

                /*! 
                 * Given a fft_size (i.e. a mode), this method assigns an array where the position
                 * of each data carrier is indicated. 
                 */
                void data_carriers_position();

            public:
                pilot_signals_impl(int mode);
                ~pilot_signals_impl();

                // Where all the action really happens
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
        };

    } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_PILOT_SIGNALS_IMPL_H */

