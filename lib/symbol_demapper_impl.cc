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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>

#include <gnuradio/io_signature.h>
#include "symbol_demapper_impl.h"

namespace gr {
    namespace isdbt {

        // TODO shouldn't these go somewhere else??
        const int symbol_demapper_impl::d_data_carriers_mode1 = 96; 
        const int symbol_demapper_impl::d_total_segments = 13; 

        const float symbol_demapper_impl::d_possible_gains[] = {1.0/sqrt(2), 1.0/sqrt(10), 1.0/sqrt(42)}; 

        symbol_demapper::sptr
            symbol_demapper::make(int mode, int constellation_size)
            {
                return gnuradio::get_initial_sptr
                    (new symbol_demapper_impl(mode, constellation_size));
            }

        /*
         * The private constructor
         */
        symbol_demapper_impl::symbol_demapper_impl(int mode, int constellation_size)
            : gr::sync_block("symbol_demapper",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*d_total_segments*d_data_carriers_mode1*((int)pow(2.0,mode-1))),
                    gr::io_signature::make(1, 1, sizeof(unsigned char)*d_total_segments*d_data_carriers_mode1*((int)pow(2.0,mode-1))))
        {
            d_mode = mode; 
            d_const_size = constellation_size; 
            if (d_const_size==4){
                d_gain = d_possible_gains[0]; 
            }else if (d_const_size==16){
                d_gain = d_possible_gains[1]; 
            }else {
                d_gain = d_possible_gains[2]; 
            }

            d_carriers_per_segment = d_data_carriers_mode1*((int)pow(2.0,mode-1)); 
            d_noutput = d_total_segments*d_carriers_per_segment; 

            make_constellation(d_const_size);
        }

        /*
         * Our virtual destructor.
         */
        symbol_demapper_impl::~symbol_demapper_impl()
        {
        }

        void symbol_demapper_impl::make_constellation(int size){
            // Since constellations are the same as in DVB-T (without alpha and step=2)
            // we use Bogdan Diaconescu's implementation. 

            //TODO - verify if QPSK works

            // The simetry of the constellation is used to calculate 
            // 16-QAM from QPSK and 64-QAM form 16-QAM

            int bits_per_axis = log2(size) / 2;
            int steps_per_axis = sqrt(size) / 2 - 1;

            for (int i = 0; i < size; i++)
            {
                // This is the quadrant made of the first two bits starting from MSB
                int q = i >> (2 * (bits_per_axis - 1)) & 3;
                // Sign for correctly calculate I and Q in each quadrant
                int sign0 = (q >> 1) ? -1 : 1; int sign1 = (q & 1) ? -1 : 1;

                int x = (i >> (bits_per_axis - 1)) & ((1 << (bits_per_axis - 1)) - 1);
                int y = i & ((1 << (bits_per_axis - 1)) - 1);

                int xval = (steps_per_axis - x) * 2;
                int yval = (steps_per_axis - y) * 2;

                int val = (bin_to_gray(x) << (bits_per_axis - 1)) + bin_to_gray(y);

                // ETSI EN 300 744 Clause 4.3.5
                // Actually the constellation is gray coded
                // but the bits on each axis are not taken in consecutive order
                // So we need to convert from b0b2b4b1b3b5->b0b1b2b3b4b5(QAM64)

                x = 0; y = 0;

                for (int j = 0; j < (bits_per_axis - 1); j++)
                {
                    x += ((val >> (1 + 2 * j)) & 1) << j;
                    y += ((val >> (2 * j)) & 1) << j;
                }

                val = (q << 2 * (bits_per_axis - 1)) + (x << (bits_per_axis - 1)) + y;

                // Keep corespondence symbol bits->complex symbol in one vector
                // Norm the signal using gain
                d_constellation[val] = gr_complex(d_gain,0) * gr_complex(sign0 * xval, sign1 * yval);
            }
        }

        int
            symbol_demapper_impl::bin_to_gray(int val)
            {
                return (val >> 1) ^ val;
            }

        int 
            symbol_demapper_impl::find_constellation_value(gr_complex val)
            {
                float min_dist = std::norm(val - d_constellation[0]);
                int min_index = 0;

                for (int i = 0; i < d_const_size; i++)
                {
                    float dist = std::norm(val - d_constellation[i]);

                    if (dist < min_dist)
                    {
                        min_dist = dist;
                        min_index = i;
                    }
                }

                return min_index;
            }



        int
            symbol_demapper_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const gr_complex *in = (const gr_complex *) input_items[0];
                unsigned char *out = (unsigned char *) output_items[0];

                for (int i = 0; i < noutput_items; i++)
                {
                    for (int carrier = 0; carrier<d_noutput; carrier++){
                        // I will for now leave it like this so as to more easily accomodate the 
                        // per-segment constellation possibility later on. 
                        out[i*d_noutput+carrier] = find_constellation_value(in[i*d_noutput+carrier]);
                    }
                }

                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

