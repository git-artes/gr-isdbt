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
#include "tmcc_decoder_impl.h"
#include <cmath>

namespace gr {
    namespace isdbt {
        // TODO shouldn't things like these be defined elsewhere? 
        const int tmcc_decoder_impl::d_total_segments = 13; 

        // The segments positions
        const int tmcc_decoder_impl::d_segments_positions[d_total_segments] = {11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12};

        // Number of symbols per frame
        const int tmcc_decoder_impl::d_symbols_per_frame=204;

        // TMCC sync sequence size
        const int tmcc_decoder_impl::d_tmcc_sync_size = 16;

        // TMCC sync sequence size for odd and even frames
        const int tmcc_decoder_impl::d_tmcc_sync_even[d_tmcc_sync_size] = {0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0};
        const int tmcc_decoder_impl::d_tmcc_sync_odd[d_tmcc_sync_size] = {1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1};


        // TMCC parity check matrix written as a (k+1) = 192 dimensions vector
        const char tmcc_decoder_impl::d_h[] = {	1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,\
            1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0,\
                1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1,\
                0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1,\
                0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1,\
                1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0,\
                0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1,\
                1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1};

        // TMCC carriers size for mode 1 (2K)
        const int tmcc_decoder_impl::tmcc_carriers_size_2k = 13;

        // TMCC carriers positions for mode 1 (2K)
        const int tmcc_decoder_impl::tmcc_carriers_2k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319};

        // TMCC carriers size for mode 2 (4K)
        const int tmcc_decoder_impl::tmcc_carriers_size_4k = 26;

        // TMCC carriers positions for mode 2 (4K)
        const int tmcc_decoder_impl::tmcc_carriers_4k[] = {
            70, 133, 233, 410, 476, 587, 697, 787, \
                947, 1033, 1165, 1289, 1319, 1474, 1537, 1637,\
                1814, 1880, 1991, 2101,	2191, 2351, 2437, 2569,\
                2693, 2723 }; 
        // TMCC carriers size for mode 3 (8K)
        const int tmcc_decoder_impl::tmcc_carriers_size_8k = 52;

        // TMCC carriers positions for mode 3 (8K)
        const int tmcc_decoder_impl::tmcc_carriers_8k[] = {
            70, 133, 233, 410,	\
                476, 587, 697, 787, \
                947, 1033, 1165, 1289,\
                1319, 1474, 1537, 1637,\
                1814, 1880, 1991, 2101,\
                2191, 2351, 2437, 2569,\
                2693, 2723, 2878, 2941,\
                3041, 3218, 3284, 3395,\
                3505, 3595, 3755, 3841,\
                3973, 4097, 4127, 4282,\
                4345, 4445, 4622, 4688,\
                4799, 4909, 4999, 5159,\
                5245, 5377, 5501, 5531 };

        // AC carriers size for mode 1 (2K)
        const int tmcc_decoder_impl::ac_carriers_size_2k = 26;

        // AC carriers positions for mode 1
        const int tmcc_decoder_impl::d_ac_carriers_2k[] = {
            10, 28, 161, 191, 277, 316, 335, 425,\
                452, 472, 614, 640, 683, 727, 832, 853,\
                868, 953, 1012, 1061, 1088, 1144, 1195, 1277,\
                1394, 1397
        };

        // AC carriers size for mode 2 (4K)
        const int tmcc_decoder_impl::ac_carriers_size_4k = 52;

        // AC carriers positions for mode 2
        const int tmcc_decoder_impl::d_ac_carriers_4k[] = {
            10, 28, 161, 191, 277, 316, 335, 425,\
                452, 472, 614, 640, 683, 727, 832, 853,\
                868, 953, 1012, 1061, 1088, 1144, 1195, 1277,\
                1394, 1397, 1414, 1432, 1565, 1595, 1681, 1720,\
                1739, 1829, 1856, 1876, 2018, 2044, 2087, 2131,\
                2236, 2257, 2272, 2357, 2416, 2465, 2492, 2548,\
                2599, 2681, 2798, 2801
        };

        // AC carriers size for mode 3 (8K)
        const int tmcc_decoder_impl::ac_carriers_size_8k = 104;

        // AC carriers positions for mode 3 (2K)
        const int tmcc_decoder_impl::d_ac_carriers_8k[] = {
            10, 28, 161, 191, 277, 316, 335, 425,\
                452, 472, 614, 640, 683, 727, 832, 853,\
                868, 953, 1012, 1061, 1088, 1144, 1195, 1277,\
                1394, 1397, 1414, 1432, 1565, 1595, 1681, 1720,\
                1739, 1829, 1856, 1876, 2018, 2044, 2087, 2131,\
                2236, 2257, 2272, 2357, 2416, 2465, 2492, 2548,\
                2599, 2681, 2798, 2801, 2818, 2836, 2969, 2999,\
                3085, 3124, 3143, 3233, 3260, 3280, 3422, 3448,\
                3491, 3535, 3640, 3661, 3676, 3761, 3820, 3869,\
                3896, 3952, 4003, 4085, 4202, 4205, 4222, 4240,\
                4373, 4403, 4489, 4528, 4547, 4637, 4664, 4684,\
                4826, 4852, 4895, 4939, 5044, 5065, 5080, 5165,\
                5224, 5273, 5300, 5356, 5407, 5489, 5606, 5609 };

        // Number of data carriers per segment for each mode
        const int tmcc_decoder_impl::d_data_carriers_per_segment_2k = 96;
        const int tmcc_decoder_impl::d_data_carriers_per_segment_4k = 192;
        const int tmcc_decoder_impl::d_data_carriers_per_segment_8k = 384;
        const int tmcc_decoder_impl::d_carriers_per_segment_2k = 108;

        // Number of SP carriers per segment for each mode
        const int tmcc_decoder_impl::sp_per_segment_2k = 9;
        const int tmcc_decoder_impl::sp_per_segment_4k = 18;
        const int tmcc_decoder_impl::sp_per_segment_8k = 36;

        void
            tmcc_decoder_impl::carriers_parameters(int payload)
            {

                /*
                 * Assing to variables tmcc_carriers, tmcc_carriers_size, d_ac_carriers, ac_carriers_size, d_data_carriers_per_segment and sp_per_segment
                 * the corresponding values according to the transmission mode
                 */
                switch (payload)
                {
                    case 1405:
                        {
                            tmcc_carriers = tmcc_carriers_2k;
                            tmcc_carriers_size = tmcc_carriers_size_2k;
                            d_ac_carriers = d_ac_carriers_2k;
                            ac_carriers_size = ac_carriers_size_2k;
                            d_data_carriers_per_segment = d_data_carriers_per_segment_2k;
                            sp_per_segment = sp_per_segment_2k;
                        }
                        break;
                    case 2809:
                        {
                            tmcc_carriers = tmcc_carriers_4k;
                            tmcc_carriers_size = tmcc_carriers_size_4k;
                            d_ac_carriers = d_ac_carriers_4k;
                            ac_carriers_size = ac_carriers_size_4k;
                            d_data_carriers_per_segment = d_data_carriers_per_segment_4k;
                            sp_per_segment = sp_per_segment_4k;
                        }
                        break;
                    case 5617:
                        {
                            tmcc_carriers = tmcc_carriers_8k;
                            tmcc_carriers_size = tmcc_carriers_size_8k;
                            d_ac_carriers = d_ac_carriers_8k;
                            ac_carriers_size = ac_carriers_size_8k;
                            d_data_carriers_per_segment = d_data_carriers_per_segment_8k;
                            sp_per_segment = sp_per_segment_8k;
                        }
                        break;
                    default:
                        break;
                }

            }

        int 
            tmcc_decoder_impl::tmcc_parity_check(std::deque<char> d_rcv_tmcc_data)
            {

                int n = 273,
                    k = 191,
                    r = n-k,
                    i, j, s;

                char syndrome[r];
                unsigned char tmcc_large[n];

                memset(&tmcc_large[0], 0, 89);
                memcpy(&tmcc_large[89], &d_rcv_tmcc_data[20], 184);

                s = 0;
                for (j=0;j<r;j++){
                    syndrome[j] = 0;
                    for (i=0;i<k+1;i++){
                        syndrome[j] += tmcc_large[i+j]*d_h[i];
                    }
                    if (syndrome[j] % 2) s++;
                }
                return s;
            }

        int
            tmcc_decoder_impl::tmcc_print(std::deque<char> d_rcv_tmcc_data){

                printf("** TMCC ANALYSIS **\n\n");
                printf("Partial recepcion			: %d\n\n", d_rcv_tmcc_data[27]);
                printf(">> Layer A\n\n\
                        Carrier Modulation Scheme	: %d%d%d\n\
                        Convolutional Coding Rate	: %d%d%d\n\
                        Interleaving Length			: %d%d%d\n\
                        Number of Segments		: %d%d%d%d\n\n",\
                        d_rcv_tmcc_data[28], d_rcv_tmcc_data[29],d_rcv_tmcc_data[30],\
                        d_rcv_tmcc_data[31],d_rcv_tmcc_data[32],d_rcv_tmcc_data[33],\
                        d_rcv_tmcc_data[34],d_rcv_tmcc_data[35],d_rcv_tmcc_data[36],\
                        d_rcv_tmcc_data[37],d_rcv_tmcc_data[38],d_rcv_tmcc_data[39],d_rcv_tmcc_data[40]);

                printf(">> Layer B\n\n\
                        Carrier Modulation Scheme	: %d%d%d\n\
                        Convolutional Coding Rate	: %d%d%d\n\
                        Interleaving Length			: %d%d%d\n\
                        Number of Segments		: %d%d%d%d\n\n",\
                        d_rcv_tmcc_data[41], d_rcv_tmcc_data[42],d_rcv_tmcc_data[43],\
                        d_rcv_tmcc_data[44],d_rcv_tmcc_data[45],d_rcv_tmcc_data[46], \
                        d_rcv_tmcc_data[47],d_rcv_tmcc_data[48],d_rcv_tmcc_data[49],\
                        d_rcv_tmcc_data[50],d_rcv_tmcc_data[51],d_rcv_tmcc_data[52],d_rcv_tmcc_data[53]);

                printf(">> Layer C\n\n\
                        Carrier Modulation Scheme	: %d%d%d\n\
                        Convolutional Coding Rate	: %d%d%d\n\
                        Interleaving Length			: %d%d%d\n\
                        Number of Segments		: %d%d%d%d\n\n",\
                        d_rcv_tmcc_data[54], d_rcv_tmcc_data[55],d_rcv_tmcc_data[56],\
                        d_rcv_tmcc_data[57],d_rcv_tmcc_data[58],d_rcv_tmcc_data[59],\
                        d_rcv_tmcc_data[60],d_rcv_tmcc_data[61],d_rcv_tmcc_data[62],\
                        d_rcv_tmcc_data[63],d_rcv_tmcc_data[64],d_rcv_tmcc_data[65],d_rcv_tmcc_data[66]);

            }

        int
            tmcc_decoder_impl::process_tmcc_data(const gr_complex * in)
            {
                /*
                 * The aim of this method is to detect the end of a frame
                 */

                int end_frame = 0;

                // As we have 'tmcc_carriers_size' TMCC carriers we will be using majority voting for decision
                int tmcc_majority_zero = 0;

                // For every tmcc carrier in the symbol...
                for (int k = 0; k < tmcc_carriers_size; k++)
                {
                    // We compare the current tmcc carrier value to the previous one, as the modulation is DBPSK
                    gr_complex phdiff = in[tmcc_carriers[k]] * conj(d_prev_tmcc_symbol[k]);

                    if (phdiff.real() >= 0.0)
                        tmcc_majority_zero++;
                    else
                        tmcc_majority_zero--;

                    d_prev_tmcc_symbol[k] = in[tmcc_carriers[k]];
                }

                // Delete first element (we keep the last 204 bits)
                d_rcv_tmcc_data.pop_front();

                // Add data to tail
                if (tmcc_majority_zero >= 0)
                    // If most of symbols voted a zero, we add a zero
                    d_rcv_tmcc_data.push_back(0);
                else
                    // Otherwise, we add a one
                    d_rcv_tmcc_data.push_back(1);

                // Match synchronization signatures

                // We compare bits 1 to 16 of the d_rcv_tmcc_data queue to the even tmcc sync sequence stored in the d_tmcc_sync_evenv queue
                if (std::equal(d_rcv_tmcc_data.begin() + 1, d_rcv_tmcc_data.begin() + d_tmcc_sync_size, d_tmcc_sync_evenv.begin()) && (!tmcc_parity_check(d_rcv_tmcc_data)))
                {
                    // Then we recognize the end of an ISDB-T frame
                    end_frame = 1;

                    // This should be erased
                    //end_frame = d_since_last_tmcc==203;
                    //d_since_last_tmcc = 0;
                    //d_symbol_index = 203;

                    // Then, we print the full tmcc
                    /*
                       for (int i = 0; i < d_symbols_per_frame; i++)
                       printf("%i", d_rcv_tmcc_data[i]);
                       printf("\n");

*/
                    //tmcc_print(d_rcv_tmcc_data);
                    printf("TMCC OK\n"); 
                }
                // We compare bits 1 to 16 of the d_rcv_tmcc_data queue to the odd tmcc sync sequence stored in the d_tmcc_sync_oddv queue
                else if (std::equal(d_rcv_tmcc_data.begin() + 1, d_rcv_tmcc_data.begin() + d_tmcc_sync_size, d_tmcc_sync_oddv.begin()) && (!tmcc_parity_check(d_rcv_tmcc_data)))
                {

                    // Then we recognize the end of an ISDB-T frame
                    end_frame = 1;

                    // This should be erased
                    // end_frame = d_since_last_tmcc==203;
                    //d_since_last_tmcc = 0;
                    //d_symbol_index = 203;

                    // Then, we print the full tmcc
                    /*
                       for (int i = 0; i < d_symbols_per_frame; i++)
                       printf("%i", d_rcv_tmcc_data[i]);
                       printf("\n");
                       */
                    //tmcc_print(d_rcv_tmcc_data);
                    printf("TMCC OK\n"); 

                }
                // This should be erased
                //else 
                // {
                //  d_since_last_tmcc++;
                // }

                // We return end_frame
                return end_frame;

            }

        /*
         * ---------------------------------------------------------------------
         */

        tmcc_decoder::sptr
            tmcc_decoder::make(int mode)
            {
                return gnuradio::get_initial_sptr
                    (new tmcc_decoder_impl(mode));
            }

        /*
         * The private constructor
         */
        //TODO the block complains about :
        //gr::buffer::allocate_buffer: warning: tried to allocate
        //   4 items of size 44928. Due to alignment requirements
        //   32 were allocated.  If this isn't OK, consider padding
        //   your structure to a power-of-two bytes.
        //   On this platform, our allocation granularity is 4096 bytes.
        tmcc_decoder_impl::tmcc_decoder_impl(int mode)
            : gr::block("tmcc_decoder",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*(1+d_total_segments*d_carriers_per_segment_2k*((int)pow(2.0,mode-1)))),
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*d_total_segments*d_data_carriers_per_segment_2k*((int)pow(2.0,mode-1))))
        {
            active_carriers = d_total_segments*d_carriers_per_segment_2k*((int)pow(2.0,mode-1)) + 1; 
            // Set the carriers parameters to mode 1, 2 or 3
            carriers_parameters(active_carriers);

            // Number of carriers per segment
            d_carriers_per_segment = (active_carriers-1)/d_total_segments;

            // We allocate memory for d_prev_tmcc_symbol attribute
            d_prev_tmcc_symbol = new gr_complex[tmcc_carriers_size];
            if (d_prev_tmcc_symbol == NULL)
            {
                std::cout << "error allocating d_prev_tmcc_symbol" << std::endl;
                return;
            }
            memset(d_prev_tmcc_symbol, 0, tmcc_carriers_size * sizeof(gr_complex));

            // Init TMCC sync sequence
            for (int i = 0; i < d_tmcc_sync_size; i++)
            {
                d_tmcc_sync_evenv.push_back(d_tmcc_sync_even[i]);
                d_tmcc_sync_oddv.push_back(d_tmcc_sync_odd[i]);
            }

            // Init receive TMCC data vector
            for (int i = 0; i < d_symbols_per_frame; i++)
                d_rcv_tmcc_data.push_back(0);

            number_symbol = 0;

            // This should be erased 
            //d_since_last_tmcc = 203;

            d_frame_end = false; 
            d_resync = true; 

            set_tag_propagation_policy(TPP_DONT); 

        }

        /*
         * Our virtual destructor.
         */
        tmcc_decoder_impl::~tmcc_decoder_impl()
        {
        }

        void
            tmcc_decoder_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
            {
                int ninputs = ninput_items_required.size();

                for (int i = 0; i < ninputs; i++)
                    ninput_items_required[i] = noutput_items;

            }

        int
            tmcc_decoder_impl::general_work (int noutput_items,
                    gr_vector_int &ninput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const gr_complex *in = (const gr_complex *) input_items[0];
                gr_complex *out = (gr_complex *) output_items[0];
                

                /*
                 * Here starts the signal processing
                 */
                for (int i = 0; i < noutput_items; i++) {
                    // We check if the block upstream signaled us to re-start sync. If so, we should also signal it downstream
                    std::vector<tag_t> tags; 
                    this->get_tags_in_window(tags,0,i,i+1, pmt::string_to_symbol("resync"));
                    if (tags.size())
                    {
                        d_resync = true; 
                        printf("TMCC DECODER: restart sync\n"); 
                    }
                    if (d_frame_end) {
                        // Signal the beginning of the frame downstream
                        // Being here means that the last OFDM symbol 
                        // constituted a frame ending. 
                        const uint64_t offset = this->nitems_written(0)+i; 
                        //printf("tmcc_decoder: offset=%d, i=%d", offset,i); 
                        pmt::pmt_t key = pmt::string_to_symbol("frame_begin"); 
                        pmt::pmt_t value = pmt::from_long(0xaa); 
                        this->add_item_tag(0,offset,key,value); 
                        if (d_resync){
                            d_resync = false; 
                            const uint64_t offset = this->nitems_written(0)+i; 
                            //printf("tmcc_decoder: offset=%d, i=%d", offset,i); 
                            pmt::pmt_t key = pmt::string_to_symbol("resync"); 
                            pmt::pmt_t value = pmt::string_to_symbol("generated by tmcc decoder"); 
                            this->add_item_tag(0,offset,key,value); 

                        }
                    }

                    // Process TMCC data

                    //If a frame is recognized then set signal end of frame to true
                    d_frame_end = process_tmcc_data(&in[i * active_carriers]);

                    // This should be erased
                    /*
                    // check whether a symbol index skip was detected upstream
                    // and correct d_symbol_index accordingly
                    std::vector<tag_t> tags; 
                    const uint64_t nread = this->nitems_read(0); 
                    this->get_tags_in_window(tags,0,i,i+1, pmt::string_to_symbol("symbol_index_skip"));
                    if (tags.size())
                    {
                    d_symbol_index = (d_symbol_index + pmt::to_long(tags[0].value)) % 204;
                    printf("se detecto un salto de %d\n",pmt::to_long(tags[0].value));
                    }
                    */

                    //currently, we obtain the symbol relative index (between 0 and 3)
                    //from the block upstream
                    this->get_tags_in_window(tags,0,i,i+1, pmt::string_to_symbol("relative_symbol_index"));
                    if (tags.size())
                        d_symbol_index = pmt::to_long(tags[0].value);
                    else 
                        printf("Warning: no relative index found in tag's stream"); 


                    // Declare and initialize to zero the number of data carrier, spilot, acpilot and tmcc pilot found
                    int carrier_out = 0;
                    int d_spilot_index = 0;
                    int d_acpilot_index = 0;
                    int d_tmccpilot_index = 0;

                    // We will determinate which kind of carrier is each. If we find a data carrier, we let it out
                    for (int carrier = 0; carrier < (active_carriers - 1);carrier++) {

                        // We see if the current carrier is an scattered pilot
                        if (carrier == (12 * d_spilot_index + 3 * (d_symbol_index % 4))) {
                            d_spilot_index++;

                        } else {
                            // We see if the current carrier is an AC pilot
                            if ((carrier == d_ac_carriers[d_acpilot_index]))
                            {
                                d_acpilot_index++;

                            } else {
                                // We see if the current carrier is a tmcc pilot
                                if ((carrier == tmcc_carriers[d_tmccpilot_index])) {
                                    d_tmccpilot_index++;

                                } else {
                                    // If is none of then we let the carrier out in the proper order
                                    //out[carrier_out] = in[i * active_carriers + carrier];
                                    gr_complex salida = in[i*active_carriers + carrier]; 
                                    out[i*d_data_carriers_per_segment*d_total_segments + (d_segments_positions[carrier_out/d_data_carriers_per_segment]*d_data_carriers_per_segment) + (carrier_out % d_data_carriers_per_segment) ] = salida;
                                    /*if (std::abs(salida.imag()) < 0.01){
                                      printf("problemas (sym=%d, portadora = %d, abs(salida.imag())=%f): out=%f+j%f\n",d_symbol_index,carrier, std::abs(salida.imag()),salida.real(),salida.imag());
                                      for (int j=-5; j<5; j++)
                                      printf("in[%d+%d]=%f+j%f\n",i*active_carriers+carrier,j,in[i*active_carriers+carrier+j].real(),in[i*active_carriers+carrier+j].imag());
                                      }*/
                                    carrier_out++;
                                }
                            }
                        }
                    }
                    //printf("data carriers: %d\n",carrier_out);
                }
                /*
                 * Here ends the signal processing
                 */

                // Tell runtime system how many input items we consumed on
                // each input stream.
                consume_each (noutput_items);

                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

