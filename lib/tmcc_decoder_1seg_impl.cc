/* -*- c++ -*- */
/*  
 * Copyright 2015
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
#include "tmcc_decoder_1seg_impl.h"
#include <cmath>

namespace gr {
    namespace isdbt {

        // Number of symbols per frame
        const int tmcc_decoder_1seg_impl::d_symbols_per_frame=204;

        // TMCC sync sequence size
        const int tmcc_decoder_1seg_impl::d_tmcc_sync_size = 16;

        // TMCC sync sequence size for odd and even frames
        const int tmcc_decoder_1seg_impl::d_tmcc_sync_even[d_tmcc_sync_size] = {0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0};
        const int tmcc_decoder_1seg_impl::d_tmcc_sync_odd[d_tmcc_sync_size] = {1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1};


        // TMCC parity check matrix written as a (k+1) = 192 dimensions vector
        const char tmcc_decoder_1seg_impl::d_h[] = {	1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,\
            1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0,\
                1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1,\
                0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1,\
                0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1,\
                1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0,\
                0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1,\
                1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1};

        // TMCC carriers size for mode 1 (2K)
        const int tmcc_decoder_1seg_impl::tmcc_carriers_size_2k = 1;
        // TMCC carriers positions for mode 1 (2K)
        const int tmcc_decoder_1seg_impl::tmcc_carriers_2k[] = {
            49};

        // TMCC carriers size for mode 2 (4K)
        const int tmcc_decoder_1seg_impl::tmcc_carriers_size_4k = 2;
        const int tmcc_decoder_1seg_impl::tmcc_carriers_4k[] = {
            23, 178 }; 
        // TMCC carriers size for mode 3 (8K)
        const int tmcc_decoder_1seg_impl::tmcc_carriers_size_8k = 4;
        const int tmcc_decoder_1seg_impl::tmcc_carriers_8k[] = {
            101, 131, 286, 349};

        // AC carriers size for mode 1 (2K)
        const int tmcc_decoder_1seg_impl::ac_carriers_size_2k = 2;
        const int tmcc_decoder_1seg_impl::d_ac_carriers_2k[] = {
            35, 79
        };

        // AC carriers size for mode 2 (4K)
        const int tmcc_decoder_1seg_impl::ac_carriers_size_4k = 4;
        const int tmcc_decoder_1seg_impl::d_ac_carriers_4k[] = {
            98, 101, 118, 136
        };

        // AC carriers size for mode 3 (8K)
        const int tmcc_decoder_1seg_impl::ac_carriers_size_8k = 8;
        const int tmcc_decoder_1seg_impl::d_ac_carriers_8k[] = {
            7, 89, 206, 209, 226, 244, 377, 407
        };

        // Number of data carriers per segment for each mode
        const int tmcc_decoder_1seg_impl::d_data_carriers_per_segment_2k = 96;
        const int tmcc_decoder_1seg_impl::d_data_carriers_per_segment_4k = 192;
        const int tmcc_decoder_1seg_impl::d_data_carriers_per_segment_8k = 384;
        const int tmcc_decoder_1seg_impl::d_carriers_per_segment_2k = 108;

        // Number of SP carriers per segment for each mode
        const int tmcc_decoder_1seg_impl::sp_per_segment_2k = 9;
        const int tmcc_decoder_1seg_impl::sp_per_segment_4k = 18;
        const int tmcc_decoder_1seg_impl::sp_per_segment_8k = 36;

        /*
         * It sets several parameters. 
         */
        void
            tmcc_decoder_1seg_impl::set_carriers_parameters(int payload)
            {

                /*
                 * Assign to variables tmcc_carriers, tmcc_carriers_size, d_ac_carriers, ac_carriers_size, 
                 * d_data_carriers_per_segment and sp_per_segment
                 * the corresponding values according to the transmission mode
                 */
                switch (payload)
                {
                    case 108:
                        {
                            tmcc_carriers = tmcc_carriers_2k;
                            tmcc_carriers_size = tmcc_carriers_size_2k;
                            d_ac_carriers = d_ac_carriers_2k;
                            ac_carriers_size = ac_carriers_size_2k;
                            d_data_carriers_per_segment = d_data_carriers_per_segment_2k;
                            sp_per_segment = sp_per_segment_2k;
                        }
                        break;
                    case 216:
                        {
                            tmcc_carriers = tmcc_carriers_4k;
                            tmcc_carriers_size = tmcc_carriers_size_4k;
                            d_ac_carriers = d_ac_carriers_4k;
                            ac_carriers_size = ac_carriers_size_4k;
                            d_data_carriers_per_segment = d_data_carriers_per_segment_4k;
                            sp_per_segment = sp_per_segment_4k;
                        }
                        break;
                    case 432:
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
                        printf("Error: unexpected payload size in TMCC DECODER 1-seg. It should be either 108, 216 or 432."); 
                        break;
                }

                //the total number of data carriers
                d_total_data_carriers = d_data_carriers_per_segment; 
                // Number of carriers per segment
                d_carriers_per_segment = d_active_carriers;

            }

        /*
         *  it constructs the d_data_carriers array. 
         */
        void tmcc_decoder_1seg_impl::construct_data_carriers_list(){

            d_data_carriers = new int[4*d_total_data_carriers]; 

            for (int symbol_index = 0; symbol_index<4; symbol_index++){
                int spilot_index = 0; 
                int ac_pilot_index = 0; 
                int tmcc_pilot_index = 0; 
                int carrier_out = 0; 


                for (int carrier = 0; carrier < d_active_carriers ;carrier++) {

                    if (carrier == (12 * spilot_index + 3 * (symbol_index % 4))) 
                    {
                        // the current carrier is an scattered pilot
                        spilot_index++;
                    } 
                    else if ((carrier == d_ac_carriers[ac_pilot_index]))
                    {
                        // the current carrier is an AC pilot
                        ac_pilot_index++;
                    } 
                    else if ((carrier == tmcc_carriers[tmcc_pilot_index])) 
                    {
                        // the current carrier is a tmcc pilot
                        tmcc_pilot_index++;
                    } 
                    else 
                    {
                        d_data_carriers[symbol_index*d_total_data_carriers + carrier_out] = carrier; 
                        //printf("carrier_out: %i, symbol_index: %i, d_data_carriers[%i]=%i\n", carrier_out, symbol_index, carrier_out+symbol_index*d_total_data_carriers, d_data_carriers[carrier_out+symbol_index*d_total_data_carriers]); 
                        carrier_out++;
                    }
                }
            }
        }

        /*
         * This method checks whether or not the BCH code in d_rcv_tmcc_data is correct. 
         */
        int 
            tmcc_decoder_1seg_impl::tmcc_parity_check( )
            {

                int n = 273,
                    k = 191,
                    r = n-k,
                    i, j, s;

                char syndrome[r];
                //unsigned char tmcc_large[n];

                std::deque<char> tmcc_large(89, 0);
                tmcc_large.insert(tmcc_large.end(), d_rcv_tmcc_data.begin()+20,d_rcv_tmcc_data.end()); 

                //memset(&tmcc_large[0], 0, 89);
                //memcpy(&tmcc_large[89], &d_rcv_tmcc_data[20], 184);

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

        /*
         * Basically a bunch of functions that interprets the TMCC bits (in the form of a deque<char>)
         * and prints its content. 
         */

        void 
            tmcc_decoder_1seg_impl::print_modulation_scheme(modulation_scheme_t modulation_scheme){

                printf("Carrier Modulation Scheme\t\t\t: ");
                switch (modulation_scheme){

                    case QPSK:
                        printf("QPSK\n");
                        break;
                    case QAM16:
                        printf("16QAM\n");
                        break;
                    case QAM64:
                        printf("64QAM\n");
                        break;
                    case M_UNUSED:
                        printf("UNUSED\n");
                        break;
                    default:
                        printf("--ERROR: something went wrong while decoding the Carrier Modulation Sheme.\n");
                }

            }
        void 
            tmcc_decoder_1seg_impl::print_convolutional_code(convolutional_code_t convolutional_code){

                printf("Convolutional Code Rate\t\t\t: ");
                switch(convolutional_code){

                    case RATE_1_2:
                        printf("1/2\n");
                        break;
                    case RATE_2_3:
                        printf("2/3\n");
                        break;
                    case RATE_3_4:
                        printf("3/4\n");
                        break;
                    case RATE_5_6:
                        printf("5/6\n");
                        break;
                    case RATE_7_8:
                        printf("7/8\n");
                        break;
                    case C_UNUSED:
                        printf("UNUSED\n");
                        break;
                    default:
                        printf("--ERROR: something went wrong while decoding the Convolutioanl Code Rate\n");	

                }

            }
        void 
            tmcc_decoder_1seg_impl::print_interleaving_length(interleaving_length_t interleaving_length){

                printf("Time Interleaving Length\t\t\t: ");
                switch(interleaving_length){

                    case I_0_0_0:
                        printf("0(mode 1), 0(mode 2), 0(mode 3)\n");
                        break;
                    case I_4_2_1:
                        printf("4(mode 1), 2(mode 2), 1(mode 3)\n");
                        break;
                    case I_8_4_2:
                        printf("8(mode 1), 4(mode 2), 2(mode 3)\n");
                        break;
                    case I_16_8_4:
                        printf("16(mode 1), 8(mode 2), 4(mode 3)\n");
                        break;
                    case I_UNUSED:
                        printf("UNUSED\n");
                        break;
                    default:
                        printf("--ERROR: something went wrong while decoding the Time Interleaving Length\n");
                }

            }

        void 
            tmcc_decoder_1seg_impl::print_number_segments(number_segments_t number_segments){

                printf("Number of segments for this layer\t: ");
                switch(number_segments){

                    case SEG_1:
                        printf("1\n");
                        break;
                    case SEG_2:
                        printf("2\n");
                        break;
                    case SEG_3:
                        printf("3\n");
                        break;
                    case SEG_4:
                        printf("4\n");
                        break;
                    case SEG_5:
                        printf("5\n");
                        break;
                    case SEG_6:
                        printf("6\n");
                        break;
                    case SEG_7:
                        printf("7\n");
                        break;
                    case SEG_8:
                        printf("8\n");
                        break;
                    case SEG_9:
                        printf("9\n");
                        break;
                    case SEG_10:
                        printf("10\n");
                        break;
                    case SEG_11:
                        printf("11\n");
                        break;
                    case SEG_12:
                        printf("12\n");
                        break;
                    case SEG_13:
                        printf("13\n");
                        break;
                    case S_UNUSED:
                        printf("UNUSED\n");
                        break;
                    default:
                        printf("--ERROR: something went wrong while decoding the Number of Segments for this layer\n");
                }

            }
        int
            tmcc_decoder_1seg_impl::tmcc_print(){

                /*We get the modulation scheme information for each layer*/
                modulation_scheme_t modulation_scheme_A, modulation_scheme_B, modulation_scheme_C;

                modulation_scheme_A = (modulation_scheme_t) ((d_rcv_tmcc_data[28]<<2) | (d_rcv_tmcc_data[29]<<1)| (d_rcv_tmcc_data[30]));
                modulation_scheme_B = (modulation_scheme_t) ((d_rcv_tmcc_data[41]<<2) | (d_rcv_tmcc_data[42]<<1)| (d_rcv_tmcc_data[43]));
                modulation_scheme_C = (modulation_scheme_t) ((d_rcv_tmcc_data[54]<<2) | (d_rcv_tmcc_data[55]<<1)| (d_rcv_tmcc_data[56]));

                /*We get the convolutional code rate information for each layer*/
                convolutional_code_t convolutional_code_A, convolutional_code_B, convolutional_code_C;

                convolutional_code_A = (convolutional_code_t) ((d_rcv_tmcc_data[31]<<2) | (d_rcv_tmcc_data[32]<<1) | d_rcv_tmcc_data[33] );
                convolutional_code_B = (convolutional_code_t) ((d_rcv_tmcc_data[44]<<2) | (d_rcv_tmcc_data[45]<<1) | d_rcv_tmcc_data[46] );
                convolutional_code_C = (convolutional_code_t) ((d_rcv_tmcc_data[57]<<2) | (d_rcv_tmcc_data[58]<<1) | d_rcv_tmcc_data[59] );

                /*We get the time interleaving length information for each layer*/
                interleaving_length_t interleaving_length_A, interleaving_length_B, interleaving_length_C;

                interleaving_length_A = (interleaving_length_t) ((d_rcv_tmcc_data[34]<<2)|(d_rcv_tmcc_data[35]<<1)|d_rcv_tmcc_data[36]);
                interleaving_length_B = (interleaving_length_t) ((d_rcv_tmcc_data[47]<<2)|(d_rcv_tmcc_data[48]<<1)|d_rcv_tmcc_data[49]);
                interleaving_length_C = (interleaving_length_t) ((d_rcv_tmcc_data[60]<<2)|(d_rcv_tmcc_data[61]<<1)|d_rcv_tmcc_data[62]);

                /*We get the number of segments used in each layer*/
                number_segments_t number_segments_A, number_segments_B, number_segments_C;

                number_segments_A = (number_segments_t) ((d_rcv_tmcc_data[37]<<3)|(d_rcv_tmcc_data[38]<<2)|(d_rcv_tmcc_data[39]<<1)|d_rcv_tmcc_data[40]);
                number_segments_B = (number_segments_t) ((d_rcv_tmcc_data[50]<<3)|(d_rcv_tmcc_data[51]<<2)|(d_rcv_tmcc_data[52]<<1)|d_rcv_tmcc_data[53]);
                number_segments_C = (number_segments_t) ((d_rcv_tmcc_data[63]<<3)|(d_rcv_tmcc_data[64]<<2)|(d_rcv_tmcc_data[65]<<1)|d_rcv_tmcc_data[66]);


                printf("\n\t\t\t\t\t\t\tTMCC ANALYSIS\t\t\t\t\t\t\n---------------------------------------------------------------------------------------------------------\n");
                printf("Layer\t\t\t\t\t\t\t\t: A\n");
                print_modulation_scheme(modulation_scheme_A);
                print_convolutional_code(convolutional_code_A);
                print_interleaving_length(interleaving_length_A);
                print_number_segments(number_segments_A);

                printf("--------------------------------------------------------------------------------------------------------\n");
                printf("Layer\t\t\t\t\t\t\t\t: B\n");
                print_modulation_scheme(modulation_scheme_B);
                print_convolutional_code(convolutional_code_B);
                print_interleaving_length(interleaving_length_B);
                print_number_segments(number_segments_B);

                printf("--------------------------------------------------------------------------------------------------------\n");
                printf("Layer\t\t\t\t\t\t\t\t: C\n");
                print_modulation_scheme(modulation_scheme_C);
                print_convolutional_code(convolutional_code_C);
                print_interleaving_length(interleaving_length_C);
                print_number_segments(number_segments_C);

                printf("--------------------------------------------------------------------------------------------------------\n");

            }

        /*
         * The aim of this method is to 1) decode the TMCC carriers and 2) detect the end of a frame indicated 
         * by the complete reception of the TMCC. 
         */
        int
            tmcc_decoder_1seg_impl::process_tmcc_data(const gr_complex * in)
            {

                int end_frame = 0;

                /*
                 * We first decide whether this OFDM symbol carries a 0 or a 1 regarding the tmcc. 
                 */

                // As we have 'tmcc_carriers_size' TMCC carriers we will be using majority voting for decision
                int tmcc_majority_zero = 0;

                // For every tmcc carrier in the symbol...
                for (int k = 0; k < tmcc_carriers_size; k++)
                {
                    // We compare the current tmcc carrier value to the previous one, as the modulation is DBPSK
                    gr_complex phdiff = in[tmcc_carriers[k]] * conj(d_prev_tmcc_symbol[k]);

                    if (phdiff.real() >= 0.0)
                    {
                        tmcc_majority_zero++;
                    }
                    else
                    {
                        tmcc_majority_zero--;
                    }

                    d_prev_tmcc_symbol[k] = in[tmcc_carriers[k]];
                }

                // Delete first element (we keep the last 204 bits)
                d_rcv_tmcc_data.pop_front();

                // Add data to tail
                if (tmcc_majority_zero >= 0)
                {
                    // If most of symbols voted a zero, we add a zero
                    d_rcv_tmcc_data.push_back(0);
                }
                else
                {
                    // Otherwise, we add a one
                    d_rcv_tmcc_data.push_back(1);
                }

                /* 
                 * We now check whether a complete TMCC frame was received. To do this we first check whether 
                 * the beginning of what we received so far is equal to the 2-bytes synchronization word. Then 
                 * we check the BCH parity code at the end. To avoid unnecesary checking, we first verify if the last TMCC 
                 * was received at least 203 frames ago. 
                 */

                if (d_since_last_tmcc>=203)
                {
                    if (std::equal(d_rcv_tmcc_data.begin() + 1, d_rcv_tmcc_data.begin() + d_tmcc_sync_size, d_tmcc_sync_evenv.begin())
                            || std::equal(d_rcv_tmcc_data.begin() + 1, d_rcv_tmcc_data.begin() + d_tmcc_sync_size, d_tmcc_sync_oddv.begin()) )
                    {
                        if (!tmcc_parity_check())
                        {
                            // Then we recognize the end of an ISDB-T frame
                            end_frame = 1;
                            d_since_last_tmcc = 0;
                            //d_symbol_index = 203;

                            // Then, we print the full tmcc
                            /*
                               for (int i = 0; i < d_symbols_per_frame; i++)
                               printf("%i", d_rcv_tmcc_data[i]);
                               printf("\n");

*/
                            if (d_print_params)
                            {
                                tmcc_print();
                            }
                            printf("TMCC OK\n"); 
                        }
                        else
                        {
                            printf("TMCC NOT OK\n");
                        }
                    }
                }
                if(!end_frame)
                {
                    d_since_last_tmcc++;
                }

                // We return end_frame
                return end_frame;

            }



        tmcc_decoder_1seg::sptr
            tmcc_decoder_1seg::make(int mode, bool print_params)
            {
                return gnuradio::get_initial_sptr
                    (new tmcc_decoder_1seg_impl(mode, print_params));
            }

        /*
         * The private constructor
         */
        tmcc_decoder_1seg_impl::tmcc_decoder_1seg_impl(int mode, bool print_params)
            : gr::sync_block("tmcc_decoder_1seg",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*(d_carriers_per_segment_2k*((int)pow(2.0,mode-1)))),
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*d_data_carriers_per_segment_2k*((int)pow(2.0,mode-1))))
        {
            d_active_carriers =d_carriers_per_segment_2k*((int)pow(2.0,mode-1)); 
            // Set the carriers parameters to mode 1, 2 or 3
            set_carriers_parameters(d_active_carriers);

            construct_data_carriers_list(); 

            d_print_params = print_params; 

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
            {
                d_rcv_tmcc_data.push_back(0);
            }

            number_symbol = 0;

            d_since_last_tmcc = 203;

            d_frame_end = false; 
            d_resync = true; 

            set_tag_propagation_policy(TPP_DONT); 


        }

        /*
         * Our virtual destructor.
         */
        tmcc_decoder_1seg_impl::~tmcc_decoder_1seg_impl()
        {
        }

        int
            tmcc_decoder_1seg_impl::work(int noutput_items,
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

                        // Moreover, if a resync was detected, it is also propagted downstream
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
                    d_frame_end = process_tmcc_data(&in[i * d_active_carriers]);

                    //currently, we obtain the symbol relative index (between 0 and 3)
                    //from the block upstream
                    this->get_tags_in_window(tags,0,i,i+1, pmt::string_to_symbol("relative_symbol_index"));
                    if (tags.size())
                    {
                        d_symbol_index = pmt::to_long(tags[0].value);
                    }
                    else 
                    {
                        printf("Warning: no relative index found in tag's stream in TMCC decoder. Mode: %i; i=%i\n", d_total_data_carriers, i); 
                    }

                    for (int data_carrier = 0; data_carrier < d_total_data_carriers; data_carrier++)
                    {
                        //printf("d_data_carriers[%i][%i]=%i; in[%i]=%i", data_carrier, d_symbol_index, d_data_carriers[data_carrier][d_symbol_index],i*d_active_carriers + d_data_carriers[data_carrier][d_symbol_index], in[i*d_active_carriers+d_data_carriers[data_carrier][d_symbol_index]]); 
                        //printf("indice_out=%i, indice_in=%i\n",i*d_total_data_carriers + data_carrier, i*d_active_carriers + d_data_carriers[d_total_data_carriers*d_symbol_index + data_carrier]); 
                        out[i*d_total_data_carriers + data_carrier] = in[i*d_active_carriers + d_data_carriers[d_total_data_carriers*d_symbol_index + data_carrier]]; 
                    }



                }
                /*
                 * Here ends the signal processing
                 */

                // Tell runtime system how many output items we produced.
                return noutput_items;

            }

    } /* namespace isdbt */
} /* namespace gr */

