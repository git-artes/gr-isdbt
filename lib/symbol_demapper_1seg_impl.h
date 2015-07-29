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

#ifndef INCLUDED_ISDBT_SYMBOL_DEMAPPER_1SEG_IMPL_H
#define INCLUDED_ISDBT_SYMBOL_DEMAPPER_1SEG_IMPL_H

#include <isdbt/symbol_demapper_1seg.h>

namespace gr {
    namespace isdbt {

        class symbol_demapper_1seg_impl : public symbol_demapper_1seg
        {
            private:

                static const int d_data_carriers_mode1; 
                static const int d_total_segments; 

                static const gr_complex d_constellation_qpsk[]; 
                static const gr_complex d_constellation_16qam[]; 
                static const gr_complex d_constellation_64qam[]; 

                int d_mode; 
                int d_const_size; 
                const gr_complex * d_constellation; 
                int d_carriers_per_segment; 
                int d_noutput; 

                /*!
                 * TODO This function is currently not optimized and is somewhat slow. 
                 * Copy the implementation of the full-seg version. 
                 */
                int find_constellation_value(gr_complex val); 
            public:
                symbol_demapper_1seg_impl(int mode, int constellation_size);
                ~symbol_demapper_1seg_impl();

                // Where all the action really happens
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
        };

    } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_SYMBOL_DEMAPPER_1SEG_IMPL_H */

