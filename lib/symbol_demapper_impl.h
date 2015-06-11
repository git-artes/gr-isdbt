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

                static const gr_complex d_constellation_qpsk[]; 
                static const gr_complex d_constellation_16qam[]; 
                static const gr_complex d_constellation_64qam[]; 

                int d_mode; 
                int d_const_size_A;
			    int d_const_size_B;
				int d_const_size_C;
				int d_nsegments_A;
				int d_nsegments_B;
				int d_nsegments_C;	
                const gr_complex * d_constellation_A;
			    const gr_complex * d_constellation_B;
				const gr_complex * d_constellation_C;	
                int d_carriers_per_segment; 
                int d_noutput; 
                int d_noutput_A; 
                int d_noutput_B; 
                int d_noutput_C; 
                
                int find_constellation_value_A(gr_complex val);
			    int	find_constellation_value_B(gr_complex val);
				int find_constellation_value_C(gr_complex val);
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

