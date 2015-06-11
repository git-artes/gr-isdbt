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

#ifndef INCLUDED_ISDBT_VITERBI_DECODER_IMPL_H
#define INCLUDED_ISDBT_VITERBI_DECODER_IMPL_H

#include <isdbt/viterbi_decoder.h>

extern "C" {
#include "d_viterbi.h"
}

namespace gr {
  namespace isdbt {

    class viterbi_decoder_impl : public viterbi_decoder
    {
     private:
      // Puncturing vectors
      static const unsigned char d_puncture_1_2[];
      static const unsigned char d_puncture_2_3[];
      static const unsigned char d_puncture_3_4[];
      static const unsigned char d_puncture_5_6[];
      static const unsigned char d_puncture_7_8[];

      // Current puncturing vector
      const unsigned char * d_puncture;


      // Code rate k/n
      int d_k;
      int d_n;
      // Constellation with m
      int d_m;

      // Block size
      int d_bsize;
      // Symbols to consume on decoding from one block
      int d_nsymbols;
      // Number of bits after depuncturing a block
      int d_nbits;
      // Number of full packed out bytes
      int d_nout;

      // Initial state
      int d_S0;
      // Final state
      int d_SK;

      // Traceback (in bytes)
      int d_ntraceback;

      // Viterbi decoder pointer
      void *d_vp;

      // Viterbi tables
      struct viterbi_state state0[64];
      struct viterbi_state state1[64];
      int mettab[2][256];

      // Buffer to keep the input bits
      unsigned char * d_inbits;

      // This is used to get rid of traceback on the first frame
      int d_init;

     public:
      viterbi_decoder_impl(int constellation_size, int rate);
      ~viterbi_decoder_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_VITERBI_DECODER_IMPL_H */

