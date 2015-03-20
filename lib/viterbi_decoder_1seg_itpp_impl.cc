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
#include "viterbi_decoder_1seg_itpp_impl.h"
#include <stdio.h>
//#include <itpp/itcomm.h>

namespace gr {
    namespace isdbt {

        viterbi_decoder_1seg_itpp::sptr
            viterbi_decoder_1seg_itpp::make()
            {
                return gnuradio::get_initial_sptr
                    (new viterbi_decoder_1seg_itpp_impl());
            }

        /*
         * The private constructor
         */
        viterbi_decoder_1seg_itpp_impl::viterbi_decoder_1seg_itpp_impl()
            : gr::block("viterbi_decoder_1seg_itpp",
                    gr::io_signature::make(1, 1, sizeof(unsigned char)),
                    gr::io_signature::make(1, 1, sizeof(unsigned char)))
        {
            // the block of bytes we will decode
            d_bsize = 408; 

            // the polynomial generator for the convolutional code
            itpp::ivec generator(2); 
            generator(0)=0171;
            generator(1)=0133;
            d_code.set_generator_polynomials(generator, 7);
            // we now set the depuncturing matrix
            itpp::bmat puncture_matrix = "1 0; 1 1;";
            d_code.set_puncture_matrix(puncture_matrix);  
            //TODO this value has to be tested. I do not know if this is correct. 
            d_code.set_truncation_length(40); 


            d_m = 2; 

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
            set_relative_rate((2.0 * d_m) / (8.0 * 3.0));


            // I will output one TSP each time
            set_output_multiple (204);

        }

        /*
         * Our virtual destructor.
         */
        viterbi_decoder_1seg_itpp_impl::~viterbi_decoder_1seg_itpp_impl()
        {
        }

        void
            viterbi_decoder_1seg_itpp_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
            {
                int input_required = noutput_items * 8 * 3 / (2 * d_m);

                unsigned ninputs = ninput_items_required.size();
                for (unsigned int i = 0; i < ninputs; i++) {
                    ninput_items_required[i] = input_required;
                }
            }

        int
            viterbi_decoder_1seg_itpp_impl::general_work (int noutput_items,
                    gr_vector_int &ninput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const unsigned char *in = (const unsigned char *) input_items[0];
                unsigned char *out = (unsigned char *) output_items[0];

                /*
                 * Look for a tag that signals frame_begin and consume all input items
                 * that are in input buffer so far.
                 * This will actually reset the viterbi decoder.
                 */
                std::vector<tag_t> tags;
                const uint64_t nread = this->nitems_read(0); //number of items read on port 0
                this->get_tags_in_window(tags, 0, 0, noutput_items*8*3/2, pmt::string_to_symbol("frame_begin"));

                if (tags.size())
                {
                    printf("viterbi: superframe_start: %i\n", tags[0].offset - nread);

                    d_code.reset(); 
                    // if we are not aligned with the beginning of a frame, we go 
                    // to that point by consuming all the inputs
                    if (tags[0].offset - nread)
                    {
                        consume_each(tags[0].offset - nread);
                        return (0);
                    }

                    // signal the frame start downstream 
                    const uint64_t offset = this->nitems_written(0);
                    pmt::pmt_t key = pmt::string_to_symbol("frame_begin");
                    pmt::pmt_t value = pmt::from_long(1);
                    this->add_item_tag(0, offset, key, value);
                }

                //printf("viterbi: noutput_items: %d, ninput_items[0]:%d\n", noutput_items,ninput_items[0]);
                itpp::vec rx_signal(noutput_items*8*3/2);
                for (int i=0; i<rx_signal.length()/d_m; i++){
                    //printf("viterbi: in[%i]=%x, ", i, in[i]);
                    for (int b=0; b<d_m; b++){
                        // TODO I believe that we first receive the MSb, 
                        // but I am not sure. Check. 
                        rx_signal[i*d_m+b] = ( ((in[i]>>(d_m-b-1)) & 1) == 1 ? -1:1 ); 
                        // this last statement looks somewhat complicated, but it simply assigns a 
                        // -1 if the coded bit was a 1, and a 1 if the coded bit was a 0, following 
                        // the soft bits coding by it++. 
                        //printf("viterbi: rx_signal[%i]=%f ",i*d_m+b,rx_signal[i*d_m+b]);
                    }
                    //printf("\n");
                } 

                itpp::bvec d_decoded_bits; 
                d_code.decode_tail(rx_signal, d_decoded_bits); 

                //printf("viterbi: noutput_items = %i, d_decoded_bits.length()/8=%f", noutput_items, d_decoded_bits.length()/8.0); 
                //noutput_items = d_decoded_bits.length()/8; 
                for(int i=0; i<noutput_items; i++){
                    out[i]=0; 
                    for (int b=0; b<8; b++){
                        //printf("decoded_bits[%i]=%x ", b,(bool) d_decoded_bits[i*8+b]); 
                        out[i] |= ((bool)d_decoded_bits[i*8+b])<<(8-b-1); 
                    }
                    //if(out[i]==0x47) printf("out[%i]=%x \n", i, out[i]);
                }

                // Tell runtime system how many input items we consumed on
                // each input stream.
                consume_each (noutput_items*8*3/2);

                // Tell runtime system how many output items we produced.
                return noutput_items;

            }

    } /* namespace isdbt */
} /* namespace gr */

