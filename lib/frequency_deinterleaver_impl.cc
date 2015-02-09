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
#include "frequency_deinterleaver_impl.h"

namespace gr {
    namespace isdbt {


        // TODO shouldn't things like these be defined elsewhere? 
        const int d_data_carriers_mode1 = 96; 
        const int d_total_segments = 13; 

        const int frequency_deinterleaver_impl::d_random_perm_mode1[] = 
        { 80, 93, 63, 92, 94, 55, 17, 81, 6, 51, 9, 85, 89, 65, 52, 15, 73, 66, 46, 71, 12, 70, 18, 13,\
            95, 34, 1, 38, 78, 59, 91, 64, 0, 28, 11, 4, 45, 35, 16, 7, 48, 22, 23, 77, 56, 19, 8, 36,\
                39, 61, 21, 3, 26, 69, 67, 20, 74, 86, 72, 25, 31, 5, 49, 42, 54, 87, 43, 60, 29, 2, 76, 84,\
                83, 40, 14, 79, 27, 57, 44, 37, 30, 68, 47, 88, 75, 41, 90, 10, 33, 32, 62, 50, 58, 82, 53, 24
        }; 

        const int frequency_deinterleaver_impl::d_random_perm_mode2[] = 
        {98, 35, 67, 116, 135, 17, 5, 93, 73, 168, 54, 143, 43, 74, 165, 48, 37, 69, 154, 150, 107, 76, 176, 79,\
            175, 36, 28, 78, 47, 128, 94, 163, 184, 72, 142, 2, 86, 14, 130, 151, 114, 68, 46, 183, 122, 112, 180, 42,\
                105, 97, 33, 134, 177, 84, 170, 45, 187, 38, 167, 10, 189, 51, 117, 156, 161, 25, 89, 125, 139, 24, 19, 57,\
                71, 39, 77, 191, 88, 85, 0, 162, 181, 113, 140, 61, 75, 82, 101, 174, 118, 20, 136, 3, 121, 190, 120, 92,\
                160, 52, 153, 127, 65, 60, 133, 147, 131, 87, 22, 58, 100, 111, 141, 83, 49, 132, 12, 155, 146, 102, 164, 66,\
                1, 62, 178, 15, 182, 96, 80, 119, 23, 6, 166, 56, 99, 123, 138, 137, 21, 145, 185, 18, 70, 129, 95, 90,\
                149, 109, 124, 50, 11, 152, 4, 31, 172, 40, 13, 32, 55, 159, 41, 8, 7, 144, 16, 26, 173, 81, 44, 103,\
                64, 9, 30, 157, 126, 179, 148, 63, 188, 171, 106, 104, 158, 115, 34, 186, 29, 108, 53, 91, 169, 110, 27, 59
        };

        const int frequency_deinterleaver_impl::d_random_perm_mode3[] = 
        {62, 13, 371, 11, 285, 336, 365, 220, 226, 92, 56, 46, 120, 175, 298, 352, 172, 235, 53, 164, 368, 187, 125, 82,\
            5, 45, 173, 258, 135, 182, 141, 273, 126, 264, 286, 88, 233, 61, 249, 367, 310, 179, 155, 57, 123, 208, 14, 227,\
                100, 311, 205, 79, 184, 185, 328, 77, 115, 277, 112, 20, 199, 178, 143, 152, 215, 204, 139, 234, 358, 192, 309, 183,\
                81, 129, 256, 314, 101, 43, 97, 324, 142, 157, 90, 214, 102, 29, 303, 363, 261, 31, 22, 52, 305, 301, 293, 177,\
                116, 296, 85, 196, 191, 114, 58, 198, 16, 167, 145, 119, 245, 113, 295, 193, 232, 17, 108, 283, 246, 64, 237, 189,\
                128, 373, 302, 320, 239, 335, 356, 39, 347, 351, 73, 158, 276, 243, 99, 38, 287, 3, 330, 153, 315, 117, 289, 213,\
                210, 149, 383, 337, 339, 151, 241, 321, 217, 30, 334, 161, 322, 49, 176, 359, 12, 346, 60, 28, 229, 265, 288, 225,\
                382, 59, 181, 170, 319, 341, 86, 251, 133, 344, 361, 109, 44, 369, 268, 257, 323, 55, 317, 381, 121, 360, 260, 275,\
                190, 19, 63, 18, 248, 9, 240, 211, 150, 230, 332, 231, 71, 255, 350, 355, 83, 87, 154, 218, 138, 269, 348, 130,\
                160, 278, 377, 216, 236, 308, 223, 254, 25, 98, 300, 201, 137, 219, 36, 325, 124, 66, 353, 169, 21, 35, 107, 50,\
                106, 333, 326, 262, 252, 271, 263, 372, 136, 0, 366, 206, 159, 122, 188, 6, 284, 96, 26, 200, 197, 186, 345, 340,\
                349, 103, 84, 228, 212, 2, 67, 318, 1, 74, 342, 166, 194, 33, 68, 267, 111, 118, 140, 195, 105, 202, 291, 259,\
                23, 171, 65, 281, 24, 165, 8, 94, 222, 331, 34, 238, 364, 376, 266, 89, 80, 253, 163, 280, 247, 4, 362, 379,\
                290, 279, 54, 78, 180, 72, 316, 282, 131, 207, 343, 370, 306, 221, 132, 7, 148, 299, 168, 224, 48, 47, 357, 313,\
                75, 104, 70, 147, 40, 110, 374, 69, 146, 37, 375, 354, 174, 41, 32, 304, 307, 312, 15, 272, 134, 242, 203, 209,\
                380, 162, 297, 327, 10, 93, 42, 250, 156, 338, 292, 144, 378, 294, 329, 127, 270, 76, 95, 91, 244, 274, 27, 51
        };

        frequency_deinterleaver::sptr
            frequency_deinterleaver::make(bool oneseg, int mode)
            {
                return gnuradio::get_initial_sptr
                    (new frequency_deinterleaver_impl(oneseg, mode));
            }

        /*
         * The private constructor
         */
        frequency_deinterleaver_impl::frequency_deinterleaver_impl(bool oneseg, int mode)
            : gr::sync_block("frequency_deinterleaver",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*d_total_segments*d_data_carriers_mode1*((int)pow(2.0,mode-1))),
                    gr::io_signature::make(1, 1, sizeof(gr_complex)*d_total_segments*d_data_carriers_mode1*((int)pow(2.0,mode-1))))
        {
            d_mode = mode; 
            d_1seg = oneseg; 
            d_carriers_per_segment = d_data_carriers_mode1*((int)pow(2.0,mode-1)); 
            d_noutput = d_total_segments*d_carriers_per_segment; 

            /*Initialize the derandomized segments*/
            d_not_random = new gr_complex [d_noutput]; 
            /*Initialize the derotated segments*/
            d_derotated = new gr_complex [d_noutput]; 

            if (d_mode==1){
                d_random_perm = d_random_perm_mode1; 
            } else if (d_mode==2){
                d_random_perm = d_random_perm_mode2; 
            } else {
                d_random_perm = d_random_perm_mode3; 
            }
        }

        /*
         * Our virtual destructor.
         */
        frequency_deinterleaver_impl::~frequency_deinterleaver_impl()
        {
        }
        
        // The three basic steps in the deinterleaving: derandomization, derotation and inter-segment deinterleaving. 
        /*----------------------------*/
        gr_complex * frequency_deinterleaver_impl::derandomize(const gr_complex * random, gr_complex * not_random){
            for (int segment = 0; segment<d_total_segments; segment++) 
            {
                for(int carrier = 0; carrier<d_carriers_per_segment; carrier++) 
                {
                    not_random[carrier+segment*d_carriers_per_segment] = random[d_random_perm[carrier]+segment*d_carriers_per_segment]; 
                }

            }
            return not_random; 
        }

        gr_complex * frequency_deinterleaver_impl::derotate(const gr_complex * rotated, gr_complex * derotated){
            for (int segment = 1; segment<d_total_segments; segment++) 
            {
                for(int carrier = 0; carrier<d_carriers_per_segment; carrier++) 
                {
                    derotated[carrier+segment*d_carriers_per_segment] = rotated[((carrier+d_carriers_per_segment-segment) % d_carriers_per_segment) + segment*d_carriers_per_segment]; 
                }

            }
            return derotated; 
        }
        
        gr_complex * frequency_deinterleaver_impl::intersegment_deinterleave(const gr_complex * interleaved, gr_complex * deinterleaved){
            // The number of segments to enter the deinterleaver. 
            int n = d_total_segments; 
            if (d_1seg){
                n = d_total_segments-1; 
            }

            int overall_carrier = 0 + (d_total_segments-n)*d_carriers_per_segment; 
            for(int carrier = 0; carrier<d_carriers_per_segment; carrier++) 
                for (int segment = d_total_segments-n; segment<d_total_segments; segment++) 
                {
                    {
                        deinterleaved[overall_carrier] = interleaved[segment*d_carriers_per_segment + carrier]; 
                        overall_carrier++; 
                    }

                }
            return deinterleaved; 
        }
        /*----------------------------*/
        
        // The actual processing. 
        int
            frequency_deinterleaver_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const gr_complex *in = (const gr_complex *) input_items[0];
                gr_complex *out = (gr_complex *) output_items[0];


                for (int i = 0; i < noutput_items; i++)
                {
                    derandomize(&in[i*d_noutput], d_not_random); 
                    derotate(d_not_random, d_derotated); 
                    intersegment_deinterleave(d_derotated, &out[i*d_noutput]); 
                }

                // Tell runtime system how many output items we produced.
                return noutput_items;
            }

    } /* namespace isdbt */
} /* namespace gr */

