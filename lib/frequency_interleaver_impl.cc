/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017, 2018, 2019, 2020, 2021
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
#include "frequency_interleaver_impl.h"

namespace gr {
  namespace isdbt {


        // TODO shouldn't things like these be defined elsewhere? 
        const int frequency_interleaver_impl::d_data_carriers_mode1 = 96; 
        const int frequency_interleaver_impl::d_total_segments = 13; 


        const int frequency_interleaver_impl::d_random_perm_mode1[] = 
        {80, 93, 63, 92, 94, 55, 17, 81, 6, 51, 9, 85, 89, 65, 52, 15, 73, 66, 46, 71, 12, 70, 18, 13, 
            95, 34, 1, 38, 78, 59, 91, 64, 0, 28, 11, 4, 45, 35, 16, 7, 48, 22, 23, 77, 56, 19, 8, 36, 
            39, 61, 21, 3, 26, 69, 67, 20, 74, 86, 72, 25, 31, 5, 49, 42, 54, 87, 43, 60, 29, 2, 76, 84, 
            83, 40, 14, 79, 27, 57, 44, 37, 30, 68, 47, 88, 75, 41, 90, 10, 33, 32, 62, 50, 58, 82, 53, 24}; 

        const int frequency_interleaver_impl::d_random_perm_mode2[] = 
        {98, 35, 67, 116, 135, 17, 5, 93, 73, 168, 54, 143, 43, 74, 165, 48, 37, 69, 154, 150, 107, 76, 176, 79, 
            175, 36, 28, 78, 47, 128, 94, 163, 184, 72, 142, 2, 86, 14, 130, 151, 114, 68, 46, 183, 122, 112, 180, 42, 
            105, 97, 33, 134, 177, 84, 170, 45, 187, 38, 167, 10, 189, 51, 117, 156, 161, 25, 89, 125, 139, 24, 19, 57, 
            71, 39, 77, 191, 88, 85, 0, 162, 181, 113, 140, 61, 75, 82, 101, 174, 118, 20, 136, 3, 121, 190, 120, 92, 
            160, 52, 153, 127, 65, 60, 133, 147, 131, 87, 22, 58, 100, 111, 141, 83, 49, 132, 12, 155, 146, 102, 164, 66, 
            1, 62, 178, 15, 182, 96, 80, 119, 23, 6, 166, 56, 99, 123, 138, 137, 21, 145, 185, 18, 70, 129, 95, 90, 
            149, 109, 124, 50, 11, 152, 4, 31, 172, 40, 13, 32, 55, 159, 41, 8, 7, 144, 16, 26, 173, 81, 44, 103, 
            64, 9, 30, 157, 126, 179, 148, 63, 188, 171, 106, 104, 158, 115, 34, 186, 29, 108, 53, 91, 169, 110, 27, 59};


        const int frequency_interleaver_impl::d_random_perm_mode3[] =
        {62, 13, 371, 11, 285, 336, 365, 220, 226, 92, 56, 46, 120, 175, 298, 352, 172, 235, 53, 164, 368, 187, 125, 82, 
            5, 45, 173, 258, 135, 182, 141, 273, 126, 264, 286, 88, 233, 61, 249, 367, 310, 179, 155, 57, 123, 208, 14, 227, 
            100, 311, 205, 79, 184, 185, 328, 77, 115, 277, 112, 20, 199, 178, 143, 152, 215, 204, 139, 234, 358, 192, 309, 183, 
            81, 129, 256, 314, 101, 43, 97, 324, 142, 157, 90, 214, 102, 29, 303, 363, 261, 31, 22, 52, 305, 301, 293, 177, 
            116, 296, 85, 196, 191, 114, 58, 198, 16, 167, 145, 119, 245, 113, 295, 193, 232, 17, 108, 283, 246, 64, 237, 189, 
            128, 373, 302, 320, 239, 335, 356, 39, 347, 351, 73, 158, 276, 243, 99, 38, 287, 3, 330, 153, 315, 117, 289, 213, 
            210, 149, 383, 337, 339, 151, 241, 321, 217, 30, 334, 161, 322, 49, 176, 359, 12, 346, 60, 28, 229, 265, 288, 225, 
            382, 59, 181, 170, 319, 341, 86, 251, 133, 344, 361, 109, 44, 369, 268, 257, 323, 55, 317, 381, 121, 360, 260, 275, 
            190, 19, 63, 18, 248, 9, 240, 211, 150, 230, 332, 231, 71, 255, 350, 355, 83, 87, 154, 218, 138, 269, 348, 130, 
            160, 278, 377, 216, 236, 308, 223, 254, 25, 98, 300, 201, 137, 219, 36, 325, 124, 66, 353, 169, 21, 35, 107, 50, 
            106, 333, 326, 262, 252, 271, 263, 372, 136, 0, 366, 206, 159, 122, 188, 6, 284, 96, 26, 200, 197, 186, 345, 340, 
            349, 103, 84, 228, 212, 2, 67, 318, 1, 74, 342, 166, 194, 33, 68, 267, 111, 118, 140, 195, 105, 202, 291, 259, 
            23, 171, 65, 281, 24, 165, 8, 94, 222, 331, 34, 238, 364, 376, 266, 89, 80, 253, 163, 280, 247, 4, 362, 379, 
            290, 279, 54, 78, 180, 72, 316, 282, 131, 207, 343, 370, 306, 221, 132, 7, 148, 299, 168, 224, 48, 47, 357, 313, 
            75, 104, 70, 147, 40, 110, 374, 69, 146, 37, 375, 354, 174, 41, 32, 304, 307, 312, 15, 272, 134, 242, 203, 209, 
            380, 162, 297, 327, 10, 93, 42, 250, 156, 338, 292, 144, 378, 294, 329, 127, 270, 76, 95, 91, 244, 274, 27, 51}; 


        frequency_interleaver::sptr
            frequency_interleaver::make(bool oneseg, int mode)
            {
                return gnuradio::get_initial_sptr
                    (new frequency_interleaver_impl(oneseg, mode));
            }

        /*
         * The private constructor
         */
        frequency_interleaver_impl::frequency_interleaver_impl(bool oneseg, int mode)
            : gr::sync_block("frequency_interleaver",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*d_total_segments*d_data_carriers_mode1*((int)pow(2.0,mode-1))),
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*d_total_segments*d_data_carriers_mode1*((int)pow(2.0,mode-1))))
        {
            d_mode = mode; 
            d_1seg = oneseg; 
            d_carriers_per_segment = d_data_carriers_mode1*((int)pow(2.0,mode-1)); 
            d_total_carriers = d_total_segments*d_carriers_per_segment; 

            /*Initialize the inter-segment interleaved vector*/
            d_interleaved = new gr_complex [d_total_carriers]; 
            /*Initialize the rotated vector*/
            d_rotated = new gr_complex [d_total_carriers]; 

            if (d_mode==1){
                d_random_perm = d_random_perm_mode1; 
            } else if (d_mode==2){
                d_random_perm = d_random_perm_mode2; 
            } else if (d_mode==3){
                d_random_perm = d_random_perm_mode3; 
            } else {
                printf("ISDB-t frequency interleaver: mode not correctly specified (it should be either 1, 2 or 3). "); 
            }

        }

         gr_complex * frequency_interleaver_impl::randomize(const gr_complex * not_random, gr_complex * random){
            for (int segment = 0; segment<d_total_segments; segment++) 
            {
                for(int carrier = 0; carrier<d_carriers_per_segment; carrier++) 
                {
                    //not_random[carrier+segment*d_carriers_per_segment] = random[d_random_perm[carrier]+segment*d_carriers_per_segment]; 
                    random[d_random_perm[carrier]+segment*d_carriers_per_segment] = not_random[carrier+segment*d_carriers_per_segment];
                }

            }
            return random; 
        }

        gr_complex * frequency_interleaver_impl::rotate(const gr_complex * derotated, gr_complex * rotated){
            // I begin with the first segment even if its not rotated, since I would have to copy it anyway. I could save a multiplication but I believe is minimal. 
            for (int segment = 0; segment<d_total_segments; segment++) 
            {
                for(int carrier = 0; carrier<d_carriers_per_segment; carrier++) 
                {
                    // since the outcome of mod of a negative number is not 100% decided upon (apparently from what I found on the internet), I took this more 
                    // indirect but unambiguous approach, where I add d_carriers_per_segments to carrier-segment in order to shift everything to the right. 
                    //derotated[carrier+segment*d_carriers_per_segment] = rotated[((carrier+d_carriers_per_segment-segment) % d_carriers_per_segment) + segment*d_carriers_per_segment];
                    rotated[((carrier+d_carriers_per_segment-segment) % d_carriers_per_segment) + segment*d_carriers_per_segment] = derotated[carrier+segment*d_carriers_per_segment]; 
				   
                }

            }
            return rotated; 
        }
        
        gr_complex * frequency_interleaver_impl::intersegment_interleave(const gr_complex * deinterleaved, gr_complex * interleaved){
            // The number of segments to enter the deinterleaver. 
            int n = d_total_segments; 
            // in case of 1-seg, the corresponding segment is not interleaved so I will simply copy it in the output. 
            if (d_1seg){
                n = d_total_segments-1; 
                for (int carrier = 0; carrier<d_carriers_per_segment; carrier++){
                    //deinterleaved[carrier] = interleaved[carrier]; 
                    interleaved[carrier] = deinterleaved[carrier]; 
                }
            }
            
            //output_carrier will begin at 0 or at d_carriers_per_segment depending on whether there is 
            //1-seg or not. It will then go up to d_carriers_per_segment*d_total_segments-1 
            int output_carrier = 0 + (d_total_segments-n)*d_carriers_per_segment; 
            for(int carrier = 0; carrier<d_carriers_per_segment; carrier++) 
            {
                // if 1-seg was present, we skip the first segment
                for (int segment = d_total_segments-n; segment<d_total_segments; segment++) 
                {
                    // we will look for the output by going through each of the n segments to interleave, and 
                    // offseting by a certain number of carriers. For instance, when carrier=0, output_carrier
                    // goes from 0 to n-1 (if 1-seg is not present), and we look for these outputs by taking 
                    // the first carrier of each fo the n segments. 
                    //deinterleaved[output_carrier] = interleaved[segment*d_carriers_per_segment + carrier]; 
                    interleaved[segment*d_carriers_per_segment + carrier] = deinterleaved[output_carrier];  
                    output_carrier++; 
                }

            }
            return interleaved; 
        }


        /*
         * Our virtual destructor.
         */
        frequency_interleaver_impl::~frequency_interleaver_impl()
        {
        }

        int
            frequency_interleaver_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const gr_complex *in = (const gr_complex *) input_items[0];
                gr_complex *out = (gr_complex *) output_items[0];

                for (int i = 0; i < noutput_items; i++)
                {
                    intersegment_interleave(&in[i*d_total_carriers], d_interleaved); 
                    rotate(d_interleaved, d_rotated); 
                    randomize(d_rotated, &out[i*d_total_carriers]); 
                }


                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */


