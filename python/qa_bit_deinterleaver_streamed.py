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
#import isdbt
import isdbt_swig as isdbt

class qa_bit_deinterleaver_streamed (gr_unittest.TestCase):

    def mode3_qpsk(self, src_data, expected_result, test_nr=1):
        # set up fg
        constellation = 4          # QPSK
        total_segments = 1 
        mode = 3                   # mode 3 for QPSK

        carrier_mod1_symbols = 96  # mode 1: 96, mode 2: 192, mode 3: 384
        total_carriers = total_segments * carrier_mod1_symbols * 2**(mode-1)
        
        deinterleaver = isdbt.bit_deinterleaver_streamed(mode, constellation)
        
        src = blocks.vector_source_b(src_data, False, total_carriers)
        dst = blocks.vector_sink_b(1)

        self.tb.connect(src, deinterleaver)
        self.tb.connect(deinterleaver, dst)
        self.tb.run()

        actual_result = dst.data()

        print '=== QPSK test Nr ' + str(test_nr)
        for i in range(0, 1200, 120):
            print src_data[i],
        print
        for i in range(0, 1200, 120):
            print actual_result[i],
        print

        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)


    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_mode3_qpsk_1 (self):

        constellation = 4          # QPSK
        total_segments = 1 
        mode = 3                   # mode 3 for QPSK
        carrier_mod1_symbols = 96  # mode 1: 96, mode 2: 192, mode 3: 384
        total_carriers = total_segments * carrier_mod1_symbols * 2**(mode-1)
        # Input are the same symbols repeated for the delay line,
        # so expected result is easy to calculate (see below).
        src_data = [ [i]*120 for i in range(constellation) ] * total_carriers
        src_data = [item for sublist in src_data for item in sublist]

        expected_result = [ [0]*120] +  \
            [ [i]*120 for i in (1, 0, 3, 2) ] * (total_carriers-1) + \
            [ [i]*120 for i in (1, 0, 3) ]
        expected_result = \
            [item for sublist in expected_result for item in sublist]

        self.mode3_qpsk(src_data, expected_result, test_nr=1)

    
    def test_mode3_qpsk_2 (self):

        constellation = 4        # QPSK
        total_segments = 1 
        mode = 3                 # mode 3 for QPSK
        carrier_mod1_symbols = 96  # mode 1: 96, mode 2: 192, mode 3: 384
        total_carriers = total_segments * carrier_mod1_symbols * 2**(mode-1)
        # Input are the same symbols repeated for the delay line,
        # so expected result is easy to calculate (see below).
        src_data_0 = [ [i]*120 for i in (0, 3, 2, 1) ] * total_carriers
        src_data = [item for sublist in src_data_0 for item in sublist]

        expected_result_0 = [ [i]*120 for  i in (1,2,3)] + \
            [ [i]*120 for i in (0, 1, 2, 3) ] * (total_carriers-1)   + \
            [ [i]*120 for i in (0,) ]
        expected_result_0 = [ [i]*120 for  i in (0, 1,2,3)] * total_carriers
        expected_result = \
            [item for sublist in expected_result_0 for item in sublist]

        self.mode3_qpsk(src_data, expected_result, test_nr=2)



if __name__ == '__main__':
    gr_unittest.run(qa_bit_deinterleaver_streamed, "qa_bit_deinterleaver_streamed.xml")
