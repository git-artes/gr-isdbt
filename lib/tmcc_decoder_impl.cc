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

namespace gr {
  namespace isdbt {

  // The segments positions
  const int d_segments_positions[13] = {11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12};

  // The tmcc carriers positions
    const int d_tmcc_carriers[52] = {
    	70,133, 233, 410, \
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
		5245, 5377, 5501, 5531
    };

    // The tmcc carriers positions
    const int d_ac_carriers[104] = {
    	  10,   28,  161,  191,  277,  316,  335,  425,\
		 452,  472,  614,  640,  683,  727,  832,  853,\
    	 868,  953, 1012, 1061, 1088, 1144, 1195, 1277,\
    	1394, 1397, 1414, 1432, 1565, 1595, 1681, 1720,\
    	1739, 1829, 1856, 1876, 2018, 2044, 2087, 2131,\
    	2236, 2257, 2272, 2357, 2416, 2465, 2492, 2548,\
    	2599, 2681, 2798, 2801, 2818, 2836, 2969, 2999,\
    	3085, 3124, 3143, 3233, 3260, 3280, 3422, 3448,\
    	3491, 3535, 3640, 3661, 3676, 3761, 3820, 3869,\
    	3896, 3952, 4003, 4085, 4202, 4205, 4222, 4240,\
    	4373, 4403, 4489, 4528, 4547, 4637, 4664, 4684,\
    	4826, 4852, 4895, 4939, 5044, 5065, 5080, 5165,\
    	5224, 5273, 5300, 5356, 5407, 5489, 5606, 5609
    };

    // TPS sync sequence for odd and even frames
    const int d_tmcc_sync_size = 16; // TODO
    const int d_tmcc_sync_even[d_tmcc_sync_size] = {
      0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0
    };
    const int d_tmcc_sync_odd[d_tmcc_sync_size] = {
      1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1
    };


    /*
     * ---------------------------------------------------------------------
     *
    void
	tmcc_decoder_impl::generate_prbs() // En dvb-t el prbs es idéntico que en isdb-t
    {
      // init PRBS register with 1s
      unsigned int reg_prbs = (1 << 11) - 1; // Inicializamos reg_prbs en 111111111111

      for (int k = 0; k < (active_carriers); k++)
      {
        d_wk[k] = (char)(reg_prbs & 0x01); // Sale el bit menos significativo en forma de caracter
        int new_bit = ((reg_prbs >> 2) ^ (reg_prbs >> 0)) & 0x01; // Este es el bit que entra a la izquierda del registro
        reg_prbs = (reg_prbs >> 1) | (new_bit << 10); // Movemos todo el registro para la derecha y agregamos el new_bit a la izquierda.

        //printf("prbs[%d] = %d\n",k,d_wk[k]);
      }
    }
     *
     * ---------------------------------------------------------------------
     */

    int
	tmcc_decoder_impl::process_tmcc_data(const gr_complex * in)
    {
    	/*
    	 * The aim of this method is to detect the end of a frame
    	 */

        int end_frame = 0;

        d_symbol_index = (d_symbol_index+ 1) % 204;

        // As we have 52 TMCC carriers we will using majority voting for decision
        int tmcc_majority_zero = 0;

        for (int k = 0; k < tmcc_carriers_size; k++)
        {
            gr_complex phdiff = in[d_tmcc_carriers[k]] * conj(d_prev_tmcc_symbol[k]);
            //gr_complex phdiff = in[k] * conj(d_prev_tmcc_symbol[k]);


            if (phdiff.real() >= 0.0)
            	tmcc_majority_zero++;
            else
            	tmcc_majority_zero--;


           // if ((tmcc_majority_zero<0) && (phdiff.real()>0))  printf("TMMC Carriers: %f + j%f\n PREVOUS: %f + j%f\n",in[tmcc_carriers[k]].real(), in[tmcc_carriers[k]].imag(),d_prev_tmcc_symbol[k].real(), d_prev_tmcc_symbol[k].imag());

            //d_prev_tmcc_symbol[k] = in[tmcc_carriers[k]];
            d_prev_tmcc_symbol[k] = in[d_tmcc_carriers[k]];


        }
      //  printf("\n -----------------------\n");


       // printf("tmcc_majority_zero: %i\n", tmcc_majority_zero);
       // printf("Number symbol: %i\n", number_symbol++);

        // Take out the front entry first
        //if (d_rcv_tmcc_data.size() >= d_symbols_per_frame) d_rcv_tmcc_data.pop_front();
        d_rcv_tmcc_data.pop_front();

        // Add data at tail
        if (tmcc_majority_zero >= 0)
        	d_rcv_tmcc_data.push_back(0);
        else
        	d_rcv_tmcc_data.push_back(1);

        //printf("d_rcv_tmcc_data.size: %i\n", d_rcv_tmcc_data.size());
        //printf("d_rcv_tmcc_data.back: %i\n", d_rcv_tmcc_data.back());
        //printf("d_tmcc_sync_evenv.size(): %i\n", d_tmcc_sync_evenv.size());

        //for (int i = 0; i < d_symbols_per_frame; i++)
        	//printf("%i", d_rcv_tmcc_data[i]);
        //printf("\n");

        // Match synchronization signatures

        if (std::equal(d_rcv_tmcc_data.begin() + 1, d_rcv_tmcc_data.begin() + d_tmcc_sync_size, d_tmcc_sync_evenv.begin()))
        {
            d_symbol_index_known = 1;
            end_frame = 1;
            d_symbol_index = 203;
/*
            printf("\n Even: ");
            for (int i = 0; i < d_symbols_per_frame; i++)
            	printf("%i", d_rcv_tmcc_data[i]);
            printf("\n");
*/
        }
        else if (std::equal(d_rcv_tmcc_data.begin() + 1, d_rcv_tmcc_data.begin() + d_tmcc_sync_size, d_tmcc_sync_oddv.begin()))
        {

            d_symbol_index_known = 1;
            end_frame = 1;
            d_symbol_index = 203;

/*
            printf("\n Odd: ");
            for (int i = 0; i < d_symbols_per_frame; i++)
            	printf("%i", d_rcv_tmcc_data[i]);
            printf("\n");
*/
        }

        //printf("Symbol Index: %i\n", d_symbol_index);

        // We return end_frame
        return end_frame;

    }
    /*
     * ---------------------------------------------------------------------
     */

    int
	tmcc_decoder_impl::is_sp_carrier(int carrier)
    {
    	for (int i=0;i<36;i++)
    		if (carrier == (12*i+3*(d_symbol_index % 4)))
    			return 1;
    	return 0;
    }
    /*
     * ---------------------------------------------------------------------
     */
    tmcc_decoder::sptr
    tmcc_decoder::make()
    {
      return gnuradio::get_initial_sptr
        (new tmcc_decoder_impl());
    }

    /*
     * The private constructor
     */
    tmcc_decoder_impl::tmcc_decoder_impl()
      : gr::block("tmcc_decoder",
              gr::io_signature::make(1, 1, sizeof(gr_complex)*5617),
			  gr::io_signature::make(1, 1, sizeof(gr_complex)*4992))
    {
        // Number of tmcc pilots
        tmcc_carriers_size=52;

        // Number of active carriers
        active_carriers=5617;

        // Number of segments
        d_number_of_segments = 13;

        // Number of carriers per segment
        d_carriers_per_segment = (active_carriers-1)/d_number_of_segments;

        d_data_carriers_per_segment = 384;

        // Number of symbols per frame
        d_symbols_per_frame=204;

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


        // We initialize attribute d_symbol_index_known
        d_symbol_index_known = 0;

        number_symbol = 0;
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
		// Process TMCC data
		//If a frame is recognized then signal end of frame
		int frame_end = process_tmcc_data(&in[i * active_carriers]);

		int carrier_out = 0;
		int d_spilot_index = 0;
		int d_acpilot_index = 0;
		int d_tmccpilot_index = 0;

		for (int carrier = 0; carrier < (active_carriers - 1);carrier++) {

			// We see if the current carrier is an scattered pilot
			if (carrier == (12 * d_spilot_index + 3 * (d_symbol_index % 4))) {
				d_spilot_index++;

			} else {
				// We see if the current carrier is an AC pilot
				if ((carrier == d_ac_carriers[d_acpilot_index])) // && (carrier != 377) && (carrier != 244) && (carrier != 226) && (carrier != 209) && (carrier != 206) && (carrier != 89) && (carrier != 7) && ((carrier+432*6) != tmcc_carriers[6*4]) && ((carrier+432*6) != tmcc_carriers[6*4+1]) && ((carrier+432*6) != tmcc_carriers[6*4+2]) && ((carrier+432*6) != tmcc_carriers[6*4+3]))
				{
					d_acpilot_index++;

				} else {
					// We see if the current carrier is a tmcc pilot
					if ((carrier == d_tmcc_carriers[d_tmccpilot_index])) {
						d_tmccpilot_index++;

					} else {
						// If is none of then we let the carrier out in the proper order
						//out[carrier_out] = in[i * 5617 + carrier];
						out[(d_segments_positions[carrier_out/d_data_carriers_per_segment]*d_data_carriers_per_segment) + (carrier_out % d_data_carriers_per_segment)] = in[i * active_carriers + carrier];
						carrier_out++;
						//if (carrier_out == d_data_carriers_per_segment) carrier_out = 0;
					}
				}
			}

		}

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

