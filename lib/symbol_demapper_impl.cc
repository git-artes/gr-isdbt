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

        const gr_complex symbol_demapper_impl::d_constellation_qpsk[] = {
            gr_complex(+1/sqrt(2),+1/sqrt(2)),
            gr_complex(+1/sqrt(2),-1/sqrt(2)), 
            gr_complex(-1/sqrt(2),+1/sqrt(2)),
            gr_complex(-1/sqrt(2),-1/sqrt(2))
        }; 
        
        const gr_complex symbol_demapper_impl::d_constellation_16qam[] = {
            gr_complex(3/sqrt(10),3/sqrt(10)),
            gr_complex(3/sqrt(10),1/sqrt(10)), 
            gr_complex(1/sqrt(10),3/sqrt(10)),
            gr_complex(1/sqrt(10),1/sqrt(10)),

            gr_complex(3/sqrt(10),-3/sqrt(10)),
            gr_complex(3/sqrt(10),-1/sqrt(10)), 
            gr_complex(1/sqrt(10),-3/sqrt(10)),
            gr_complex(1/sqrt(10),-1/sqrt(10)),

            gr_complex(-3/sqrt(10),3/sqrt(10)),
            gr_complex(-3/sqrt(10),1/sqrt(10)), 
            gr_complex(-1/sqrt(10),3/sqrt(10)),
            gr_complex(-1/sqrt(10),1/sqrt(10)),
            
            gr_complex(-3/sqrt(10),-3/sqrt(10)),
            gr_complex(-3/sqrt(10),-1/sqrt(10)), 
            gr_complex(-1/sqrt(10),-3/sqrt(10)),
            gr_complex(-1/sqrt(10),-1/sqrt(10))
        }; 
        
        const gr_complex symbol_demapper_impl::d_constellation_64qam[] = {
            gr_complex(7/sqrt(42),7/sqrt(42)),
            gr_complex(7/sqrt(42),5/sqrt(42)), 
            gr_complex(5/sqrt(42),7/sqrt(42)),
            gr_complex(5/sqrt(42),5/sqrt(42)),

            gr_complex(7/sqrt(42),1/sqrt(42)),
            gr_complex(7/sqrt(42),3/sqrt(42)), 
            gr_complex(5/sqrt(42),1/sqrt(42)),
            gr_complex(5/sqrt(42),3/sqrt(42)),

            gr_complex(1/sqrt(42),7/sqrt(42)),
            gr_complex(1/sqrt(42),5/sqrt(42)), 
            gr_complex(3/sqrt(42),7/sqrt(42)),
            gr_complex(3/sqrt(42),5/sqrt(42)),

            gr_complex(1/sqrt(42),1/sqrt(42)),
            gr_complex(1/sqrt(42),3/sqrt(42)), 
            gr_complex(3/sqrt(42),1/sqrt(42)),
            gr_complex(3/sqrt(42),3/sqrt(42)),

            //////////////
            gr_complex(7/sqrt(42),-7/sqrt(42)),
            gr_complex(7/sqrt(42),-5/sqrt(42)), 
            gr_complex(5/sqrt(42),-7/sqrt(42)),
            gr_complex(5/sqrt(42),-5/sqrt(42)),

            gr_complex(7/sqrt(42),-1/sqrt(42)),
            gr_complex(7/sqrt(42),-3/sqrt(42)), 
            gr_complex(5/sqrt(42),-1/sqrt(42)),
            gr_complex(5/sqrt(42),-3/sqrt(42)),

            gr_complex(1/sqrt(42),-7/sqrt(42)),
            gr_complex(1/sqrt(42),-5/sqrt(42)), 
            gr_complex(3/sqrt(42),-7/sqrt(42)),
            gr_complex(3/sqrt(42),-5/sqrt(42)),

            gr_complex(1/sqrt(42),-1/sqrt(42)),
            gr_complex(1/sqrt(42),-3/sqrt(42)), 
            gr_complex(3/sqrt(42),-1/sqrt(42)),
            gr_complex(3/sqrt(42),-3/sqrt(42)),

            //////////////
            gr_complex(-7/sqrt(42),7/sqrt(42)),
            gr_complex(-7/sqrt(42),5/sqrt(42)), 
            gr_complex(-5/sqrt(42),7/sqrt(42)),
            gr_complex(-5/sqrt(42),5/sqrt(42)),

            gr_complex(-7/sqrt(42),1/sqrt(42)),
            gr_complex(-7/sqrt(42),3/sqrt(42)), 
            gr_complex(-5/sqrt(42),1/sqrt(42)),
            gr_complex(-5/sqrt(42),3/sqrt(42)),

            gr_complex(-1/sqrt(42),7/sqrt(42)),
            gr_complex(-1/sqrt(42),5/sqrt(42)), 
            gr_complex(-3/sqrt(42),7/sqrt(42)),
            gr_complex(-3/sqrt(42),5/sqrt(42)),

            gr_complex(-1/sqrt(42),1/sqrt(42)),
            gr_complex(-1/sqrt(42),3/sqrt(42)), 
            gr_complex(-3/sqrt(42),1/sqrt(42)),
            gr_complex(-3/sqrt(42),3/sqrt(42)),

            //////////////
            gr_complex(-7/sqrt(42),-7/sqrt(42)),
            gr_complex(-7/sqrt(42),-5/sqrt(42)), 
            gr_complex(-5/sqrt(42),-7/sqrt(42)),
            gr_complex(-5/sqrt(42),-5/sqrt(42)),

            gr_complex(-7/sqrt(42),-1/sqrt(42)),
            gr_complex(-7/sqrt(42),-3/sqrt(42)), 
            gr_complex(-5/sqrt(42),-1/sqrt(42)),
            gr_complex(-5/sqrt(42),-3/sqrt(42)),

            gr_complex(-1/sqrt(42),-7/sqrt(42)),
            gr_complex(-1/sqrt(42),-5/sqrt(42)), 
            gr_complex(-3/sqrt(42),-7/sqrt(42)),
            gr_complex(-3/sqrt(42),-5/sqrt(42)),

            gr_complex(-1/sqrt(42),-1/sqrt(42)),
            gr_complex(-1/sqrt(42),-3/sqrt(42)), 
            gr_complex(-3/sqrt(42),-1/sqrt(42)),
            gr_complex(-3/sqrt(42),-3/sqrt(42)),
        }; 

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
                d_constellation_A = d_constellation_qpsk;
            } else if (d_const_size_A==16) {
                d_constellation_A = d_constellation_16qam; 
            } else if (d_const_size_A==64) {
                d_constellation_A = d_constellation_64qam; 
            } else {
            std::cout << "symbol_demapper: error in d_const_size_A\n"; 
            }

            if (d_const_size_B==4) {
	            d_constellation_B = d_constellation_qpsk;
	        } else if (d_const_size_B==16) {
		        d_constellation_B = d_constellation_16qam;
		    } else if (d_const_size_B==64) {
		    	d_constellation_B = d_constellation_64qam;
			} else {
				std::cout << "symbol_demapper: error in d_const_size_B\n";
			}

            if (d_const_size_C == 4) {
		    	d_constellation_C = d_constellation_qpsk;
			} else if (d_const_size_C == 16) {
				d_constellation_C = d_constellation_16qam;
			} else if (d_const_size_C == 64) {
				d_constellation_C = d_constellation_64qam;
			} else {
				std::cout << "symbol_demapper: error in d_const_size_C\n";
			}

            d_carriers_per_segment = d_data_carriers_mode1 * 
                ((int)pow(2.0,mode-1)); 
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

        int 
            symbol_demapper_impl::find_constellation_value_A(gr_complex val)
            {
                float min_dist = std::norm(val - d_constellation_A[0]);
                int min_index = 0;

                for (int i = 0; i < d_const_size_A; i++)
                {
                    float dist = std::norm(val - d_constellation_A[i]);
                    if (dist < min_dist)
                    {
                        min_dist = dist;
                        min_index = i;
                    }
                }

                return min_index;
            }



        int
			symbol_demapper_impl::find_constellation_value_B(gr_complex val)
			{
				float min_dist = std::norm(val - d_constellation_B[0]);
				int min_index = 0;

				for (int i = 0; i < d_const_size_B; i++)
				{
					float dist = std::norm(val - d_constellation_B[i]);
					if (dist < min_dist)
					{
						min_dist = dist;
						min_index = i;
					}
				}
			return min_index;
			
			}

        int
			symbol_demapper_impl::find_constellation_value_C(gr_complex val)
			{
				float min_dist = std::norm(val - d_constellation_C[0]);
				int min_index = 0;

				for (int i = 0; i < d_const_size_C; i++)
				{
					float dist = std::norm(val - d_constellation_C[i]);
					if (dist < min_dist)
					{
							min_dist = dist;
							min_index = i;
					}
				}
			return min_index;				
			}

        int
            symbol_demapper_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const gr_complex *in = (const gr_complex *) input_items[0];
                //unsigned char *out = (unsigned char *) output_items[0];
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
					a = 0;
					b = 0;
					c = 0;	
                    for (int carrier = 0; carrier<d_noutput; carrier++){
                        // leave it like this for now to easily accomodate the 
                        // per-segment constellation possibility later on. 
                        if (carrier< d_nsegments_A*d_carriers_per_segment){ 
								out_A[i*d_noutput_A+a] = find_constellation_value_A(in[i*d_noutput+carrier]);
								a++;
								//printf("carrier A = %d\t value A = %d\n", carrier, find_constellation_value_A(in[i*d_noutput+carrier]));
						}
						else if ((carrier< (d_nsegments_A + d_nsegments_B)*d_carriers_per_segment)&&(out_B_connected)){
                                out_B[i*d_noutput_B+b] = find_constellation_value_B(in[i*d_noutput+carrier]);
								b++;
								//printf("carrier B = %d\n", carrier);
						}
						else if ((carrier< (d_nsegments_A + d_nsegments_B + d_nsegments_C)*d_carriers_per_segment)&&(out_C_connected)){
                                out_C[i*d_noutput_C+c] = find_constellation_value_C(in[i*d_noutput+carrier]);
								c++;
								//printf("carrier C = %d\n", carrier);
						}

                    }
                }

                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

