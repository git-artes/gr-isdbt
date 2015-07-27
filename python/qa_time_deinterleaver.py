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

class qa_time_deinterleaver (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_mode3_length4 (self):
        # set up fg

        segments_A = 12; 
        segments_B = 1; 
        segments_C = 0; 
        mode = 3; 
        length_A=4
        length_B=4
        length_C=4
        total_segments = segments_A + segments_B + segments_C
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.time_deinterleaver(mode, segments_A, length_A, segments_B, length_B, segments_C, length_C)

        how_many = 1000
        # the input data are how_many vector (each entry with the same number ranging from 1 to how_many)
        # that increases by one with each new vector
        src_data = [[i+1]*total_carriers for i in range(how_many)]
        src_data = [item for sublist in src_data for item in sublist]
        
        src = blocks.vector_source_c(src_data, False, total_carriers)
        dst = blocks.vector_sink_c(total_carriers)

        self.tb.connect(src,deinterleaver)
        self.tb.connect(deinterleaver,dst)
        self.tb.run()

        actual_result = dst.data()

        for carrier in range(total_carriers): 
            # in each carrier, I should have an increasing number as an output, preceded by 
            # as many zeros as the delay correponding to the particular carrier
            mi = (5*carrier) % 96
            delay = length_A*(95-mi)
            carrier_actual_result = []
            for i in range(how_many): 
                carrier_actual_result.append(actual_result[i*total_carriers+carrier]) 

            carrier_expected_result = [0]*min(how_many,delay)
            carrier_expected_result += range(1,how_many-delay+1)
            #print "carrier_expected_result=", carrier_expected_result
            #print "carrier_actual_result=", carrier_actual_result
            self.assertFloatTuplesAlmostEqual(carrier_expected_result, carrier_actual_result)
 
if __name__ == '__main__':
    gr_unittest.run(qa_time_deinterleaver, "qa_time_deinterleaver.xml")
