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

class qa_time_deinterleaver (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_mode3_length4 (self):
        # set up fg

        total_segments = 1; 
        mode = 3; 
        length=4
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.time_deinterleaver(mode, length)

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
            delay = length*(95-mi)
            carrier_actual_result = []
            for i in range(how_many): 
                carrier_actual_result.append(actual_result[i*total_carriers+carrier]) 

            carrier_expected_result = [0]*min(how_many,delay)
            carrier_expected_result += range(1,how_many-delay+1)
            self.assertFloatTuplesAlmostEqual(carrier_expected_result, carrier_actual_result)
    
    def test_mode3_length2 (self):
        # set up fg

        total_segments = 1; 
        mode = 3; 
        length=2
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.time_deinterleaver(mode, length)

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
            delay = length*(95-mi)
            carrier_actual_result = []
            for i in range(how_many): 
                carrier_actual_result.append(actual_result[i*total_carriers+carrier]) 

            carrier_expected_result = [0]*min(how_many,delay)
            carrier_expected_result += range(1,how_many-delay+1)
            self.assertFloatTuplesAlmostEqual(carrier_expected_result, carrier_actual_result)

    def test_mode3_length1 (self):
        # set up fg

        total_segments = 1; 
        mode = 3; 
        length=2
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.time_deinterleaver(mode, length)

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
            delay = length*(95-mi)
            carrier_actual_result = []
            for i in range(how_many): 
                carrier_actual_result.append(actual_result[i*total_carriers+carrier]) 

            carrier_expected_result = [0]*min(how_many,delay)
            carrier_expected_result += range(1,how_many-delay+1)
            self.assertFloatTuplesAlmostEqual(carrier_expected_result, carrier_actual_result)
    
    def test_mode3_length0 (self):
        # set up fg

        total_segments = 1
        mode = 3 
        length=0
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.time_deinterleaver(mode, length)

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
            delay = length*(95-mi)
            carrier_actual_result = []
            for i in range(how_many): 
                carrier_actual_result.append(actual_result[i*total_carriers+carrier]) 

            carrier_expected_result = [0]*min(how_many,delay)
            carrier_expected_result += range(1,how_many-delay+1)
            self.assertFloatTuplesAlmostEqual(carrier_expected_result, carrier_actual_result)

    def test_mode2_length8 (self):
        # set up fg

        total_segments = 1; 
        mode = 2; 
        length=8
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.time_deinterleaver(mode, length)

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
            delay = length*(95-mi)
            carrier_actual_result = []
            for i in range(how_many): 
                carrier_actual_result.append(actual_result[i*total_carriers+carrier]) 

            carrier_expected_result = [0]*min(how_many,delay)
            carrier_expected_result += range(1,how_many-delay+1)
            self.assertFloatTuplesAlmostEqual(carrier_expected_result, carrier_actual_result)

    def test_mode2_length4 (self):
        # set up fg

        total_segments = 1; 
        mode = 2; 
        length=4
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.time_deinterleaver(mode, length)

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
            delay = length*(95-mi)
            carrier_actual_result = []
            for i in range(how_many): 
                carrier_actual_result.append(actual_result[i*total_carriers+carrier]) 

            carrier_expected_result = [0]*min(how_many,delay)
            carrier_expected_result += range(1,how_many-delay+1)
            self.assertFloatTuplesAlmostEqual(carrier_expected_result, carrier_actual_result)

    def test_mode2_length2 (self):
        # set up fg

        total_segments = 1; 
        mode = 2; 
        length=2
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.time_deinterleaver(mode, length)

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
            delay = length*(95-mi)
            carrier_actual_result = []
            for i in range(how_many): 
                carrier_actual_result.append(actual_result[i*total_carriers+carrier]) 

            carrier_expected_result = [0]*min(how_many,delay)
            carrier_expected_result += range(1,how_many-delay+1)
            self.assertFloatTuplesAlmostEqual(carrier_expected_result, carrier_actual_result)

    def test_mode2_length0 (self):
        # set up fg

        total_segments = 1; 
        mode = 2; 
        length=0
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.time_deinterleaver(mode, length)

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
            delay = length*(95-mi)
            carrier_actual_result = []
            for i in range(how_many): 
                carrier_actual_result.append(actual_result[i*total_carriers+carrier]) 

            carrier_expected_result = [0]*min(how_many,delay)
            carrier_expected_result += range(1,how_many-delay+1)
            self.assertFloatTuplesAlmostEqual(carrier_expected_result, carrier_actual_result)

    def test_mode1_length16 (self):
        # set up fg

        total_segments = 1; 
        mode = 1; 
        length=16
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.time_deinterleaver(mode, length)

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
            delay = length*(95-mi)
            carrier_actual_result = []
            for i in range(how_many): 
                carrier_actual_result.append(actual_result[i*total_carriers+carrier]) 

            carrier_expected_result = [0]*min(how_many,delay)
            carrier_expected_result += range(1,how_many-delay+1)
            self.assertFloatTuplesAlmostEqual(carrier_expected_result, carrier_actual_result)

    def test_mode1_length8 (self):
        # set up fg

        total_segments = 1; 
        mode = 1; 
        length=8
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.time_deinterleaver(mode, length)

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
            delay = length*(95-mi)
            carrier_actual_result = []
            for i in range(how_many): 
                carrier_actual_result.append(actual_result[i*total_carriers+carrier]) 

            carrier_expected_result = [0]*min(how_many,delay)
            carrier_expected_result += range(1,how_many-delay+1)
            self.assertFloatTuplesAlmostEqual(carrier_expected_result, carrier_actual_result)

    def test_mode1_length4 (self):
        # set up fg

        total_segments = 1; 
        mode = 1; 
        length=4
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.time_deinterleaver(mode, length)

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
            delay = length*(95-mi)
            carrier_actual_result = []
            for i in range(how_many): 
                carrier_actual_result.append(actual_result[i*total_carriers+carrier]) 

            carrier_expected_result = [0]*min(how_many,delay)
            carrier_expected_result += range(1,how_many-delay+1)
            self.assertFloatTuplesAlmostEqual(carrier_expected_result, carrier_actual_result)

    def test_mode1_length0 (self):
        # set up fg

        total_segments = 1; 
        mode = 1; 
        length=0
        total_carriers = total_segments*96*2**(mode-1)
        deinterleaver = isdbt.time_deinterleaver(mode, length)

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
            delay = length*(95-mi)
            carrier_actual_result = []
            for i in range(how_many): 
                carrier_actual_result.append(actual_result[i*total_carriers+carrier]) 

            carrier_expected_result = [0]*min(how_many,delay)
            carrier_expected_result += range(1,how_many-delay+1)
            self.assertFloatTuplesAlmostEqual(carrier_expected_result, carrier_actual_result)

if __name__ == '__main__':
    gr_unittest.run(qa_time_deinterleaver, "qa_time_deinterleaver.xml")
