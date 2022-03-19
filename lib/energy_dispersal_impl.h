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

#ifndef INCLUDED_ISDBT_ENERGY_DISPERSAL_IMPL_H
#define INCLUDED_ISDBT_ENERGY_DISPERSAL_IMPL_H

#include <gnuradio/isdbt/energy_dispersal.h>

namespace gr {
    namespace isdbt {

        class energy_dispersal_impl : public energy_dispersal
        {
            private:
                static const int d_data_carriers_mode1;
                // Packet size
                static const int d_TSP_SIZE;
                // SYNC value
                static const int d_SYNC;
                // required to calculate when to re-start the PRBS
                //mode
                int d_mode;
                //constellation size
                int d_const_size; 
                // convolutional code rate
                float d_rate; 
                //number of segments in this layer
                int d_nsegments;


                // Register for PRBS
                int d_reg;

                int d_tsp_encoded;

                int d_tsp_per_frame; //Modo 2, TC 1/8 = 2304

                void init_prbs();
                int clock_prbs(int clocks);

                int compute_tsp_per_ofdm_frame(int mode, int const_size, float rate, int nsegments);

            public:
                energy_dispersal_impl(int mode, int constellation_size, int rate, int nsegments);
                ~energy_dispersal_impl();


                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
        };

    } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_ENERGY_DISPERSAL_IMPL_H */


