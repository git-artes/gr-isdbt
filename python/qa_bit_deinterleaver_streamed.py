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

class qa_bit_deinterleaver_streamed (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_mode3_qpsk (self):
        # set up fg

        total_segments = 1; 
        mode = 3; 
        constellation=4
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.bit_deinterleaver_streamed(mode, constellation)

        # the input are the same symbols repeated for the delay line. The
        # expected result in this case is easy to calculate (see below)
        src_data = [[i]*120 for i in range(constellation)]*total_carriers
        src_data = [item for sublist in src_data for item in sublist]
        
        src = blocks.vector_source_b(src_data, False, total_carriers)
        dst = blocks.vector_sink_b(1)

        self.tb.connect(src,deinterleaver)
        self.tb.connect(deinterleaver,dst)
        self.tb.run()

        actual_result = dst.data()
        
        expected_result =[[0]*120] + [[i]*120 for i in (1, 0, 3, 2)]*(total_carriers-1) + [[i]*120 for i in (1, 0, 3)]
        expected_result = [item for sublist in expected_result for item in sublist]
        print expected_result

        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)


if __name__ == '__main__':
    gr_unittest.run(qa_bit_deinterleaver_streamed, "qa_bit_deinterleaver_streamed.xml")
