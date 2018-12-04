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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "byte_interleaver_impl.h"

namespace gr {
    namespace isdbt {


        const int byte_interleaver_impl::d_SYNC = 0x47;
        const int byte_interleaver_impl::d_TSP_SIZE = 204; 
        const int byte_interleaver_impl::d_I = 12; 
        const int byte_interleaver_impl::d_M = 17; 
        const int byte_interleaver_impl::d_data_carriers_mode1 = 96; 

        byte_interleaver::sptr
            byte_interleaver::make(int mode, int constellation_size, int rate, int nsegments)
            {
                return gnuradio::get_initial_sptr
                    (new byte_interleaver_impl(mode, constellation_size, rate, nsegments));
            }

        /*
         * The private constructor
         */
        byte_interleaver_impl::byte_interleaver_impl(int mode, int constellation_size, int rate, int nsegments)
            : gr::sync_interpolator("byte_interleaver",
                    gr::io_signature::make(1, 1, d_TSP_SIZE*sizeof(unsigned char)),
                    gr::io_signature::make(1, 1, sizeof(unsigned char)), d_TSP_SIZE)
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

            int extra_delay = compute_extra_delay(d_mode, d_const_size, d_rate, d_nsegments);
            // the norm's delay is in TSP. Since I'll add this delay to all buffers (which are 
            // in bytes), I multiply it by TSP_SIZE and divide by the total buffers. 
            extra_delay = extra_delay*d_TSP_SIZE/d_I;
            // The "difficult" part in any deinterleaver is setting the buffer's size
            for (int i=0; i<d_I; i++)
            {
                d_shift.push_back(new boost::circular_buffer<unsigned char>(extra_delay + 1 + d_M*i, 0)); 
            }

        }

        int byte_interleaver_impl::compute_extra_delay(int mode, int const_size, float rate, int nsegments)
        {

            int num_bits = log2(const_size);

            int extra_delay = (nsegments * num_bits*rate*d_data_carriers_mode1*((int)pow(2.0,mode-1)))/8 - 11;

            return extra_delay; 
        }

        /*
         * Our virtual destructor.
         */
        byte_interleaver_impl::~byte_interleaver_impl()
        {
            for (unsigned int i = 0; i < d_shift.size(); i++)
            {
                delete d_shift.back();
                d_shift.pop_back();
            }
        }

        int
            byte_interleaver_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const unsigned char *in = (const unsigned char *) input_items[0];
                unsigned char *out = (unsigned char *) output_items[0];

                int index_out = 0; 
                for (int i = 0; i < noutput_items; i++)
                {
                        d_shift[i % d_I]->push_back(in[i]);
                        out[i] = d_shift[i % d_I]->front();
                }

                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

