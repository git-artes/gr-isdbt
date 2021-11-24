/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 * Copyright 2021 Federico 'Larroca' La Rocca (small changes)
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

#ifdef DTV_SSE2
#include <xmmintrin.h>
#endif

/* The two generator polynomials for the NASA Standard K=7 code.
 * Since these polynomials are known to be optimal for this constraint
 * length there is not much point in changing them.
 */

#define POLYA 0x4f
#define POLYB 0x6d
// Maximum number of traceback bytes
#define TRACEBACK_MAX 24

#ifdef DTV_SSE2
union branchtab27 {
    unsigned char c[32];
    __m128i v[2];
};
#else
struct branchtab27 {
    unsigned char c[32];
};
#endif

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
                static const unsigned char d_Partab[];

#ifdef DTV_SSE2
                static __m128i d_metric0[4];
                static __m128i d_metric1[4];
                static __m128i d_path0[4];
                static __m128i d_path1[4];
#else
                static unsigned char d_metric0_generic[64];
                static unsigned char d_metric1_generic[64];
                static unsigned char d_path0_generic[64];
                static unsigned char d_path1_generic[64];
#endif

#ifdef DTV_SSE2
                static branchtab27 Branchtab27_sse2[2];
#else
                static branchtab27 Branchtab27_generic[2];
#endif

                // Metrics for each state
                static unsigned char mmresult[64];
                // Paths for each state
                static unsigned char ppresult[TRACEBACK_MAX][64];

                // Current puncturing vector
                const unsigned char* d_puncture;

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

                int d_last_minmetric;
                int d_last_bestmetric;
                float d_new_ber;
                float d_alpha_avg;
                float d_last_ber_out;

                // Traceback (in bytes)
                int d_ntraceback;

                // Viterbi tables
                int mettab[2][256];

                // Buffer to keep the input bits
                unsigned char* d_inbits;

                // This is used to get rid of traceback on the first frame
                int d_init;

                // Position in circular buffer where the current decoded byte is stored
                int store_pos;

#ifdef DTV_SSE2
                void viterbi_chunks_init_sse2(__m128i* mm0, __m128i* pp0);
                void viterbi_butterfly2_sse2(
                        unsigned char* symbols, __m128i m0[], __m128i m1[], __m128i p0[], __m128i p1[]);
                int viterbi_get_output_sse2(__m128i* mm0,
                        __m128i* pp0,
                        int ntraceback,
                        unsigned char* outbuf);
#else
                void viterbi_chunks_init_generic(unsigned char* mm0, unsigned char* pp0);
                void viterbi_butterfly2_generic(unsigned char* symbols,
                        unsigned char m0[],
                        unsigned char m1[],
                        unsigned char p0[],
                        unsigned char p1[]);
                int viterbi_get_output_generic(unsigned char* mm0,
                        unsigned char* pp0,
                        int ntraceback,
                        unsigned char* outbuf);
#endif

            public:
                viterbi_decoder_impl(int constellation_size, int rate);
                ~viterbi_decoder_impl();

                void forecast(int noutput_items, gr_vector_int& ninput_items_required);

                int general_work(int noutput_items,
                        gr_vector_int& ninput_items,
                        gr_vector_const_void_star& input_items,
                        gr_vector_void_star& output_items);
        };
    } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_VITERBI_DECODER_IMPL_H */

