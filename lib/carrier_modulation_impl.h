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

#ifndef INCLUDED_ISDBT_CARRIER_MODULATION_IMPL_H
#define INCLUDED_ISDBT_CARRIER_MODULATION_IMPL_H

#include <gnuradio/isdbt/carrier_modulation.h>
#include <boost/circular_buffer.hpp>

namespace gr {
    namespace isdbt {

        class carrier_modulation_impl : public carrier_modulation
        {
            private:
                static const int d_data_carriers_mode1; 

                //all possible symbol delays and the associated constellations
                static const int d_delay_qpsk[]; 
                static const gr_complex d_symbols_qpsk[]; 
                static const int d_delay_16qam[]; 
                static const gr_complex d_symbols_16qam[]; 
                static const int d_delay_64qam[]; 
                static const gr_complex d_symbols_64qam[]; 

                //the current delay and constellation
                int * d_delay;
                gr_complex * d_symbols;

                int d_mode; 
                int d_const_size;
                int d_num_bits;
                int d_nsegments;
                int d_carriers_per_segment; 
                int d_total_carriers;

                //Here are the symbols. To create an output I have to check
                //several symbols back. Where and how many depends on the 
                //constellation size. 
                //std::vector<std::deque<unsigned char> *> d_shift; 
                boost::circular_buffer<unsigned char> * d_shift; 

                // compute so-called delay-adjustement per table 3.9 in the standard
                int compute_extra_delay(int mode, int const_size, int nsegments);

            public:
                carrier_modulation_impl(int mode, int segments, int constellation_size);
                ~carrier_modulation_impl();

                // Where all the action really happens
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
        };

    } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_CARRIER_MODULATION_IMPL_H */


