#!/usr/bin/env python
# -*- coding: utf-8 -*-
#  
# Copyright 2015
#   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
#   Pablo Belzarena 
#   Gabriel Gomez Sena 
#   Pablo Flores Guridi 
#   Victor Gonzalez Barbone
# 
#   Instituto de Ingenieria Electrica, Facultad de Ingenieria,
#   Universidad de la Republica, Uruguay.
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
        sym_with_cp = self.generate_symbol_with_cp(sym_without_cp, cp_length)
        return sym_with_cp

    def generate_symbol_with_cp(self, ofdm_sym, cp_length):
        '''It prefixes the CP to a given ofdm symbol. '''
        sym_with_cp = ofdm_sym[len(ofdm_sym)-cp_length:len(ofdm_sym)] + ofdm_sym
        return sym_with_cp

    def test_ofdm_symbol_aquisition_mode3_cp116 (self):

        mode = 3
        total_carriers = 2**(10+mode)
        cp_len = 1/16.0

        sym1 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym2 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym3 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym4 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym5 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        #sym1 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));
        #sym2 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));
        #sym3 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));
        #sym4 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));
        #sym5 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));

        src_data = [random.random() for i in range(20)]+ sym1 + sym2 + sym3 + sym4 + sym5
        #src_data = sym1 + sym2 + sym3 + sym4 + sym5
        expected_result = sym1[int(total_carriers*cp_len):len(sym1)]+\
                sym2[int(total_carriers*cp_len):len(sym2)]+\
                sym3[int(total_carriers*cp_len):len(sym3)]+\
                sym4[int(total_carriers*cp_len):len(sym4)]

        expected_freq = (0,)*4


        src = blocks.vector_source_c(src_data)
        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=total_carriers, cp_length=int(total_carriers*cp_len), snr=50)
        dst = blocks.vector_sink_c(total_carriers)
        dst_freq = blocks.vector_sink_f()

        self.tb.connect(src,ofdmsym)
        self.tb.connect(ofdmsym,dst)
        self.tb.connect((ofdmsym,1),dst_freq)
        self.tb.run()

        # check data
        actual_result = dst.data()
        actual_freq = dst_freq.data()
        #print "actual=", actual_result
        #print "expected=", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)
        self.assertFloatTuplesAlmostEqual(expected_freq, actual_freq)

    def test_ofdm_symbol_aquisition_mode3_cp132 (self):

        mode = 3
        total_carriers = 2**(10+mode)
        cp_len = 1/32.0

        sym1 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym2 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym3 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym4 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym5 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))

        src_data = [random.random() for i in range(20)]+ sym1 + sym2 + sym3 + sym4 + sym5
        expected_result = sym1[int(total_carriers*cp_len):len(sym1)]+\
                sym2[int(total_carriers*cp_len):len(sym2)]+\
                sym3[int(total_carriers*cp_len):len(sym3)]+\
                sym4[int(total_carriers*cp_len):len(sym4)]

        expected_freq = (0,)*4


        src = blocks.vector_source_c(src_data)
        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=total_carriers, cp_length=int(total_carriers*cp_len), snr=50)
        dst = blocks.vector_sink_c(total_carriers)
        dst_freq = blocks.vector_sink_f()

        self.tb.connect(src,ofdmsym)
        self.tb.connect(ofdmsym,dst)
        self.tb.connect((ofdmsym,1),dst_freq)
        self.tb.run()

        # check data
        actual_result = dst.data()
        actual_freq = dst_freq.data()
        #print "actual=", actual_result
        #print "expected=", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)
        self.assertFloatTuplesAlmostEqual(expected_freq, actual_freq)

    def test_ofdm_symbol_aquisition_mode3_cp18 (self):

        mode = 3
        total_carriers = 2**(10+mode)
        cp_len = 1/8.0

        sym1 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym2 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym3 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym4 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym5 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))

        src_data = [random.random() for i in range(20)]+ sym1 + sym2 + sym3 + sym4 + sym5
        expected_result = sym1[int(total_carriers*cp_len):len(sym1)]+\
                sym2[int(total_carriers*cp_len):len(sym2)]+\
                sym3[int(total_carriers*cp_len):len(sym3)]+\
                sym4[int(total_carriers*cp_len):len(sym4)]

        expected_freq = (0,)*4


        src = blocks.vector_source_c(src_data)
        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=total_carriers, cp_length=int(total_carriers*cp_len), snr=50)
        dst = blocks.vector_sink_c(total_carriers)
        dst_freq = blocks.vector_sink_f()

        self.tb.connect(src,ofdmsym)
        self.tb.connect(ofdmsym,dst)
        self.tb.connect((ofdmsym,1),dst_freq)
        self.tb.run()

        # check data
        actual_result = dst.data()
        actual_freq = dst_freq.data()
        #print "actual=", actual_result
        #print "expected=", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)
        self.assertFloatTuplesAlmostEqual(expected_freq, actual_freq)

    def test_ofdm_symbol_aquisition_mode3_cp14 (self):

        mode = 3
        total_carriers = 2**(10+mode)
        cp_len = 1/4.0

        sym1 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym2 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym3 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym4 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym5 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))

        src_data = [random.random() for i in range(20)]+ sym1 + sym2 + sym3 + sym4 + sym5
        expected_result = sym1[int(total_carriers*cp_len):len(sym1)]+\
                sym2[int(total_carriers*cp_len):len(sym2)]+\
                sym3[int(total_carriers*cp_len):len(sym3)]+\
                sym4[int(total_carriers*cp_len):len(sym4)]

        expected_freq = (0,)*4


        src = blocks.vector_source_c(src_data)
        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=total_carriers, cp_length=int(total_carriers*cp_len), snr=50)
        dst = blocks.vector_sink_c(total_carriers)
        dst_freq = blocks.vector_sink_f()

        self.tb.connect(src,ofdmsym)
        self.tb.connect(ofdmsym,dst)
        self.tb.connect((ofdmsym,1),dst_freq)
        self.tb.run()

        # check data
        actual_result = dst.data()
        actual_freq = dst_freq.data()
        #print "actual=", actual_result
        #print "expected=", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)
        self.assertFloatTuplesAlmostEqual(expected_freq, actual_freq)

    def test_ofdm_symbol_aquisition_mode2_cp116 (self):

        mode = 2
        total_carriers = 2**(10+mode)
        cp_len = 1/16.0

        sym1 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym2 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym3 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym4 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym5 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        #sym1 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));
        #sym2 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));
        #sym3 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));
        #sym4 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));
        #sym5 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));

        src_data = [random.random() for i in range(20)]+ sym1 + sym2 + sym3 + sym4 + sym5
        expected_result = sym1[int(total_carriers*cp_len):len(sym1)]+\
                sym2[int(total_carriers*cp_len):len(sym2)]+\
                sym3[int(total_carriers*cp_len):len(sym3)]+\
                sym4[int(total_carriers*cp_len):len(sym4)]

        expected_freq = (0,)*4


        src = blocks.vector_source_c(src_data)
        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=total_carriers, cp_length=int(total_carriers*cp_len), snr=50)
        dst = blocks.vector_sink_c(total_carriers)
        dst_freq = blocks.vector_sink_f()

        self.tb.connect(src,ofdmsym)
        self.tb.connect(ofdmsym,dst)
        self.tb.connect((ofdmsym,1),dst_freq)
        self.tb.run()

        # check data
        actual_result = dst.data()
        actual_freq = dst_freq.data()
        #print "actual=", actual_result
        #print "expected=", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)
        self.assertFloatTuplesAlmostEqual(expected_freq, actual_freq)

    def test_ofdm_symbol_aquisition_mode2_cp132 (self):

        mode = 2
        total_carriers = 2**(10+mode)
        cp_len = 1/32.0

        sym1 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym2 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym3 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym4 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym5 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))

        src_data = [random.random() for i in range(20)]+ sym1 + sym2 + sym3 + sym4 + sym5
        expected_result = sym1[int(total_carriers*cp_len):len(sym1)]+\
                sym2[int(total_carriers*cp_len):len(sym2)]+\
                sym3[int(total_carriers*cp_len):len(sym3)]+\
                sym4[int(total_carriers*cp_len):len(sym4)]

        expected_freq = (0,)*4


        src = blocks.vector_source_c(src_data)
        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=total_carriers, cp_length=int(total_carriers*cp_len), snr=50)
        dst = blocks.vector_sink_c(total_carriers)
        dst_freq = blocks.vector_sink_f()

        self.tb.connect(src,ofdmsym)
        self.tb.connect(ofdmsym,dst)
        self.tb.connect((ofdmsym,1),dst_freq)
        self.tb.run()

        # check data
        actual_result = dst.data()
        actual_freq = dst_freq.data()
        #print "actual=", actual_result
        #print "expected=", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)
        self.assertFloatTuplesAlmostEqual(expected_freq, actual_freq)

    def test_ofdm_symbol_aquisition_mode2_cp18 (self):

        mode = 2
        total_carriers = 2**(10+mode)
        cp_len = 1/8.0

        sym1 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym2 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym3 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym4 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym5 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))

        src_data = [random.random() for i in range(20)]+ sym1 + sym2 + sym3 + sym4 + sym5
        expected_result = sym1[int(total_carriers*cp_len):len(sym1)]+\
                sym2[int(total_carriers*cp_len):len(sym2)]+\
                sym3[int(total_carriers*cp_len):len(sym3)]+\
                sym4[int(total_carriers*cp_len):len(sym4)]

        expected_freq = (0,)*4


        src = blocks.vector_source_c(src_data)
        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=total_carriers, cp_length=int(total_carriers*cp_len), snr=50)
        dst = blocks.vector_sink_c(total_carriers)
        dst_freq = blocks.vector_sink_f()

        self.tb.connect(src,ofdmsym)
        self.tb.connect(ofdmsym,dst)
        self.tb.connect((ofdmsym,1),dst_freq)
        self.tb.run()

        # check data
        actual_result = dst.data()
        actual_freq = dst_freq.data()
        #print "actual=", actual_result
        #print "expected=", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)
        self.assertFloatTuplesAlmostEqual(expected_freq, actual_freq)

    def test_ofdm_symbol_aquisition_mode2_cp14 (self):

        mode = 2
        total_carriers = 2**(10+mode)
        cp_len = 1/4.0

        sym1 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym2 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym3 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym4 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym5 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))

        src_data = [random.random() for i in range(20)]+ sym1 + sym2 + sym3 + sym4 + sym5
        expected_result = sym1[int(total_carriers*cp_len):len(sym1)]+\
                sym2[int(total_carriers*cp_len):len(sym2)]+\
                sym3[int(total_carriers*cp_len):len(sym3)]+\
                sym4[int(total_carriers*cp_len):len(sym4)]

        expected_freq = (0,)*4


        src = blocks.vector_source_c(src_data)
        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=total_carriers, cp_length=int(total_carriers*cp_len), snr=50)
        dst = blocks.vector_sink_c(total_carriers)
        dst_freq = blocks.vector_sink_f()

        self.tb.connect(src,ofdmsym)
        self.tb.connect(ofdmsym,dst)
        self.tb.connect((ofdmsym,1),dst_freq)
        self.tb.run()

        # check data
        actual_result = dst.data()
        actual_freq = dst_freq.data()
        #print "actual=", actual_result
        #print "expected=", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)
        self.assertFloatTuplesAlmostEqual(expected_freq, actual_freq)

    def test_ofdm_symbol_aquisition_mode1_cp116 (self):

        mode = 1
        total_carriers = 2**(10+mode)
        cp_len = 1/16.0

        sym1 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym2 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym3 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym4 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym5 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        #sym1 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));
        #sym2 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));
        #sym3 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));
        #sym4 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));
        #sym5 = self.generate_symbol_with_cp(range(total_carriers), int(total_carriers*cp_len));

        src_data = [random.random() for i in range(20)]+ sym1 + sym2 + sym3 + sym4 + sym5
        expected_result = sym1[int(total_carriers*cp_len):len(sym1)]+\
                sym2[int(total_carriers*cp_len):len(sym2)]+\
                sym3[int(total_carriers*cp_len):len(sym3)]+\
                sym4[int(total_carriers*cp_len):len(sym4)]

        expected_freq = (0,)*4


        src = blocks.vector_source_c(src_data)
        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=total_carriers, cp_length=int(total_carriers*cp_len), snr=50)
        dst = blocks.vector_sink_c(total_carriers)
        dst_freq = blocks.vector_sink_f()

        self.tb.connect(src,ofdmsym)
        self.tb.connect(ofdmsym,dst)
        self.tb.connect((ofdmsym,1),dst_freq)
        self.tb.run()

        # check data
        actual_result = dst.data()
        actual_freq = dst_freq.data()
        #print "actual=", actual_result
        #print "expected=", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)
        self.assertFloatTuplesAlmostEqual(expected_freq, actual_freq)

    def test_ofdm_symbol_aquisition_mode1_cp132 (self):

        mode = 1
        total_carriers = 2**(10+mode)
        cp_len = 1/32.0

        sym1 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym2 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym3 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym4 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym5 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))

        src_data = [random.random() for i in range(20)]+ sym1 + sym2 + sym3 + sym4 + sym5
        expected_result = sym1[int(total_carriers*cp_len):len(sym1)]+\
                sym2[int(total_carriers*cp_len):len(sym2)]+\
                sym3[int(total_carriers*cp_len):len(sym3)]+\
                sym4[int(total_carriers*cp_len):len(sym4)]

        expected_freq = (0,)*4


        src = blocks.vector_source_c(src_data)
        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=total_carriers, cp_length=int(total_carriers*cp_len), snr=50)
        dst = blocks.vector_sink_c(total_carriers)
        dst_freq = blocks.vector_sink_f()

        self.tb.connect(src,ofdmsym)
        self.tb.connect(ofdmsym,dst)
        self.tb.connect((ofdmsym,1),dst_freq)
        self.tb.run()

        # check data
        actual_result = dst.data()
        actual_freq = dst_freq.data()
        #print "actual=", actual_result
        #print "expected=", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)
        self.assertFloatTuplesAlmostEqual(expected_freq, actual_freq)

    def test_ofdm_symbol_aquisition_mode1_cp18 (self):

        mode = 1
        total_carriers = 2**(10+mode)
        cp_len = 1/8.0

        sym1 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym2 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym3 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym4 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym5 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))

        src_data = [random.random() for i in range(20)]+ sym1 + sym2 + sym3 + sym4 + sym5
        expected_result = sym1[int(total_carriers*cp_len):len(sym1)]+\
                sym2[int(total_carriers*cp_len):len(sym2)]+\
                sym3[int(total_carriers*cp_len):len(sym3)]+\
                sym4[int(total_carriers*cp_len):len(sym4)]

        expected_freq = (0,)*4


        src = blocks.vector_source_c(src_data)
        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=total_carriers, cp_length=int(total_carriers*cp_len), snr=50)
        dst = blocks.vector_sink_c(total_carriers)
        dst_freq = blocks.vector_sink_f()

        self.tb.connect(src,ofdmsym)
        self.tb.connect(ofdmsym,dst)
        self.tb.connect((ofdmsym,1),dst_freq)
        self.tb.run()

        # check data
        actual_result = dst.data()
        actual_freq = dst_freq.data()
        #print "actual=", actual_result
        #print "expected=", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)
        self.assertFloatTuplesAlmostEqual(expected_freq, actual_freq)

    def test_ofdm_symbol_aquisition_mode1_cp14 (self):

        mode = 1
        total_carriers = 2**(10+mode)
        cp_len = 1/4.0

        sym1 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym2 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym3 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym4 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))
        sym5 = self.generate_random_symbol(total_carriers, int(total_carriers*cp_len))

        src_data = [random.random() for i in range(20)]+ sym1 + sym2 + sym3 + sym4 + sym5
        expected_result = sym1[int(total_carriers*cp_len):len(sym1)]+\
                sym2[int(total_carriers*cp_len):len(sym2)]+\
                sym3[int(total_carriers*cp_len):len(sym3)]+\
                sym4[int(total_carriers*cp_len):len(sym4)]

        expected_freq = (0,)*4


        src = blocks.vector_source_c(src_data)
        ofdmsym = isdbt.ofdm_sym_acquisition(fft_length=total_carriers, cp_length=int(total_carriers*cp_len), snr=50)
        dst = blocks.vector_sink_c(total_carriers)
        dst_freq = blocks.vector_sink_f()

        self.tb.connect(src,ofdmsym)
        self.tb.connect(ofdmsym,dst)
        self.tb.connect((ofdmsym,1),dst_freq)
        self.tb.run()

        # check data
        actual_result = dst.data()
        actual_freq = dst_freq.data()
        #print "actual=", actual_result
        #print "expected=", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)
        self.assertFloatTuplesAlmostEqual(expected_freq, actual_freq)


if __name__ == '__main__':
    gr_unittest.run(qa_ofdm_sym_acquisition, "qa_ofdm_sym_acquisition.xml")
