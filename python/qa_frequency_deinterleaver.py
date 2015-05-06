#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2015 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import isdbt

class qa_frequency_deinterleaver (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_1seg_mode3 (self):
        # set up fg

        total_segments = 1; 
        mode = 3; 
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.frequency_deinterleaver(oneseg=True,mode=3)

        # the random array of indices
        src_data = (249, 272, 269, 137, 309, 24, 255, 327, 294, 197, 364, 3, 160, 1, 46, 354, 104, 113, 195, 193, 59, 236, 90, 288, 292, 224, 258, 382, 163, 85, 153, 89, 350, 277, 298, 237, 230, 345, 135, 127, 340, 349, 366, 77, 180, 25, 11, 333, 332, 157, 239, 383, 91, 18, 314, 185, 10, 43, 102, 169, 162, 37, 0, 194, 117, 290, 233, 270, 278, 343, 338, 204, 317, 130, 273, 336, 377, 55, 315, 51, 304, 72, 23, 208, 266, 98, 174, 209, 35, 303, 82, 379, 9, 365, 295, 378, 257, 78, 225, 134, 48, 76, 84, 265, 337, 284, 240, 238, 114, 179, 341, 280, 58, 109, 101, 56, 96, 141, 281, 107, 12, 188, 253, 44, 232, 22, 32, 375, 120, 73, 215, 320, 326, 176, 356, 28, 248, 228, 212, 66, 282, 30, 80, 62, 371, 106, 344, 339, 328, 145, 200, 149, 63, 139, 210, 42, 368, 81, 131, 252, 216, 155, 361, 306, 19, 293, 275, 105, 330, 235, 171, 289, 16, 26, 348, 13, 158, 95, 61, 41, 316, 170, 29, 71, 52, 53, 261, 21, 254, 119, 192, 100, 69, 111, 276, 283, 99, 260, 103, 60, 259, 227, 285, 358, 65, 50, 251, 321, 45, 359, 144, 199, 268, 143, 83, 64, 219, 152, 211, 229, 7, 325, 296, 222, 331, 167, 8, 47, 267, 164, 201, 203, 112, 36, 67, 17, 220, 118, 299, 124, 198, 150, 357, 133, 380, 108, 116, 308, 196, 38, 367, 175, 244, 305, 223, 205, 74, 183, 27, 287, 190, 88, 243, 246, 33, 165, 302, 279, 182, 213, 376, 245, 355, 31, 381, 191, 132, 57, 217, 313, 307, 291, 319, 115, 256, 4, 34, 136, 166, 142, 312, 286, 370, 94, 373, 110, 97, 362, 14, 329, 226, 93, 122, 86, 351, 92, 324, 352, 221, 70, 40, 49, 353, 335, 75, 140, 318, 186, 271, 172, 123, 151, 156, 184, 79, 231, 242, 363, 54, 374, 138, 297, 202, 241, 154, 125, 5, 147, 369, 148, 263, 173, 274, 322, 177, 262, 161, 128, 214, 264, 206, 129, 15, 234, 347, 207, 126, 334, 68, 159, 189, 178, 310, 87, 300, 6, 250, 39, 20, 181, 323, 2, 247, 121, 342, 346, 301, 218, 372, 311, 360, 187, 168, 146)
       
        
        src_data = src_data*total_segments*3

	expected_result = range(96*2**(mode-1))*total_segments*3

        src = blocks.vector_source_c(src_data, False, total_carriers)
        dst = blocks.vector_sink_c(total_carriers)

        self.tb.connect(src,deinterleaver)
        self.tb.connect(deinterleaver,dst)
        self.tb.run()

        # check data
        actual_result = dst.data()
 #       print "src data: ", src_data
 #       print "actual result: ", actual_result
 #       print "expected result: ", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)

    def test_1seg_mode2 (self):
        # set up fg

        total_segments = 1; 
        mode = 2; 
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.frequency_deinterleaver(True,mode)

        # the random array of indices
        src_data = (78, 120, 35, 91, 150, 6, 129, 160, 159, 169, 59, 148, 114, 154, 37, 123, 162, 5, 139, 70, 89, 136, 106, 128, 69, 65, 163, 190, 26, 184, 170, 151, 155, 50, 182, 1, 25, 16, 57, 73, 153, 158, 47, 12, 166, 55, 42, 28, 15, 112, 147, 61, 97, 186, 10, 156, 131, 71, 107, 191, 101, 83, 121, 175, 168, 100, 119, 2, 41, 17, 140, 72, 33, 8, 13, 84, 21, 74, 27, 23, 126, 165, 85, 111, 53, 77, 36, 105, 76, 66, 143, 187, 95, 7, 30, 142, 125, 49, 0, 132, 108, 86, 117, 167, 179, 48, 178, 20, 185, 145, 189, 109, 45, 81, 40, 181, 3, 62, 88, 127, 94, 92, 44, 133, 146, 67, 172, 99, 29, 141, 38, 104, 113, 102, 51, 4, 90, 135, 134, 68, 82, 110, 34, 11, 161, 137, 116, 103, 174, 144, 19, 39, 149, 98, 18, 115, 63, 171, 180, 157, 96, 64, 79, 31, 118, 14, 130, 58, 9, 188, 54, 177, 152, 164, 87, 24, 22, 52, 122, 173, 46, 80, 124, 43, 32, 138, 183, 56, 176, 60, 93, 75)
        
        src_data = src_data*total_segments

	expected_result = range(96*2**(mode-1))*total_segments

        src = blocks.vector_source_c(src_data, False, total_carriers)
        dst = blocks.vector_sink_c(total_carriers)

        self.tb.connect(src,deinterleaver)
        self.tb.connect(deinterleaver,dst)
        self.tb.run()

        # check data
        actual_result = dst.data()
#        print "src data: ", src_data
#        print "actual result: ", actual_result
#        print "expected result: ", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)

    def test_1seg_mode1 (self):
        # set up fg

        total_segments = 1; 
        mode = 1; 
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.frequency_deinterleaver(True,mode)

        # the random array of indices
        
        src_data = (32, 26, 69, 51, 35, 61, 8, 39, 46, 10, 87, 34, 20, 23, 74, 15, 38, 6, 22, 45, 55, 50, 41, 42, 95, 59, 52, 76, 33, 68, 80, 60, 89, 88, 25, 37, 47, 79, 27, 48, 73, 85, 63, 66, 78, 36, 18, 82, 40, 62, 91, 9, 14, 94, 64, 5, 44, 77, 92, 29, 67, 49, 90, 2, 31, 13, 17, 54, 81, 53, 21, 19, 58, 16, 56, 84, 70, 43, 28, 75, 0, 7, 93, 72, 71, 11, 57, 65, 83, 12, 86, 30, 3, 1, 4, 24)
        src_data = src_data*total_segments

	expected_result = range(96*2**(mode-1))*total_segments

        src = blocks.vector_source_c(src_data, False, total_carriers)
        dst = blocks.vector_sink_c(total_carriers)

        self.tb.connect(src,deinterleaver)
        self.tb.connect(deinterleaver,dst)
        self.tb.run()

        # check data
        actual_result = dst.data()
        print "src data: ", src_data
        print "actual result: ", actual_result
        print "expected result: ", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)

if __name__ == '__main__':
    gr_unittest.run(qa_frequency_deinterleaver, "qa_frequency_deinterleaver.xml")
