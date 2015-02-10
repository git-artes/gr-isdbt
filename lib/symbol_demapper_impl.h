/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
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
                static const float d_possible_gains[]; 

                int d_mode; 
                int d_const_size; 
                int d_gain; 
                gr_complex d_constellation[]; 
                int d_carriers_per_segment; 
                int d_noutput; 
                
                /**
                 * It puts the complex numbers corresponding to the 
                 * constellation in the order established by the standard
                 * so that for example d_constellation[0] corresponds to 
                 * 1/sqrt(2)+j/sqrt(2) in QPSK
                 * */
                void make_constellation(int size); 
                int bin_to_gray(int val); 
                int find_constellation_value(gr_complex val); 
            public:
                symbol_demapper_impl(int mode, int constellation_size);
                ~symbol_demapper_impl();

                // Where all the action really happens
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
        };

    } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_SYMBOL_DEMAPPER_IMPL_H */

