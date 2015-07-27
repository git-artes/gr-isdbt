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


#ifndef INCLUDED_ISDBT_SYMBOL_DEMAPPER_IMPL_H
#define INCLUDED_ISDBT_SYMBOL_DEMAPPER_IMPL_H

#include <isdbt/symbol_demapper.h>

namespace gr {
    namespace isdbt {

        class symbol_demapper_impl : public symbol_demapper
        {
            private:

                static const int d_data_carriers_mode1; 
                static const int d_total_segments; 

                static const float d_th_16qam; 
                static const float d_th_64qam; 

                int d_mode; 
                int d_const_size_A;
			    int d_const_size_B;
				int d_const_size_C;
				int d_nsegments_A;
				int d_nsegments_B;
				int d_nsegments_C;	

                //Pointers to the functions that finds the bit-word given the complex symbol. 
                unsigned int (symbol_demapper_impl::*find_constellation_value_lA)(gr_complex val); 
                unsigned int (symbol_demapper_impl::*find_constellation_value_lB)(gr_complex val); 
                unsigned int (symbol_demapper_impl::*find_constellation_value_lC)(gr_complex val); 

                int d_carriers_per_segment; 
                int d_noutput; 
                int d_noutput_A; 
                int d_noutput_B; 
                int d_noutput_C; 
                
                /*!
                 * \brief Given an incoming complex symbol, it outputs the corresponding bit-word assuming a 
                 * QPSK modulation. 
                 *
                 * Taking advantage of the Grey's coding used in the standard, this method is relatively fast. 
                 */
                unsigned int find_constellation_value_qpsk(gr_complex val);
            
                /*!
                 * \brief Given an incoming complex symbol, it outputs the corresponding bit-word assuming a 
                 * 16QAM modulation. 
                 *
                 * Taking advantage of the Grey's coding used in the standard, this method is relatively fast. 
                 */
				unsigned int find_constellation_value_16qam(gr_complex val);

				 /*!
                 * \brief Given an incoming complex symbol, it outputs the corresponding bit-word assuming a 
                 * 64QAM modulation. 
                 *
                 * Taking advantage of the Grey's coding used in the standard, this method is relatively fast. 
                 */
                unsigned int find_constellation_value_64qam(gr_complex val);

            public:
                symbol_demapper_impl(int mode, int segments_A, int constellation_size_A, int segments_B, int constellation_size_B, int segments_C, int constellation_size_C);
                ~symbol_demapper_impl();

                // Where all the action really happens
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
        };

    } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_SYMBOL_DEMAPPER_IMPL_H */

