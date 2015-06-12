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
import isdbt
import math, random

class qa_symbol_demapper_1seg (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None
    
    def add_noise(self, z, amnt_noise=1.0):
        '''Adds random value less than amnt_noise to real, imag in complex.
        
        Adds a random value rnd1 to z.real and rnd2 to z.imag. Both rnd1 and rnd2 are less than amnt_noise in absolute value, and each may be positive or negative.'''
        rnd1, rnd2 = random.random(), random.random()
        rnd1, rnd2 = rnd1*amnt_noise*2, rnd2*amnt_noise*2
        if rnd1 < amnt_noise:
            z = z + rnd1
        else:
            z = z - (rnd1 - amnt_noise)
        if rnd2 < amnt_noise:
            z = z + rnd2*(1j)
        else:
            z = z - (rnd2 - amnt_noise)*(1j)
        return z


    def test_symbol_demapper_qpsk (self):
        # set up fg
        demapper = isdbt.symbol_demapper_1seg(mode=3, constellation_size=4)
        #src_data = (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
        #            0.8 + 1.0j, -0.5 + 0.1j,  0.5 - 1.2j, -0.2 - 1.0j) * 48
        src_syms = [
            1 + 1j,  1 - 1j, -1 - 1j, -1 + 1j,
            1 + 1j, -1 + 1j,  1 - 1j, -1 - 1j ]
        src_syms_noisy = [self.add_noise(z) for z in src_syms]
        src_data = src_syms_noisy * 48
        # normalización sqrt(2) ?
        expected_result = \
            (    0,       1,       3,       2,
                 0,       2,       1,       3) * 48

        src = blocks.vector_source_c(src_data)
        str_to_vec = blocks.stream_to_vector(gr.sizeof_gr_complex, 384)
        vec_to_str = blocks.vector_to_stream(gr.sizeof_char, 384)
        dst = blocks.vector_sink_b()

        self.tb.connect(src,str_to_vec)
        self.tb.connect(str_to_vec,demapper)
        self.tb.connect(demapper,vec_to_str)
        self.tb.connect(vec_to_str,dst)
        self.tb.run()

        # check data
        actual_result = dst.data()
        #print "actual result", actual_result
        #print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)


    def test_symbol_demapper_16qam (self):
        # set up fg
        demapper = isdbt.symbol_demapper_1seg(mode=3,constellation_size=16)
        """src_data = ( \
            -3.5 + 2.5j, -0.7 + 3.2j, 1.8 + 3.1j, 3.45 + 2.8j,
            -3.8 + 1.0j, -0.9 + 0.1j, 0.5 + 1.2j, 2.22 + 1.0j, 
            -3.1 - 0.5j, -0.5 - 1.2j, 0.8 - 0.1j, 2.45 - 0.8j,
            -2.8 - 3.1j, -1.5 - 2.1j, 1.5 - 2.2j, 3.32 - 3.0j) * 24
        """
        src_syms = [
            -3+3j, -1+3j, +1+3j, +3+3j,
            -3+1j, -1+1j, +1+1j, +3+1j,
            -3-1j, -1-1j, +1-1j, +3-1j,
            -3-3j, -1-3j, +1-3j, +3-3j ]
        src_syms_noisy = [self.add_noise(z) for z in src_syms]
        src_data = src_syms_noisy * 24

        src_data = [x / math.sqrt(10.0) for x in src_data]
        expected_result = (  \
                8,     10,     2,    0,
                9,     11,     3,    1,
               13,     15,     7,    5,
               12,     14,     6,    4) * 24

        src = blocks.vector_source_c(src_data)
        str_to_vec = blocks.stream_to_vector(gr.sizeof_gr_complex,384)
        vec_to_str = blocks.vector_to_stream(gr.sizeof_char,384)
        dst = blocks.vector_sink_b()

        self.tb.connect(src,str_to_vec)
        self.tb.connect(str_to_vec,demapper)
        self.tb.connect(demapper,vec_to_str)
        self.tb.connect(vec_to_str,dst)
        self.tb.run()

        # check data
        actual_result = dst.data()
        #print "actual result", actual_result
        #print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)


    def test_symbol_demapper_64qam (self):
        # set up fg
        demapper = isdbt.symbol_demapper_1seg(mode=3, constellation_size=64)
        
        """
        src_data =  (-7.5 + 7.5j, -5.7 + 7.2j, -3.8 + 6.1j, -0.45 + 6.8j,
                    -7.8 + 5.0j, -4.9 + 4.1j, -2.5 + 4.2j, -0.22 + 5.0j, 
                    -7.1 + 3.5j, -4.5 + 2.2j, -2.8 + 2.1j, -1.45 + 3.8j,
                    -6.8 + 1.1j, -5.5 + 0.1j, -2.5 + 1.2j, -1.32 + 1.0j, 

                     7.5 + 7.5j, 5.7 + 7.2j, 3.8 + 6.1j, 0.45 + 6.8j,
                     7.8 + 5.0j, 4.9 + 4.1j, 2.5 + 4.2j, 0.22 + 5.0j, 
                     7.1 + 3.5j, 4.5 + 2.2j, 2.8 + 2.1j, 1.45 + 3.8j,
                     6.8 + 1.1j, 5.5 + 0.1j, 2.5 + 1.2j, 1.32 + 1.0j, 

                    -7.5 - 7.5j, -5.7 - 7.2j, -3.8 - 6.1j, -0.45 - 6.8j,
                    -7.8 - 5.0j, -4.9 - 4.1j, -2.5 - 4.2j, -0.22 - 5.0j, 
                    -7.1 - 3.5j, -4.5 - 2.2j, -2.8 - 2.1j, -1.45 - 3.8j,
                    -6.8 - 1.1j, -5.5 - 0.1j, -2.5 - 1.2j, -1.32 - 1.0j, 

                     7.5 - 7.5j, 5.7 - 7.2j, 3.8 - 6.1j, 0.45 - 6.8j,
                     7.8 - 5.0j, 4.9 - 4.1j, 2.5 - 4.2j, 0.22 - 5.0j, 
                     7.1 - 3.5j, 4.5 - 2.2j, 2.8 - 2.1j, 1.45 - 3.8j,
                     6.8 - 1.1j, 5.5 - 0.1j, 2.5 - 1.2j, 1.32 - 1.0j)*6
        """
        src_syms =  (  \
            -7 + 7j, -5 + 7j, -3 + 7j, -1 + 7j,
            -7 + 5j, -5 + 5j, -3 + 5j, -1 + 5j, 
            -7 + 3j, -5 + 3j, -3 + 3j, -1 + 3j,
            -7 + 1j, -5 + 1j, -3 + 1j, -1 + 1j, 

            +7 + 7j, +5 + 7j, +3 + 7j, +1 + 7j,
            +7 + 5j, +5 + 5j, +3 + 5j, +1 + 5j, 
            +7 + 3j, +5 + 3j, +3 + 3j, +1 + 3j,
            +7 + 1j, +5 + 1j, +3 + 1j, +1 + 1j, 

            -7 - 7j, -5 - 7j, -3 - 7j, -1 - 7j,
            -7 - 5j, -5 - 5j, -3 - 5j, -1 - 5j, 
            -7 - 3j, -5 - 3j, -3 - 3j, -1 - 3j,
            -7 - 1j, -5 - 1j, -3 - 1j, -1 - 1j, 

            +7 - 7j, +5 - 7j, +3 - 7j, +1 - 7j,
            +7 - 5j, +5 - 5j, +3 - 5j, +1 - 5j, 
            +7 - 3j, +5 - 3j, +3 - 3j, +1 - 3j,
            +7 - 1j, +5 - 1j, +3 - 1j, +1 - 1j)
        src_syms_noisy = [self.add_noise(z) for z in src_syms]
        src_data = src_syms_noisy * 6

        src_data = [x / math.sqrt(42.0) for x in src_data]
        expected_result = ( \
                32,      34,       42,      40,
                33,      35,       43,      41,
                37,      39,       47,      45,
                36,      38,       46,      44,

                 0,       2,       10,       8,
                 1,       3,       11,       9,
                 5,       7,       15,      13,
                 4,       6,       14,      12,
    
                48,      50,       58,      56,
                49,      51,       59,      57,
                53,      55,       63,      61,
                52,      54,       62,      60,
    
                16,      18,       26,      24,
                17,      19,       27,      25,
                21,      23,       31,      29,
                20,      22,       30,      28) * 6

        src = blocks.vector_source_c(src_data)
        str_to_vec = blocks.stream_to_vector(gr.sizeof_gr_complex,384)
        vec_to_str = blocks.vector_to_stream(gr.sizeof_char,384)
        dst = blocks.vector_sink_b()

        self.tb.connect(src,str_to_vec)
        self.tb.connect(str_to_vec,demapper)
        self.tb.connect(demapper,vec_to_str)
        self.tb.connect(vec_to_str,dst)
        self.tb.run()

        # check data
        actual_result = dst.data() 
        #print "actual result", actual_result
        #print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)

if __name__ == '__main__':
    gr_unittest.run(qa_symbol_demapper_1seg, "qa_symbol_demapper_1seg.xml")
