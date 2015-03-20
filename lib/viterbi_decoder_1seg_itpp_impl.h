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

#ifndef INCLUDED_ISDBT_VITERBI_DECODER_1SEG_ITPP_IMPL_H
#define INCLUDED_ISDBT_VITERBI_DECODER_1SEG_ITPP_IMPL_H

#include <isdbt/viterbi_decoder_1seg_itpp.h>

#include <itpp/itcomm.h>

namespace gr {
    namespace isdbt {

        class viterbi_decoder_1seg_itpp_impl : public viterbi_decoder_1seg_itpp
        {
            private:
                // The it++ class that will decode the incoming signal. 
               itpp::Punctured_Convolutional_Code d_code;
                // Block size
                int d_bsize;
                // the number of bits per symbol
                int d_m; 

            public:
                viterbi_decoder_1seg_itpp_impl();
                ~viterbi_decoder_1seg_itpp_impl();

                // Where all the action really happens
                void forecast (int noutput_items, gr_vector_int &ninput_items_required);

                int general_work(int noutput_items,
                        gr_vector_int &ninput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
        };

    } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_VITERBI_DECODER_1SEG_ITPP_IMPL_H */

