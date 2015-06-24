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
import math, random

class qa_ofdm_sym_acquisition (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

##    def test_001_t (self):
##        # set up fg
##        self.tb.run ()
##        # check data

    def generate_random_symbol(self, symbol_length, cp_length):
        '''Generates a random OFDM symbol (a random complex vector of length symbol_length) and then 
        prefixes it with the a copy of the last cp_length samples at the beginning. '''

        sym_without_cp_realpart = [random.random() for i in range(symbol_length)]
        sym_without_cp_imagpart = [random.random() for i in range(symbol_length)]
        
        sym_without_cp = [real + (1j)*imag for (real, imag) in zip(sym_without_cp_realpart, sym_without_cp_imagpart)]
        sym_with_cp = sym_without_cp[len(sym_without_cp)-cp_length:len(sym_without_cp)-1] + sym_without_cp
        return sym_with_cp

    def test_ofdm_symbol_aquisition_1 (self):

        mode = 3
        total_carriers = 2**(10+mode)
        cp_len = 1/16.0

        sym1 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym2 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym3 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))

        src_data =  sym1 + sym2 + sym3 + sym1 + sym2 + sym3

        expected_result = \
            (    0,       1,       3,       2,
                 0,       2,       1,       3) * 48

        src = blocks.vector_source_c(src_data)
        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=total_carriers, cp_length=int(total_carriers*cp_len), snr=10)
        dst = blocks.vector_sink_c(total_carriers)

        self.tb.connect(src,ofdmsym)
        self.tb.connect(ofdmsym,dst)
        self.tb.run()

        # check data
        actual_result = dst.nitems_read(0)
        print "actual result", actual_result
        #print "expected result", expected_result
        #self.assertFloatTuplesAlmostEqual(expected_result, actual_result)

if __name__ == '__main__':
    gr_unittest.run(qa_ofdm_sym_acquisition, "qa_ofdm_sym_acquisition.xml")
