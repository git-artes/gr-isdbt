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
#include "viterbi_decoder_1seg_impl.h"
#include <xmmintrin.h>
#include <stdio.h>
#include <sys/time.h>

// TODO - these variables should not be static/global
// but members of the class. DO the change when refactoring
// the viterbi decoder.
static __m128i metric0[4] __attribute__ ((aligned(16)));
static __m128i metric1[4] __attribute__ ((aligned(16)));
static __m128i path0[4] __attribute__ ((aligned(16)));
static __m128i path1[4] __attribute__ ((aligned(16)));

namespace gr {
    namespace isdbt {

        const unsigned char viterbi_decoder_1seg_impl::d_puncture_1_2[2] = {1, 1};
        const unsigned char viterbi_decoder_1seg_impl::d_puncture_2_3[4] = {1, 1, 0, 1};
        const unsigned char viterbi_decoder_1seg_impl::d_puncture_3_4[6] = {1, 1, 0, 1, 1, 0};
        const unsigned char viterbi_decoder_1seg_impl::d_puncture_5_6[10] = {1, 1, 0, 1, 1, 0, 0, 1, 1, 0};
        const unsigned char viterbi_decoder_1seg_impl::d_puncture_7_8[14] = {1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0};

        viterbi_decoder_1seg::sptr
            viterbi_decoder_1seg::make()
            {
                return gnuradio::get_initial_sptr
                    (new viterbi_decoder_1seg_impl());
            }

        /*
         * The private constructor
         */
        viterbi_decoder_1seg_impl::viterbi_decoder_1seg_impl()
            : gr::block("viterbi_decoder_1seg",
                    gr::io_signature::make(1, 1, sizeof(unsigned char)),
                    gr::io_signature::make(1, 1, sizeof(unsigned char)))
        {
            // block size
            d_bsize = 408; 
            // initial state
            d_init = 0; 
            // input of encoder
            d_k = 1; 
            // outpuput of encoder
            d_n = 2;
            // constellation size
            d_m = 2;  

            // depuncturing matrix
            d_puncture = d_puncture_2_3;
            d_ntraceback = 9;

            /*
             * We input n bytes, each carrying m bits => nm bits
             * The result after decoding is km bits, therefore km/8 bytes.
             *
             * out/in rate is km/8n in bytes. We are outputting unpacked bytes. 
             * To generate k bits, we needed n bits (and since we are inputting
             * bytes with one symbol at a time, each with m bits). Thus, the 
             * relative rate would be d_k/(d_n/d_m). However, since we are outputting
             * packed bytes (all 8 bits carry useful data) the output rate is divided by
             * 8. 
             */
            assert((d_k * d_m) % (8 * d_n));
            set_relative_rate((d_k * d_m) / (8 * d_n));

            assert ((d_bsize * d_n) % d_m == 0);
            set_output_multiple (d_bsize * d_k / 8);

            /*
             * Calculate process variables:
             * Number of symbols (d_m bits) in all blocks
             * It is also the number of input bytes since
             * one byte always contains just one symbol.
             */
            d_nsymbols = d_bsize * d_n / d_m;
            // Number of bits after depuncturing a block (before decoding)
            d_nbits = 2 * d_k * d_bsize;
            // Number of output bytes after decoding
            d_nout = d_nbits / 2 / 8;

            // Allocate the buffer for the bits
            d_inbits = new unsigned char [d_nbits];
            if (d_inbits == NULL)
                std::cout << "error allocating d_inbits" << std::endl;


            // TODO - clean this up
            int amp = 100;
            float RATE=0.5;
            float ebn0 = 12.0;
            float esn0 = RATE*pow(10.0, ebn0/10);
            d_gen_met(mettab, amp, esn0, 0.0, 4);
            d_viterbi_chunks_init(state0);

            d_viterbi_chunks_init_sse2(metric0, path0);
        }

        /*
         * Our virtual destructor.
         */
        viterbi_decoder_1seg_impl::~viterbi_decoder_1seg_impl()
        {
            delete [] d_inbits;
        }

        void
            viterbi_decoder_1seg_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
            {
                int input_required = noutput_items * 8 * d_n / (d_k * d_m);

                unsigned ninputs = ninput_items_required.size();
                for (unsigned int i = 0; i < ninputs; i++) {
                    ninput_items_required[i] = input_required;
                }
            }

        int
            viterbi_decoder_1seg_impl::general_work (int noutput_items,
                    gr_vector_int &ninput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                int nstreams = input_items.size();
                int nblocks = 8 * noutput_items / (d_bsize * d_k);
                int out_count = 0;

                //gettimeofday(&tvs, &tzs);

                for (int m=0;m<nstreams;m++)
                {
                    const unsigned char *in = (const unsigned char *) input_items[m];
                    unsigned char *out = (unsigned char *) output_items[m];

                    /*
                     * Look for a tag that signals frame_begin and consume all input items
                     * that are in input buffer so far.
                     * This will actually reset the viterbi decoder.
                     */
                    std::vector<tag_t> tags;
                    const uint64_t nread = this->nitems_read(0); //number of items read on port 0
                    this->get_tags_in_range(tags, 0, nread, nread + (nblocks * d_nsymbols), pmt::string_to_symbol("frame_begin"));

                    if (tags.size())
                    {
                        d_init = 0;
                        d_viterbi_chunks_init_sse2(metric0, path0);

                        //printf("viterbi: superframe_start: %i\n", tags[0].offset - nread);

                        if (tags[0].offset - nread)
                        {
                            consume_each(tags[0].offset - nread);
                            return (0);
                        }
                    }

                    // This is actually the Viterbi decoder
                    for (int n = 0; n < nblocks; n++)
                    {
                        /*
                         * Depuncture and unpack a block.
                         * We receive the symbol (d_m bits/byte) in one byte (e.g. for QAM16 00001111).
                         * Create a buffer of bytes containing just one bit/byte.
                         * Also depuncture according to the puncture vector.
                         * TODO - reduce the number of branches while depuncturing.
                         */
                        for (int count = 0, i = 0; i < d_nsymbols; i++)
                        {
                            for (int j = (d_m - 1); j >= 0; j--)
                            {
                                // Depuncture
                                while (d_puncture[count % (2 * d_k)] == 0)
                                    d_inbits[count++] = 2;

                                // Insert received bits
                                d_inbits[count++] = (in[(n * d_nsymbols) + i] >> j) & 1;

                                // Depuncture
                                while (d_puncture[count % (2 * d_k)] == 0)
                                    d_inbits[count++] = 2;
                            }
                        }

                        /*
                         * Decode a block.
                         */
                        for (int in_count = 0; in_count < d_nbits; in_count++)
                        {
                            if ((in_count % 4) == 0) //0 or 3
                            {
                                d_viterbi_butterfly2_sse2(&d_inbits[in_count & 0xfffffffc], metric0, metric1, path0, path1);
                                //d_viterbi_butterfly2(&d_inbits[in_count & 0xfffffffc], mettab, state0, state1);

                                if ((in_count > 0) && (in_count % 16) == 8) // 8 or 11
                                {
                                    unsigned char c;

                                    d_viterbi_get_output_sse2(metric0, path0, d_ntraceback, &c);
                                    //d_viterbi_get_output(state0, &c);

                                    if (d_init == 0)
                                    {
                                        if (out_count >= d_ntraceback)
                                        {
                                            out[out_count - d_ntraceback] = c;
                                            //printf("out_init[%i]: %x\n", out_count - d_ntraceback, out[out_count - d_ntraceback]);
                                        }
                                    }
                                    else
                                    {
                                        out[out_count] = c;
                                        //printf("out[%i]: %x\n", out_count, out[out_count]);
                                    }

                                    out_count++;
                                }
                            }
                        }
                    }
                }

                int to_out = noutput_items;

                if (d_init == 0)
                {
                    /*
                     * Send frame_begin to signal this situation
                     * downstream
                     */
                    const uint64_t offset = this->nitems_written(0);
                    pmt::pmt_t key = pmt::string_to_symbol("superframe_start");
                    pmt::pmt_t value = pmt::from_long(1);
                    this->add_item_tag(0, offset, key, value);

                    // Take in consideration the traceback length
                    to_out = to_out - d_ntraceback;
                    d_init = 1;
                }

                //gettimeofday(&tve, &tze);
                //PRINTF("VITERBI: nblocks: %i, out: %f Mbit/s\n", \
                //        nblocks, (float) (nblocks * d_nout * 8) / (float)(tve.tv_usec - tvs.tv_usec));

                // Tell runtime system how many input items we consumed on
                // each input stream.
                consume_each (nblocks * d_nsymbols);

                // Tell runtime system how many output items we produced.
                return (to_out);
            }

    } /* namespace isdbt */
} /* namespace gr */

