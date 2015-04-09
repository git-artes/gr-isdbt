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

#include <stdio.h>

#include <gnuradio/io_signature.h>
#include "energy_descrambler_impl.h"

namespace gr {
    namespace isdbt {

        const int energy_descrambler_impl::d_TSP_SIZE = 204;
        const int energy_descrambler_impl::d_SYNC = 0x47;

        void
            energy_descrambler_impl::init_prbs()
            {
                d_reg = 0xa9;
            }

        int
            energy_descrambler_impl::clock_prbs(int clocks)
            {
                int res = 0;
                int feedback = 0;

                for(int i = 0; i < clocks; i++)
                {
                    feedback = ((d_reg >> (14 - 1)) ^ (d_reg >> (15 - 1))) & 0x1;
                    d_reg = ((d_reg << 1) | feedback) & 0x7fff;

                    res = (res << 1) | feedback;
                }

                return res;
            }

        energy_descrambler::sptr
            energy_descrambler::make()
            {
                return gnuradio::get_initial_sptr
                    (new energy_descrambler_impl());
            }

        /*
         * The private constructor
         */
        energy_descrambler_impl::energy_descrambler_impl() : gr::block("energy_descrambler",
                gr::io_signature::make(1, 1, sizeof(unsigned char)*d_TSP_SIZE),
                gr::io_signature::make(1, 1, sizeof(unsigned char)*d_TSP_SIZE))
        {
            set_relative_rate(1.0); 

        }


        /*
         * Our virtual destructor.
         */
        energy_descrambler_impl::~energy_descrambler_impl()
        {
        }

        void
            energy_descrambler_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
            {
                //ninput_items_required[0] = 4 * (noutput_items / (d_nblocks * d_bsize));
                ninput_items_required[0] = noutput_items;
            }

        int
            energy_descrambler_impl::general_work (int noutput_items,
                    gr_vector_int &ninput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const unsigned char *in = (const unsigned char *) input_items[0];
                unsigned char *out = (unsigned char *) output_items[0];

                int to_consume, to_out;

                //PRINTF("ENERGY: noutput_items: %i, d_index: %i\n", noutput_items, d_index);

                // Search for frame_begin, when the PRBS should be restarted. 
                std::vector<tag_t> tags;
                const uint64_t nread = this->nitems_read(0); //number of items read on port 0
                this->get_tags_in_range(tags, 0, nread, nread + noutput_items, pmt::string_to_symbol("frame_begin"));
                //printf("tags.size(): %i \n", tags.size()) ; 

                if (tags.size())
                {
                    //printf("ENERGY DESCRAMBLER: %i \n", pmt::symbol_to_string(tags[0].value)) ; 
                    std::cout << "ENERGY DESCRAMBLER: "<< tags[0].value <<std::endl ; 
                    init_prbs();
                    if (tags[0].offset - nread)
                    {
                        consume_each(tags[0].offset - nread);
                        return (0);
                    }
                }


                for (int i = 0; i < noutput_items; i++)
                {
                    
                    // The sync byte should be taken from the end and put in the beginning. 
                    //out[i*d_TSP_SIZE] = d_SYNC; 
                    out[i*d_TSP_SIZE] = in[d_TSP_SIZE-1 + i*d_TSP_SIZE];
                    for (int byte = 0; byte < d_TSP_SIZE-1; byte++)
                    {
                        out[byte + i*d_TSP_SIZE+1] = in[byte + i*d_TSP_SIZE] ^ clock_prbs(8);
                        //out[byte + i*d_TSP_SIZE] = in[byte + i*d_TSP_SIZE] ^ clock_prbs(8);
                    }
                    // For subsequent blocks PRBS is clocked also on SYNC
                    // but its output is not used
                    clock_prbs(8);
                    // d_SYNC should be in this position, but I would like to verify it. 
                    //out[d_TSP_SIZE-1 + i*d_TSP_SIZE] = in[d_TSP_SIZE-1 + i*d_TSP_SIZE];
                    
                    printf("DESCRAMBLER: in[%i]=%x\n", d_TSP_SIZE-1 + i*d_TSP_SIZE, in[d_TSP_SIZE-1 + i*d_TSP_SIZE]);
                }


                // Tell runtime how many input items we consumed
                consume_each(noutput_items);

                // Tell runtime system how many output items we produced.
                return (noutput_items);
            }

    } /* namespace isdbt */
} /* namespace gr */

