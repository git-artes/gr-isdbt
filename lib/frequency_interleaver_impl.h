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

#ifndef INCLUDED_ISDBT_FREQUENCY_INTERLEAVER_IMPL_H
#define INCLUDED_ISDBT_FREQUENCY_INTERLEAVER_IMPL_H

#include <gnuradio/isdbt/frequency_interleaver.h>

namespace gr {
    namespace isdbt {

        class frequency_interleaver_impl : public frequency_interleaver
        {
            private:
                int d_mode; 
                bool d_1seg; 
                int d_carriers_per_segment; 
                int d_total_carriers;

                gr_complex * d_interleaved; 
                gr_complex * d_rotated; 

                static const int d_data_carriers_mode1; 
                static const int d_total_segments; 

                static const int d_random_perm_mode1[]; 
                static const int d_random_perm_mode2[]; 
                static const int d_random_perm_mode3[]; 

                /*The current random permutation matrix*/
                const int * d_random_perm; 

                /*!
                 * It simply randomizes the carriers of each segment as specified by the standard. More 
                 * details are commented in the cc code. 
                 */
                gr_complex * randomize(const gr_complex * random, gr_complex * not_random); 

                /*!
                 * It simply rotates the carriers of each segment as specified by the standard. More 
                 * details are commented in the cc code. 

*/
                gr_complex * rotate(const gr_complex * rotated, gr_complex * derotated); 

                /*!
                 * It performs the inter-segment interleaving as specified in the standard. More 
                 * details are commented in the cc code. 
                 TODO It currently 
                 * does not support differential modulation schemes, which would require a separate interleaving. 
                 */
                gr_complex * intersegment_interleave(const gr_complex * interleaved, gr_complex * deinterleaved); 



            public:
                frequency_interleaver_impl(bool oneseg, int mode);
                ~frequency_interleaver_impl();

                // Where all the action really happens
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
        };

    } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_FREQUENCY_INTERLEAVER_IMPL_H */


