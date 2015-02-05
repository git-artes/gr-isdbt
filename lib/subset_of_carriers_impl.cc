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

#include <gnuradio/io_signature.h>
#include "subset_of_carriers_impl.h"

namespace gr {
  namespace isdbt {

    subset_of_carriers::sptr
    subset_of_carriers::make(int total, int first, int last)
    {
      return gnuradio::get_initial_sptr
        (new subset_of_carriers_impl(total, first, last));
    }

    /*
     * The private constructor
     */
    subset_of_carriers_impl::subset_of_carriers_impl(int total, int first, int last)
      : gr::sync_block("subset_of_carriers",
              gr::io_signature::make(1, 1, sizeof(gr_complex)*total),
              gr::io_signature::make(1, 1, sizeof(gr_complex)*(last-first+1)))
    {
        d_noutput = last-first+1; 
        d_total = total; 
        d_first = first; 
        d_last = last; 
    }

    /*
     * Our virtual destructor.
     */
    subset_of_carriers_impl::~subset_of_carriers_impl()
    {
    }

    int
    subset_of_carriers_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];

        // Do <+signal processing+>

        for (int i=0; i<noutput_items; i++)
        {
            for(int carrier = 0; carrier < d_noutput; carrier++)
            {
                out[i*d_noutput+carrier] = in[i*d_total+d_first+carrier]; 
            }
        }

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace isdbt */
} /* namespace gr */

