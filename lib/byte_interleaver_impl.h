/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017, 2018
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

#ifndef INCLUDED_ISDBT_BYTE_INTERLEAVER_IMPL_H
#define INCLUDED_ISDBT_BYTE_INTERLEAVER_IMPL_H

#include <isdbt/byte_interleaver.h>
#include <boost/circular_buffer.hpp>

namespace gr {
    namespace isdbt {

        class byte_interleaver_impl : public byte_interleaver
        {
            private:

                static const int d_SYNC;
                static const int d_TSP_SIZE; 
                static const int d_I;
                static const int d_M;
                static const int d_data_carriers_mode1;

                // required to calculate the extra-delay
                //mode
                int d_mode;
                //constellation size
                int d_const_size; 
                // convolutional code rate
                float d_rate; 
                //number of segments in this layer
                int d_nsegments;

                std::vector< boost::circular_buffer<unsigned char> * > d_shift;

                // computes the delay-adjustment as per table 3-7 in the standard
                int compute_extra_delay(int mode, int const_size, float rate, int nsegments);

            public:
                byte_interleaver_impl(int mode, int constellation_size, int rate, int nsegments);
                ~byte_interleaver_impl();

                // Where all the action really happens
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
        };

    } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_BYTE_INTERLEAVER_IMPL_H */

