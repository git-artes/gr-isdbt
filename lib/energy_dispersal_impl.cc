/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "energy_dispersal_impl.h"

namespace gr {
    namespace isdbt {

        const int energy_dispersal_impl::d_TSP_SIZE = 204;
        const int energy_dispersal_impl::d_SYNC = 0x47;
        const int energy_dispersal_impl::d_data_carriers_mode1 = 96; 

        void
            energy_dispersal_impl::init_prbs()
            {
                d_reg = 0xa9;
            }

        int
            energy_dispersal_impl::clock_prbs(int clocks)
            {
                int res = 0;
                int feedback = 0;

                for (int i = 0; i < clocks; i++) {
                    feedback = ((d_reg >> (14 - 1)) ^ (d_reg >> (15 - 1))) & 0x1;
                    d_reg = ((d_reg << 1) | feedback) & 0x7fff;
                    res = (res << 1) | feedback;
                }
                return res;
            }


        energy_dispersal::sptr
            energy_dispersal::make(int mode, int constellation_size, int rate, int nsegments)
            {
                return gnuradio::get_initial_sptr
                    (new energy_dispersal_impl(mode, constellation_size, rate, nsegments));
            }

        /*
         * The private constructor
         */
        energy_dispersal_impl::energy_dispersal_impl(int mode, int constellation_size, int rate, int nsegments)
            : gr::sync_block("energy_dispersal",
                    gr::io_signature::make(1, 1, d_TSP_SIZE*sizeof(unsigned char)),
                    gr::io_signature::make(1, 1, d_TSP_SIZE*sizeof(unsigned char))
                    )
        {

            d_mode = mode; 
            d_const_size = constellation_size; 
            switch (rate){
                case 0:
                    d_rate = 0.5; 
                    break;
                case 1:
                    d_rate = (float) 2/3;
                    break;
                case 2:
                    d_rate = (float) 3/4;
                    break;
                case 3:
                    d_rate = (float) 5/6;
                    break;
                case 4:
                    d_rate = (float) 7/8;
                    break;
            }

            d_nsegments = nsegments; 

            // I compute the number of TSP per OFDM frame, after which period I have to reset the PRBS
            d_tsp_per_frame = compute_tsp_per_ofdm_frame(d_mode, d_const_size, d_rate, d_nsegments);
            
            init_prbs();
            d_tsp_encoded = 0;

        }

        /*
         * Our virtual destructor.
         */
        energy_dispersal_impl::~energy_dispersal_impl()
        {
        }

        int energy_dispersal_impl::compute_tsp_per_ofdm_frame(int mode, int const_size, float rate, int nsegments)
        {

            int num_bits = log2(const_size);

            int tsp_per_ofdm_frame = (nsegments * num_bits*rate*d_data_carriers_mode1*((int)pow(2.0,mode-1)))/8;

            return tsp_per_ofdm_frame; 
        }


        int
            energy_dispersal_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const unsigned char *in = (const unsigned char *) input_items[0];
                unsigned char *out = (unsigned char *) output_items[0];


                for (int i = 0; i < noutput_items; i++)
                {
                    if (d_tsp_encoded % d_tsp_per_frame == 0)
                    {
                        //Reset PRBS 
                        //printf("symbols_coded: %i\n", symbols_coded);
                        //printf("symbols_coded mod 204: %i\n", symbols_coded % 204);
                        init_prbs();
                        d_tsp_encoded = 0;
                    }
                    //XOR every bit from 0 to 202
                    for (int j = 0; j < d_TSP_SIZE - 1; j++)
                    {
                        out[i*d_TSP_SIZE + j] = in[i*d_TSP_SIZE + j] ^ clock_prbs(8); 
                    }
                    //Byte 203 should be sync. Don't do XOR, copy it
                    out[i*d_TSP_SIZE + 203] = in[i*d_TSP_SIZE + 203];

                    clock_prbs(8); // Consume 1 clock_prbs
                    d_tsp_encoded++;
                }


                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

