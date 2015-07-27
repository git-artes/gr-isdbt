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
import random, math

class qa_symbol_demapper (gr_unittest.TestCase):

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
        mode = 3
        segments_A = 1
        constellation_size_A = 4
        segments_B = 11
        constellation_size_B = 4
        segments_C = 1
        constellation_size_C = 4
        
        demapper = isdbt.symbol_demapper(mode, segments_A, constellation_size_A, segments_B, constellation_size_B, segments_C, constellation_size_C)
        #src_data = (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
        #            0.8 + 1.0j, -0.5 + 0.1j,  0.5 - 1.2j, -0.2 - 1.0j) * 48
        src_syms = [
            1 + 1j,  1 - 1j, -1 - 1j, -1 + 1j,
            1 + 1j, -1 + 1j,  1 - 1j, -1 - 1j ]
        src_syms_noisy = [self.add_noise(z) for z in src_syms]
        src_data = src_syms_noisy*48*13
        # normalización sqrt(2) ?
        expected_result_A = \
            (    0,       1,       3,       2,
                 0,       2,       1,       3) * 48*segments_A
        expected_result_B = \
            (    0,       1,       3,       2,
                 0,       2,       1,       3) * 48*segments_B
        expected_result_C = \
            (    0,       1,       3,       2,
                 0,       2,       1,       3) * 48*segments_C

        src = blocks.vector_source_c(src_data*3, False, 384*13)
        dst_A = blocks.vector_sink_b(384*segments_A)
        dst_B = blocks.vector_sink_b(384*segments_B)
        dst_C = blocks.vector_sink_b(384*segments_C)

        self.tb.connect(src,demapper)
        self.tb.connect((demapper,0),dst_A)
        self.tb.connect((demapper,1),dst_B)
        self.tb.connect((demapper,2),dst_C)
        self.tb.run()

        # check data
        actual_result_A = dst_A.data()
        actual_result_B = dst_B.data()
        actual_result_C = dst_C.data()
        #print "actual result", actual_result
        #print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result_A*3, actual_result_A)
        self.assertFloatTuplesAlmostEqual(expected_result_B*3, actual_result_B)
        self.assertFloatTuplesAlmostEqual(expected_result_C*3, actual_result_C)

    def test_symbol_demapper_16qam (self):
        # set up fg
        mode = 3
        segments_A = 1
        constellation_size_A = 16
        segments_B = 11
        constellation_size_B = 16
        segments_C = 1
        constellation_size_C = 16
        
        demapper = isdbt.symbol_demapper(mode, segments_A, constellation_size_A, segments_B, constellation_size_B, segments_C, constellation_size_C)
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
        src_data = src_syms_noisy*24*13
        src_data = [x / math.sqrt(10.0) for x in src_data]

        expected_result_A = (  \
                8,     10,     2,    0,
                9,     11,     3,    1,
               13,     15,     7,    5,
               12,     14,     6,    4) * 24*segments_A

        expected_result_B = (  \
                8,     10,     2,    0,
                9,     11,     3,    1,
               13,     15,     7,    5,
               12,     14,     6,    4) * 24*segments_B

        expected_result_C = (  \
                8,     10,     2,    0,
                9,     11,     3,    1,
               13,     15,     7,    5,
               12,     14,     6,    4) * 24*segments_C

        src = blocks.vector_source_c(src_data*3, False, 384*13)
        dst_A = blocks.vector_sink_b(384*segments_A)
        dst_B = blocks.vector_sink_b(384*segments_B)
        dst_C = blocks.vector_sink_b(384*segments_C)

        self.tb.connect(src,demapper)
        self.tb.connect((demapper,0),dst_A)
        self.tb.connect((demapper,1),dst_B)
        self.tb.connect((demapper,2),dst_C)
        self.tb.run()

        # check data
        actual_result_A = dst_A.data()
        actual_result_B = dst_B.data()
        actual_result_C = dst_C.data()
        #print "actual result_A", actual_result_A
        #print "expected result_A", expected_result_A
        self.assertFloatTuplesAlmostEqual(expected_result_A*3, actual_result_A)
        self.assertFloatTuplesAlmostEqual(expected_result_B*3, actual_result_B)
        self.assertFloatTuplesAlmostEqual(expected_result_C*3, actual_result_C)

    def test_symbol_demapper_64qam (self):
        # set up fg
        mode = 3
        segments_A = 1
        constellation_size_A = 64
        segments_B = 11
        constellation_size_B = 64
        segments_C = 1
        constellation_size_C = 64
        
        demapper = isdbt.symbol_demapper(mode, segments_A, constellation_size_A, segments_B, constellation_size_B, segments_C, constellation_size_C)
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
        src_data = src_syms_noisy*6*13
        src_data = [x / math.sqrt(42.0) for x in src_data]

        expected_result_A = (  \
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
                20,      22,       30,      28) * 6*segments_A

        expected_result_B = (  \
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
                20,      22,       30,      28) * 6*segments_B

        expected_result_C = (  \
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
                20,      22,       30,      28) * 6*segments_C

        src = blocks.vector_source_c(src_data*3, False, 384*13)
        dst_A = blocks.vector_sink_b(384*segments_A)
        dst_B = blocks.vector_sink_b(384*segments_B)
        dst_C = blocks.vector_sink_b(384*segments_C)

        self.tb.connect(src,demapper)
        self.tb.connect((demapper,0),dst_A)
        self.tb.connect((demapper,1),dst_B)
        self.tb.connect((demapper,2),dst_C)
        self.tb.run()

        # check data
        actual_result_A = dst_A.data()
        actual_result_B = dst_B.data()
        actual_result_C = dst_C.data()
        #print "actual result_A", actual_result_A
        #print "expected result_A", expected_result_A
        self.assertFloatTuplesAlmostEqual(expected_result_A*3, actual_result_A)
        self.assertFloatTuplesAlmostEqual(expected_result_B*3, actual_result_B)
        self.assertFloatTuplesAlmostEqual(expected_result_C*3, actual_result_C)

    def test_symbol_demapper_qpsk_64qam (self):
        # set up fg
        mode = 3
        segments_A = 1
        constellation_size_A = 4
        segments_B = 12
        constellation_size_B = 64
        segments_C = 0
        constellation_size_C = 64
        
        demapper = isdbt.symbol_demapper(mode, segments_A, constellation_size_A, segments_B, constellation_size_B, segments_C, constellation_size_C)

        #src_data = (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
        #            0.8 + 1.0j, -0.5 + 0.1j,  0.5 - 1.2j, -0.2 - 1.0j) * 48
        src_syms = [
            1 + 1j,  1 - 1j, -1 - 1j, -1 + 1j,
            1 + 1j, -1 + 1j,  1 - 1j, -1 - 1j ]
        src_syms_noisy = [self.add_noise(z) for z in src_syms]
        src_data_A = src_syms_noisy * 48*segments_A
        expected_result_A = \
            (    0,       1,       3,       2,
                 0,       2,       1,       3) * 48*segments_A


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
        src_data_B = src_syms_noisy*6*segments_B
        src_data_B = [x / math.sqrt(42.0) for x in src_data_B]

        src_data = src_data_A + src_data_B

        expected_result_B = (  \
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
                20,      22,       30,      28) * 6*segments_B

        src = blocks.vector_source_c(src_data*3, False, 384*13)
        dst_A = blocks.vector_sink_b(384*segments_A)
        dst_B = blocks.vector_sink_b(384*segments_B)

        self.tb.connect(src,demapper)
        self.tb.connect((demapper,0),dst_A)
        self.tb.connect((demapper,1),dst_B)
        self.tb.run()

        # check data
        actual_result_A = dst_A.data()
        actual_result_B = dst_B.data()
        #print "actual result_A", actual_result_A
        #print "expected result_A", expected_result_A
        #print "actual result_B", actual_result_B
        #print "expected result_B", expected_result_B
        self.assertFloatTuplesAlmostEqual(expected_result_A*3, actual_result_A)
        self.assertFloatTuplesAlmostEqual(expected_result_B*3, actual_result_B)


if __name__ == '__main__':
    gr_unittest.run(qa_symbol_demapper, "qa_symbol_demapper.xml")


