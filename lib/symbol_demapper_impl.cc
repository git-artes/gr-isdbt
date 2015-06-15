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
#include <stdio.h>

#include <gnuradio/io_signature.h>
#include "symbol_demapper_impl.h"

namespace gr {
    namespace isdbt {

        // TODO shouldn't these go somewhere else??
        const int symbol_demapper_impl::d_data_carriers_mode1 = 96; 
        const int symbol_demapper_impl::d_total_segments = 13; 
        const float symbol_demapper_impl::d_th_16qam = 2.0/sqrt(10.0); 
        const float symbol_demapper_impl::d_th_64qam = 2.0/sqrt(42.0); 

        symbol_demapper::sptr
            symbol_demapper::make(int mode, int segments_A, int constellation_size_A, int segments_B, int constellation_size_B, int segments_C, int constellation_size_C)
            {
                return gnuradio::get_initial_sptr
                    (new symbol_demapper_impl(mode, segments_A, constellation_size_A, segments_B, constellation_size_B, segments_C, constellation_size_C));
            }

        /*
         * The private constructor
         */
        symbol_demapper_impl::
            symbol_demapper_impl(int mode, int segments_A, int constellation_size_A, int segments_B, int constellation_size_B, int segments_C, int constellation_size_C)
                : gr::sync_block("symbol_demapper",
                    gr::io_signature::make(1, 1, 
                        sizeof(gr_complex) * d_total_segments *
                            d_data_carriers_mode1 * ((int)pow(2.0,mode-1))),
                    gr::io_signature::make3(1, 3, sizeof(unsigned char) * segments_A * d_data_carriers_mode1 * ((int)pow(2.0,mode-1)), sizeof(unsigned char) * segments_B * d_data_carriers_mode1 * ((int)pow(2.0,mode-1)), sizeof(unsigned char) * segments_C * d_data_carriers_mode1 * ((int)pow(2.0,mode-1))))
                        //sizeof(unsigned char) * d_total_segments * d_data_carriers_mode1 * ((int)pow(2.0,mode-1))))
        {
            d_mode = mode; 
            d_const_size_A = constellation_size_A;
		   	d_const_size_B = constellation_size_B;
			d_const_size_C = constellation_size_C;	
            
			d_nsegments_A = segments_A;
			d_nsegments_B = segments_B;
			d_nsegments_C = segments_C;
			if (d_const_size_A==4) {
                find_constellation_value_lA = &symbol_demapper_impl::find_constellation_value_qpsk; 
            } else if (d_const_size_A==16) {
                find_constellation_value_lA = &symbol_demapper_impl::find_constellation_value_16qam; 
            } else if (d_const_size_A==64) {
                find_constellation_value_lA = &symbol_demapper_impl::find_constellation_value_64qam; 
            } else {
            std::cout << "symbol_demapper: error in d_const_size_A\n"; 
            }

            if (d_const_size_B==4) {
                find_constellation_value_lB = &symbol_demapper_impl::find_constellation_value_qpsk; 
	        } else if (d_const_size_B==16) {
                find_constellation_value_lB = &symbol_demapper_impl::find_constellation_value_16qam; 
		    } else if (d_const_size_B==64) {
                find_constellation_value_lB = &symbol_demapper_impl::find_constellation_value_64qam; 
			} else {
				std::cout << "symbol_demapper: error in d_const_size_B\n";
			}

            if (d_const_size_C == 4) {
                find_constellation_value_lC = &symbol_demapper_impl::find_constellation_value_qpsk; 
			} else if (d_const_size_C == 16) {
                find_constellation_value_lC = &symbol_demapper_impl::find_constellation_value_16qam; 
			} else if (d_const_size_C == 64) {
                find_constellation_value_lC = &symbol_demapper_impl::find_constellation_value_64qam; 
			} else {
				std::cout << "symbol_demapper: error in d_const_size_C\n";
			}

            d_carriers_per_segment = d_data_carriers_mode1 * 
                ((int)pow(2.0,mode-1)); 
            
            //It is not possible to declare an output signature with size=0. Thus, segments_* have to be
            //bigger than zero. Thus, segments_A+segments_B+segments_C may be more than total_segments (13). 
            d_noutput = d_total_segments*d_carriers_per_segment;
		   	d_noutput_A = segments_A*d_carriers_per_segment;
		   	d_noutput_B = segments_B*d_carriers_per_segment;
		   	d_noutput_C = segments_C*d_carriers_per_segment;

	
            
        }

        /*
         * Our virtual destructor.
         */
        symbol_demapper_impl::~symbol_demapper_impl()
        {
        }

        unsigned int 
            symbol_demapper_impl::find_constellation_value_qpsk(gr_complex val)
            {
                return (val.real()<0)<<1 | (val.imag()<0); 
            }

        unsigned int 
            symbol_demapper_impl::find_constellation_value_16qam(gr_complex val)
            {
                return ((val.real()<0)<<3) | ((val.real()<d_th_16qam & val.real()>-d_th_16qam)<<1) | ((val.imag()<0)<<2) | ((val.imag()<d_th_16qam & val.imag()>-d_th_16qam)); 

            }

        unsigned int 
            symbol_demapper_impl::find_constellation_value_64qam(gr_complex val)
            {
                return ((val.real()<0)<<5) | ((val.real()<2*d_th_64qam & val.real()>-2*d_th_64qam)<<3) | (( (val.real()>d_th_64qam & val.real()<3*d_th_64qam)|(val.real()<-d_th_64qam & val.real()>-3*d_th_64qam) )<<1) \
                    | ((val.imag()<0)<<4) | ((val.imag()<2*d_th_64qam & val.imag()>-2*d_th_64qam)<<2) | (( (val.imag()>d_th_64qam & val.imag()<3*d_th_64qam)|(val.imag()<-d_th_64qam & val.imag()>-3*d_th_64qam) )) ; 
            }

        int
            symbol_demapper_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const gr_complex *in = (const gr_complex *) input_items[0];
				unsigned char *out_A = (unsigned char *) output_items[0];
				unsigned char *out_B = (unsigned char *) output_items[1];
				unsigned char *out_C = (unsigned char *) output_items[2];
				//printf("%d\t %d\t %d\n",  d_nsegments_A*d_carriers_per_segment, (d_nsegments_A + d_nsegments_B)*d_carriers_per_segment, (d_nsegments_A + d_nsegments_B+ d_nsegments_C)*d_carriers_per_segment);

                bool out_B_connected = output_items.size()>=2;
                bool out_C_connected = output_items.size()>=3;		
				//printf("output_items.size() = %lu\n", output_items.size());		
                int a, b, c;
				//printf("---SYMBOL DEMAPPER-> noutput_items = %d\n",noutput_items);
				for (int i = 0; i < noutput_items; i++)
                {
                    for (int carrier = 0; carrier<d_noutput_A; carrier++){
						out_A[i*d_noutput_A+carrier] = (this->*find_constellation_value_lA)(in[i*d_noutput+carrier]);
					}
                    for (int carrier = d_noutput_A; carrier<d_noutput_A+d_noutput_B; carrier++){
						out_B[i*d_noutput_B+carrier-d_noutput_A] = (this->*find_constellation_value_lB)(in[i*d_noutput+carrier]);
					}
                    for (int carrier = d_noutput_A+d_noutput_B; carrier<d_noutput; carrier++){
						out_C[i*d_noutput_C+carrier-(d_noutput_A+d_noutput_B)] = (this->*find_constellation_value_lC)(in[i*d_noutput+carrier]);
					}

                }

                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

