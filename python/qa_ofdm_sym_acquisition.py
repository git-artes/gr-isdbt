#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2014 <+YOU OR YOUR COMPANY+>.
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
import isdbt as isdbt

class qa_ofdm_sym_acquisition (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data

    def test_ofdm_symbol_aquisition_1 (self):

        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=8192, cp_length=512, snr=10)

        cp1 = (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
               0.8 + 1.0j, -0.5 + 0.1j,  0.5 - 1.2j, -0.2 - 1.0j) * 64      # 8 * 64 = 512 samples
        frame1_sin_cp1 = (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
               0.8 + 1.0j, -0.5 + 0.1j,  0.5 - 1.2j, -0.2 - 1.0j) * 960      # 8 * 960 = 7680 samples (8192-512=7680)
        frame1 = frame1_sin_cp1 + cp1

        cp2 = (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
               0.8 + 1.0j, -0.5 + 0.1j,  0.5 - 1.2j, -0.2 - 1.0j) * 64      # 8 * 64 = 512 samples
        frame2_sin_cp2 = (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
               0.8 + 1.0j, -0.5 + 0.1j,  0.5 - 1.2j, -0.2 - 1.0j) * 960     # 8 * 960 = 7680 samples (8192-512=7680)
        frame2 = frame2_sin_cp2 + cp2 

        src_data =  cp1 + frame1 + cp2 + frame2 

        expected_result = \
            (    0,       1,       3,       2,
                 0,       2,       1,       3) * 48

        src = blocks.vector_source_c(src_data)
        dst = blocks.vector_sink_c()

        self.tb.connect(src,dst)
        self.tb.run()

        # check data
        actual_result = dst.nitems_read(0)
        print "actual result", actual_result
        #print "expected result", expected_result
        #self.assertFloatTuplesAlmostEqual(expected_result, actual_result)

if __name__ == '__main__':
    gr_unittest.run(qa_ofdm_sym_acquisition, "qa_ofdm_sym_acquisition.xml")
