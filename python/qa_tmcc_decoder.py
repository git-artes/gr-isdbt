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

class qa_tmcc_decoder (gr_unittest.TestCase):
        

    def setUp (self):
        self.tb = gr.top_block ()
        # for each segment it indicates where are the tmcc and AC
        self.tmcc_dict_mode3 = {
                0:(101,131,286,349), 
                1:( 31,191,277,409), 
                2:( 17,194,260,371), 
                3:( 86,152,263,373), 
                4:( 49,139,299,385),
                5:( 23,178,241,341),
                6:( 85,209,239,394), 
                7:( 83,169,301,425),
                8:( 25,125,302,368),
                9:( 44,155,265,355),
                10:(47,157,247,407),
                11:(70,133,233,410),
                12:(61,193,317,347)}
        self.ac_dict_mode3 = {
                0:(  7,  89, 206, 209, 226, 244, 377, 407), 
                1:( 76,  97, 112, 197, 256, 305, 332, 388), 
                2:( 61, 100, 119, 209, 236, 256, 398, 424), 
                3:( 11, 101, 128, 148, 290, 316, 359, 403), 
                4:( 35,  79, 184, 205, 220, 305, 364, 413),
                5:( 98, 101, 118, 136, 269, 299, 385, 424),
                6:( 8,   64, 115, 197, 314, 317, 334, 352), 
                7:( 4,   89, 148, 197, 224, 280, 331, 413),
                8:( 53,  83, 169, 208, 227, 317, 344, 364),
                9:( 20,  40, 182, 208, 251, 295, 400, 421),
                10:(74, 100, 143, 187, 292, 313, 328, 413),
                11:(10,  28, 161, 191, 277, 316, 335, 425),
                12:(40,  89, 116, 172, 223, 305, 422, 425)}

        # for each segment it indicates where are the tmcc and AC
        self.tmcc_dict_mode2 = {
                0:(  23, 178), 
                1:(  85, 209), 
                2:(  25, 125), 
                3:(  83, 169), 
                4:(  86, 152),
                5:(  49, 139),
                6:(  47, 157), 
                7:(  44, 155),
                8:(  31, 191),
                9:(  17, 194),
                10:( 61, 193),
                11:( 70, 133),
                12:(101, 131)}
        self.ac_dict_mode2 = {
                0:( 98, 101, 118, 136), 
                1:(  8,  64, 115, 197), 
                2:( 53,  83, 169, 208), 
                3:(  4,  89, 148, 197), 
                4:( 11, 101, 128, 148),
                5:( 35,  79, 184, 205),
                6:( 74, 100, 143, 187), 
                7:( 20,  40, 182, 208),
                8:( 76,  97, 112, 197),
                9:( 61, 100, 119, 209),
                10:(40,  89, 116, 172),
                11:(10,  28, 161, 191),
                12:( 7,  89, 206, 209)}

        # for each segment it indicates where are the tmcc and AC
        self.tmcc_dict_mode1 = {
                0: (49,), 
                1: (47,), 
                2: (31,), 
                3: (44,), 
                4: (83,),
                5: (86,),
                6: (61,), 
                7: (17,),
                8: (85,),
                9: (25,),
                10:(101,),
                11:(70,),
                12:(23,)}
        self.ac_dict_mode1 = {
                0:( 35,  79), 
                1:( 74, 100), 
                2:( 76,  97), 
                3:( 20,  40), 
                4:(  4,  89),
                5:( 11, 101),
                6:( 40,  89), 
                7:( 61, 100),
                8:(  8,  64),
                9:( 53,  83),
                10:( 7,  89),
                11:(10,  28),
                12:(98, 101)}

    def tearDown (self):
        self.tb = None
        self.tmcc_dict = None
        self.ac_dict = None

    def test_sym0_mode3 (self):
        # set up fg
        total_segments = 13; 
        mode = 3;

        total_data_carriers = total_segments*96*2**(mode-1)
        total_carriers = total_segments*108*2**(mode-1)+1
        tmcc_decoder = isdbt.tmcc_decoder(mode)

        # the input are the carrier index in each carrier. The
        # expected result in this case is easy to calculate (see below)
        src_data = range(total_carriers)*2
        
        sym_index = 0
        tag = gr.tag_utils.python_to_tag({'offset':0, 'key':gr.pmt.string_to_symbol("relative_symbol_index"), 'value':gr.pmt.from_long(sym_index)})
        src = blocks.vector_source_c(src_data, False, total_carriers, [tag])
        dst = blocks.vector_sink_c(total_data_carriers)

        self.tb.connect(src,tmcc_decoder)
        self.tb.connect(tmcc_decoder,dst)
        self.tb.run()

        actual_result = dst.data()
        
        # the expected result are all data carriers (indexed as originally when all carriers were present)
        disordered_segments = (11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12)
        expected_result = range(total_carriers)
        # I remove the CP
        expected_result.remove(total_carriers-1)
        # I remove the SP
        sp_list = [12*i+3*sym_index for i in range((total_carriers-1)/12)]
        expected_result = [item for item in expected_result if item not in sp_list]
        # i then remove the tmcc
        tmcc_list = []
        for segment in range(13):
            tmcc_segment = self.tmcc_dict_mode3[disordered_segments[segment]]
            tmcc_list += [segment*(total_carriers-1)/13+tmcc_segment[i] for i in range(len(tmcc_segment))]
        expected_result = [item for item in expected_result if item not in tmcc_list]
        # I remove the AC
        ac_list = []
        for segment in range(13):
            ac_segment = self.ac_dict_mode3[disordered_segments[segment]]
            ac_list += [segment*(total_carriers-1)/13+ac_segment[i] for i in range(len(ac_segment))]
        expected_result = [item for item in expected_result if item not in ac_list]

        # I finally re-arrange the segments
        reordered_expected_result = []
        for segment in range(13):
            where = disordered_segments.index(segment)
            reordered_expected_result = reordered_expected_result + expected_result[where*total_data_carriers/13:(where+1)*total_data_carriers/13]

        #print "expected: ", reordered_expected_result
        #print "actual: ", actual_result
        #print "diff: ", [ai-bi for ai,bi in zip(reordered_expected_result,actual_result)]

        self.assertFloatTuplesAlmostEqual(reordered_expected_result*2, actual_result)


    def test_sym1_mode3 (self):
        # set up fg
        total_segments = 13; 
        mode = 3;

        total_data_carriers = total_segments*96*2**(mode-1)
        total_carriers = total_segments*108*2**(mode-1)+1
        tmcc_decoder = isdbt.tmcc_decoder(mode)

        # the input are the carrier index in each carrier. The
        # expected result in this case is easy to calculate (see below)
        src_data = range(total_carriers)*2
        
        sym_index = 1
        tag = gr.tag_utils.python_to_tag({'offset':0, 'key':gr.pmt.string_to_symbol("relative_symbol_index"), 'value':gr.pmt.from_long(sym_index)})
        src = blocks.vector_source_c(src_data, False, total_carriers, [tag])
        dst = blocks.vector_sink_c(total_data_carriers)

        self.tb.connect(src,tmcc_decoder)
        self.tb.connect(tmcc_decoder,dst)
        self.tb.run()

        actual_result = dst.data()
        
        # the expected result are all data carriers (indexed as originally when all carriers were present)
        disordered_segments = (11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12)
        expected_result = range(total_carriers)
        # I remove the CP
        expected_result.remove(total_carriers-1)
        # I remove the SP
        sp_list = [12*i+3*sym_index for i in range((total_carriers-1)/12)]
        expected_result = [item for item in expected_result if item not in sp_list]
        # i then remove the tmcc
        tmcc_list = []
        for segment in range(13):
            tmcc_segment = self.tmcc_dict_mode3[disordered_segments[segment]]
            tmcc_list += [segment*(total_carriers-1)/13+tmcc_segment[i] for i in range(len(tmcc_segment))]
        expected_result = [item for item in expected_result if item not in tmcc_list]
        # I remove the AC
        ac_list = []
        for segment in range(13):
            ac_segment = self.ac_dict_mode3[disordered_segments[segment]]
            ac_list += [segment*(total_carriers-1)/13+ac_segment[i] for i in range(len(ac_segment))]
        expected_result = [item for item in expected_result if item not in ac_list]

        # I finally re-arrange the segments
        reordered_expected_result = []
        for segment in range(13):
            where = disordered_segments.index(segment)
            reordered_expected_result = reordered_expected_result + expected_result[where*total_data_carriers/13:(where+1)*total_data_carriers/13]
        
        #print actual_result
        self.assertFloatTuplesAlmostEqual(reordered_expected_result*2, actual_result)


    def test_sym2_mode3 (self):
        # set up fg
        total_segments = 13; 
        mode = 3;

        total_data_carriers = total_segments*96*2**(mode-1)
        total_carriers = total_segments*108*2**(mode-1)+1
        tmcc_decoder = isdbt.tmcc_decoder(mode)

        # the input are the carrier index in each carrier. The
        # expected result in this case is easy to calculate (see below)
        src_data = range(total_carriers)*2
        
        sym_index = 2
        tag = gr.tag_utils.python_to_tag({'offset':0, 'key':gr.pmt.string_to_symbol("relative_symbol_index"), 'value':gr.pmt.from_long(sym_index)})
        src = blocks.vector_source_c(src_data, False, total_carriers, [tag])
        dst = blocks.vector_sink_c(total_data_carriers)

        self.tb.connect(src,tmcc_decoder)
        self.tb.connect(tmcc_decoder,dst)
        self.tb.run()

        actual_result = dst.data()
        
        # the expected result are all data carriers (indexed as originally when all carriers were present)
        disordered_segments = (11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12)
        expected_result = range(total_carriers)
        # I remove the CP
        expected_result.remove(total_carriers-1)
        # I remove the SP
        sp_list = [12*i+3*sym_index for i in range((total_carriers-1)/12)]
        expected_result = [item for item in expected_result if item not in sp_list]
        # i then remove the tmcc
        tmcc_list = []
        for segment in range(13):
            tmcc_segment = self.tmcc_dict_mode3[disordered_segments[segment]]
            tmcc_list += [segment*(total_carriers-1)/13+tmcc_segment[i] for i in range(len(tmcc_segment))]
        expected_result = [item for item in expected_result if item not in tmcc_list]
        # I remove the AC
        ac_list = []
        for segment in range(13):
            ac_segment = self.ac_dict_mode3[disordered_segments[segment]]
            ac_list += [segment*(total_carriers-1)/13+ac_segment[i] for i in range(len(ac_segment))]
        expected_result = [item for item in expected_result if item not in ac_list]

        # I finally re-arrange the segments
        reordered_expected_result = []
        for segment in range(13):
            where = disordered_segments.index(segment)
            reordered_expected_result = reordered_expected_result + expected_result[where*total_data_carriers/13:(where+1)*total_data_carriers/13]

        self.assertFloatTuplesAlmostEqual(reordered_expected_result*2, actual_result)

    def test_sym3_mode3 (self):
        # set up fg
        total_segments = 13; 
        mode = 3;

        total_data_carriers = total_segments*96*2**(mode-1)
        total_carriers = total_segments*108*2**(mode-1)+1
        tmcc_decoder = isdbt.tmcc_decoder(mode)

        # the input are the carrier index in each carrier. The
        # expected result in this case is easy to calculate (see below)
        src_data = range(total_carriers)*2
        
        sym_index = 3
        tag = gr.tag_utils.python_to_tag({'offset':0, 'key':gr.pmt.string_to_symbol("relative_symbol_index"), 'value':gr.pmt.from_long(sym_index)})
        src = blocks.vector_source_c(src_data, False, total_carriers, [tag])
        dst = blocks.vector_sink_c(total_data_carriers)

        self.tb.connect(src,tmcc_decoder)
        self.tb.connect(tmcc_decoder,dst)
        self.tb.run()

        actual_result = dst.data()
        
        # the expected result are all data carriers (indexed as originally when all carriers were present)
        disordered_segments = (11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12)
        expected_result = range(total_carriers)
        # I remove the CP
        expected_result.remove(total_carriers-1)
        # I remove the SP
        sp_list = [12*i+3*sym_index for i in range((total_carriers-1)/12)]
        expected_result = [item for item in expected_result if item not in sp_list]
        # i then remove the tmcc
        tmcc_list = []
        for segment in range(13):
            tmcc_segment = self.tmcc_dict_mode3[disordered_segments[segment]]
            tmcc_list += [segment*(total_carriers-1)/13+tmcc_segment[i] for i in range(len(tmcc_segment))]
        expected_result = [item for item in expected_result if item not in tmcc_list]
        # I remove the AC
        ac_list = []
        for segment in range(13):
            ac_segment = self.ac_dict_mode3[disordered_segments[segment]]
            ac_list += [segment*(total_carriers-1)/13+ac_segment[i] for i in range(len(ac_segment))]
        expected_result = [item for item in expected_result if item not in ac_list]

        # I finally re-arrange the segments
        reordered_expected_result = []
        for segment in range(13):
            where = disordered_segments.index(segment)
            reordered_expected_result = reordered_expected_result + expected_result[where*total_data_carriers/13:(where+1)*total_data_carriers/13]

        self.assertFloatTuplesAlmostEqual(reordered_expected_result*2, actual_result)


    def test_sym0_mode2 (self):
        # set up fg
        total_segments = 13; 
        mode = 2;

        total_data_carriers = total_segments*96*2**(mode-1)
        total_carriers = total_segments*108*2**(mode-1)+1
        tmcc_decoder = isdbt.tmcc_decoder(mode)

        # the input are the carrier index in each carrier. The
        # expected result in this case is easy to calculate (see below)
        src_data = range(total_carriers)*2
        
        sym_index = 0
        tag = gr.tag_utils.python_to_tag({'offset':0, 'key':gr.pmt.string_to_symbol("relative_symbol_index"), 'value':gr.pmt.from_long(sym_index)})
        src = blocks.vector_source_c(src_data, False, total_carriers, [tag])
        dst = blocks.vector_sink_c(total_data_carriers)

        self.tb.connect(src,tmcc_decoder)
        self.tb.connect(tmcc_decoder,dst)
        self.tb.run()

        actual_result = dst.data()
        
        # the expected result are all data carriers (indexed as originally when all carriers were present)
        disordered_segments = (11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12)
        expected_result = range(total_carriers)
        # I remove the CP
        expected_result.remove(total_carriers-1)
        # I remove the SP
        sp_list = [12*i+3*sym_index for i in range((total_carriers-1)/12)]
        expected_result = [item for item in expected_result if item not in sp_list]
        # i then remove the tmcc
        tmcc_list = []
        for segment in range(13):
            tmcc_segment = self.tmcc_dict_mode2[disordered_segments[segment]]
            tmcc_list += [segment*(total_carriers-1)/13+tmcc_segment[i] for i in range(len(tmcc_segment))]
        expected_result = [item for item in expected_result if item not in tmcc_list]
        # I remove the AC
        ac_list = []
        for segment in range(13):
            ac_segment = self.ac_dict_mode2[disordered_segments[segment]]
            ac_list += [segment*(total_carriers-1)/13+ac_segment[i] for i in range(len(ac_segment))]
        expected_result = [item for item in expected_result if item not in ac_list]

        # I finally re-arrange the segments
        reordered_expected_result = []
        for segment in range(13):
            where = disordered_segments.index(segment)
            reordered_expected_result = reordered_expected_result + expected_result[where*total_data_carriers/13:(where+1)*total_data_carriers/13]

        self.assertFloatTuplesAlmostEqual(reordered_expected_result*2, actual_result)

    def test_sym1_mode2 (self):
        # set up fg
        total_segments = 13; 
        mode = 2;

        total_data_carriers = total_segments*96*2**(mode-1)
        total_carriers = total_segments*108*2**(mode-1)+1
        tmcc_decoder = isdbt.tmcc_decoder(mode)

        # the input are the carrier index in each carrier. The
        # expected result in this case is easy to calculate (see below)
        src_data = range(total_carriers)*2
        
        sym_index = 1
        tag = gr.tag_utils.python_to_tag({'offset':0, 'key':gr.pmt.string_to_symbol("relative_symbol_index"), 'value':gr.pmt.from_long(sym_index)})
        src = blocks.vector_source_c(src_data, False, total_carriers, [tag])
        dst = blocks.vector_sink_c(total_data_carriers)

        self.tb.connect(src,tmcc_decoder)
        self.tb.connect(tmcc_decoder,dst)
        self.tb.run()

        actual_result = dst.data()
        
        # the expected result are all data carriers (indexed as originally when all carriers were present)
        disordered_segments = (11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12)
        expected_result = range(total_carriers)
        # I remove the CP
        expected_result.remove(total_carriers-1)
        # I remove the SP
        sp_list = [12*i+3*sym_index for i in range((total_carriers-1)/12)]
        expected_result = [item for item in expected_result if item not in sp_list]
        # i then remove the tmcc
        tmcc_list = []
        for segment in range(13):
            tmcc_segment = self.tmcc_dict_mode2[disordered_segments[segment]]
            tmcc_list += [segment*(total_carriers-1)/13+tmcc_segment[i] for i in range(len(tmcc_segment))]
        expected_result = [item for item in expected_result if item not in tmcc_list]
        # I remove the AC
        ac_list = []
        for segment in range(13):
            ac_segment = self.ac_dict_mode2[disordered_segments[segment]]
            ac_list += [segment*(total_carriers-1)/13+ac_segment[i] for i in range(len(ac_segment))]
        expected_result = [item for item in expected_result if item not in ac_list]

        # I finally re-arrange the segments
        reordered_expected_result = []
        for segment in range(13):
            where = disordered_segments.index(segment)
            reordered_expected_result = reordered_expected_result + expected_result[where*total_data_carriers/13:(where+1)*total_data_carriers/13]

        self.assertFloatTuplesAlmostEqual(reordered_expected_result*2, actual_result)

    def test_sym2_mode2 (self):
        # set up fg
        total_segments = 13; 
        mode = 2;

        total_data_carriers = total_segments*96*2**(mode-1)
        total_carriers = total_segments*108*2**(mode-1)+1
        tmcc_decoder = isdbt.tmcc_decoder(mode)

        # the input are the carrier index in each carrier. The
        # expected result in this case is easy to calculate (see below)
        src_data = range(total_carriers)*2
        
        sym_index = 2
        tag = gr.tag_utils.python_to_tag({'offset':0, 'key':gr.pmt.string_to_symbol("relative_symbol_index"), 'value':gr.pmt.from_long(sym_index)})
        src = blocks.vector_source_c(src_data, False, total_carriers, [tag])
        dst = blocks.vector_sink_c(total_data_carriers)

        self.tb.connect(src,tmcc_decoder)
        self.tb.connect(tmcc_decoder,dst)
        self.tb.run()

        actual_result = dst.data()
        
        # the expected result are all data carriers (indexed as originally when all carriers were present)
        disordered_segments = (11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12)
        expected_result = range(total_carriers)
        # I remove the CP
        expected_result.remove(total_carriers-1)
        # I remove the SP
        sp_list = [12*i+3*sym_index for i in range((total_carriers-1)/12)]
        expected_result = [item for item in expected_result if item not in sp_list]
        # i then remove the tmcc
        tmcc_list = []
        for segment in range(13):
            tmcc_segment = self.tmcc_dict_mode2[disordered_segments[segment]]
            tmcc_list += [segment*(total_carriers-1)/13+tmcc_segment[i] for i in range(len(tmcc_segment))]
        expected_result = [item for item in expected_result if item not in tmcc_list]
        # I remove the AC
        ac_list = []
        for segment in range(13):
            ac_segment = self.ac_dict_mode2[disordered_segments[segment]]
            ac_list += [segment*(total_carriers-1)/13+ac_segment[i] for i in range(len(ac_segment))]
        expected_result = [item for item in expected_result if item not in ac_list]

        # I finally re-arrange the segments
        reordered_expected_result = []
        for segment in range(13):
            where = disordered_segments.index(segment)
            reordered_expected_result = reordered_expected_result + expected_result[where*total_data_carriers/13:(where+1)*total_data_carriers/13]

        self.assertFloatTuplesAlmostEqual(reordered_expected_result*2, actual_result)

    def test_sym3_mode2 (self):
        # set up fg
        total_segments = 13; 
        mode = 2;

        total_data_carriers = total_segments*96*2**(mode-1)
        total_carriers = total_segments*108*2**(mode-1)+1
        tmcc_decoder = isdbt.tmcc_decoder(mode)

        # the input are the carrier index in each carrier. The
        # expected result in this case is easy to calculate (see below)
        src_data = range(total_carriers)*2
        
        sym_index = 3
        tag = gr.tag_utils.python_to_tag({'offset':0, 'key':gr.pmt.string_to_symbol("relative_symbol_index"), 'value':gr.pmt.from_long(sym_index)})
        src = blocks.vector_source_c(src_data, False, total_carriers, [tag])
        dst = blocks.vector_sink_c(total_data_carriers)

        self.tb.connect(src,tmcc_decoder)
        self.tb.connect(tmcc_decoder,dst)
        self.tb.run()

        actual_result = dst.data()
        
        # the expected result are all data carriers (indexed as originally when all carriers were present)
        disordered_segments = (11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12)
        expected_result = range(total_carriers)
        # I remove the CP
        expected_result.remove(total_carriers-1)
        # I remove the SP
        sp_list = [12*i+3*sym_index for i in range((total_carriers-1)/12)]
        expected_result = [item for item in expected_result if item not in sp_list]
        # i then remove the tmcc
        tmcc_list = []
        for segment in range(13):
            tmcc_segment = self.tmcc_dict_mode2[disordered_segments[segment]]
            tmcc_list += [segment*(total_carriers-1)/13+tmcc_segment[i] for i in range(len(tmcc_segment))]
        expected_result = [item for item in expected_result if item not in tmcc_list]
        # I remove the AC
        ac_list = []
        for segment in range(13):
            ac_segment = self.ac_dict_mode2[disordered_segments[segment]]
            ac_list += [segment*(total_carriers-1)/13+ac_segment[i] for i in range(len(ac_segment))]
        expected_result = [item for item in expected_result if item not in ac_list]

        # I finally re-arrange the segments
        reordered_expected_result = []
        for segment in range(13):
            where = disordered_segments.index(segment)
            reordered_expected_result = reordered_expected_result + expected_result[where*total_data_carriers/13:(where+1)*total_data_carriers/13]

        self.assertFloatTuplesAlmostEqual(reordered_expected_result*2, actual_result)

    def test_sym0_mode1 (self):
        # set up fg
        total_segments = 13; 
        mode = 1;

        total_data_carriers = total_segments*96*2**(mode-1)
        total_carriers = total_segments*108*2**(mode-1)+1
        tmcc_decoder = isdbt.tmcc_decoder(mode)

        # the input are the carrier index in each carrier. The
        # expected result in this case is easy to calculate (see below)
        src_data = range(total_carriers)*2
        
        sym_index = 0
        tag = gr.tag_utils.python_to_tag({'offset':0, 'key':gr.pmt.string_to_symbol("relative_symbol_index"), 'value':gr.pmt.from_long(sym_index)})
        src = blocks.vector_source_c(src_data, False, total_carriers, [tag])
        dst = blocks.vector_sink_c(total_data_carriers)

        self.tb.connect(src,tmcc_decoder)
        self.tb.connect(tmcc_decoder,dst)
        self.tb.run()

        actual_result = dst.data()
        
        # the expected result are all data carriers (indexed as originally when all carriers were present)
        disordered_segments = (11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12)
        expected_result = range(total_carriers)
        # I remove the CP
        expected_result.remove(total_carriers-1)
        # I remove the SP
        sp_list = [12*i+3*sym_index for i in range((total_carriers-1)/12)]
        expected_result = [item for item in expected_result if item not in sp_list]
        # i then remove the tmcc
        tmcc_list = []
        for segment in range(13):
            tmcc_segment = self.tmcc_dict_mode1[disordered_segments[segment]]
            tmcc_list += [segment*(total_carriers-1)/13+tmcc_segment[i] for i in range(len(tmcc_segment))]
        expected_result = [item for item in expected_result if item not in tmcc_list]
        # I remove the AC
        ac_list = []
        for segment in range(13):
            ac_segment = self.ac_dict_mode1[disordered_segments[segment]]
            ac_list += [segment*(total_carriers-1)/13+ac_segment[i] for i in range(len(ac_segment))]
        expected_result = [item for item in expected_result if item not in ac_list]

        # I finally re-arrange the segments
        reordered_expected_result = []
        for segment in range(13):
            where = disordered_segments.index(segment)
            reordered_expected_result = reordered_expected_result + expected_result[where*total_data_carriers/13:(where+1)*total_data_carriers/13]

        self.assertFloatTuplesAlmostEqual(reordered_expected_result*2, actual_result)

    def test_sym1_mode1 (self):
        # set up fg
        total_segments = 13; 
        mode = 1;

        total_data_carriers = total_segments*96*2**(mode-1)
        total_carriers = total_segments*108*2**(mode-1)+1
        tmcc_decoder = isdbt.tmcc_decoder(mode)

        # the input are the carrier index in each carrier. The
        # expected result in this case is easy to calculate (see below)
        src_data = range(total_carriers)*2
        
        sym_index = 1
        tag = gr.tag_utils.python_to_tag({'offset':0, 'key':gr.pmt.string_to_symbol("relative_symbol_index"), 'value':gr.pmt.from_long(sym_index)})
        src = blocks.vector_source_c(src_data, False, total_carriers, [tag])
        dst = blocks.vector_sink_c(total_data_carriers)

        self.tb.connect(src,tmcc_decoder)
        self.tb.connect(tmcc_decoder,dst)
        self.tb.run()

        actual_result = dst.data()
        
        # the expected result are all data carriers (indexed as originally when all carriers were present)
        disordered_segments = (11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12)
        expected_result = range(total_carriers)
        # I remove the CP
        expected_result.remove(total_carriers-1)
        # I remove the SP
        sp_list = [12*i+3*sym_index for i in range((total_carriers-1)/12)]
        expected_result = [item for item in expected_result if item not in sp_list]
        # i then remove the tmcc
        tmcc_list = []
        for segment in range(13):
            tmcc_segment = self.tmcc_dict_mode1[disordered_segments[segment]]
            tmcc_list += [segment*(total_carriers-1)/13+tmcc_segment[i] for i in range(len(tmcc_segment))]
        expected_result = [item for item in expected_result if item not in tmcc_list]
        # I remove the AC
        ac_list = []
        for segment in range(13):
            ac_segment = self.ac_dict_mode1[disordered_segments[segment]]
            ac_list += [segment*(total_carriers-1)/13+ac_segment[i] for i in range(len(ac_segment))]
        expected_result = [item for item in expected_result if item not in ac_list]

        # I finally re-arrange the segments
        reordered_expected_result = []
        for segment in range(13):
            where = disordered_segments.index(segment)
            reordered_expected_result = reordered_expected_result + expected_result[where*total_data_carriers/13:(where+1)*total_data_carriers/13]

        self.assertFloatTuplesAlmostEqual(reordered_expected_result*2, actual_result)

    def test_sym2_mode1 (self):
        # set up fg
        total_segments = 13; 
        mode = 1;

        total_data_carriers = total_segments*96*2**(mode-1)
        total_carriers = total_segments*108*2**(mode-1)+1
        tmcc_decoder = isdbt.tmcc_decoder(mode)

        # the input are the carrier index in each carrier. The
        # expected result in this case is easy to calculate (see below)
        src_data = range(total_carriers)*2
        
        sym_index = 2
        tag = gr.tag_utils.python_to_tag({'offset':0, 'key':gr.pmt.string_to_symbol("relative_symbol_index"), 'value':gr.pmt.from_long(sym_index)})
        src = blocks.vector_source_c(src_data, False, total_carriers, [tag])
        dst = blocks.vector_sink_c(total_data_carriers)

        self.tb.connect(src,tmcc_decoder)
        self.tb.connect(tmcc_decoder,dst)
        self.tb.run()

        actual_result = dst.data()
        
        # the expected result are all data carriers (indexed as originally when all carriers were present)
        disordered_segments = (11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12)
        expected_result = range(total_carriers)
        # I remove the CP
        expected_result.remove(total_carriers-1)
        # I remove the SP
        sp_list = [12*i+3*sym_index for i in range((total_carriers-1)/12)]
        expected_result = [item for item in expected_result if item not in sp_list]
        # i then remove the tmcc
        tmcc_list = []
        for segment in range(13):
            tmcc_segment = self.tmcc_dict_mode1[disordered_segments[segment]]
            tmcc_list += [segment*(total_carriers-1)/13+tmcc_segment[i] for i in range(len(tmcc_segment))]
        expected_result = [item for item in expected_result if item not in tmcc_list]
        # I remove the AC
        ac_list = []
        for segment in range(13):
            ac_segment = self.ac_dict_mode1[disordered_segments[segment]]
            ac_list += [segment*(total_carriers-1)/13+ac_segment[i] for i in range(len(ac_segment))]
        expected_result = [item for item in expected_result if item not in ac_list]

        # I finally re-arrange the segments
        reordered_expected_result = []
        for segment in range(13):
            where = disordered_segments.index(segment)
            reordered_expected_result = reordered_expected_result + expected_result[where*total_data_carriers/13:(where+1)*total_data_carriers/13]

        self.assertFloatTuplesAlmostEqual(reordered_expected_result*2, actual_result)

    def test_sym3_mode1 (self):
        # set up fg
        total_segments = 13; 
        mode = 1;

        total_data_carriers = total_segments*96*2**(mode-1)
        total_carriers = total_segments*108*2**(mode-1)+1
        tmcc_decoder = isdbt.tmcc_decoder(mode)

        # the input are the carrier index in each carrier. The
        # expected result in this case is easy to calculate (see below)
        src_data = range(total_carriers)*2
        
        sym_index = 3
        tag = gr.tag_utils.python_to_tag({'offset':0, 'key':gr.pmt.string_to_symbol("relative_symbol_index"), 'value':gr.pmt.from_long(sym_index)})
        src = blocks.vector_source_c(src_data, False, total_carriers, [tag])
        dst = blocks.vector_sink_c(total_data_carriers)

        self.tb.connect(src,tmcc_decoder)
        self.tb.connect(tmcc_decoder,dst)
        self.tb.run()

        actual_result = dst.data()
        
        # the expected result are all data carriers (indexed as originally when all carriers were present)
        disordered_segments = (11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12)
        expected_result = range(total_carriers)
        # I remove the CP
        expected_result.remove(total_carriers-1)
        # I remove the SP
        sp_list = [12*i+3*sym_index for i in range((total_carriers-1)/12)]
        expected_result = [item for item in expected_result if item not in sp_list]
        # i then remove the tmcc
        tmcc_list = []
        for segment in range(13):
            tmcc_segment = self.tmcc_dict_mode1[disordered_segments[segment]]
            tmcc_list += [segment*(total_carriers-1)/13+tmcc_segment[i] for i in range(len(tmcc_segment))]
        expected_result = [item for item in expected_result if item not in tmcc_list]
        # I remove the AC
        ac_list = []
        for segment in range(13):
            ac_segment = self.ac_dict_mode1[disordered_segments[segment]]
            ac_list += [segment*(total_carriers-1)/13+ac_segment[i] for i in range(len(ac_segment))]
        expected_result = [item for item in expected_result if item not in ac_list]

        # I finally re-arrange the segments
        reordered_expected_result = []
        for segment in range(13):
            where = disordered_segments.index(segment)
            reordered_expected_result = reordered_expected_result + expected_result[where*total_data_carriers/13:(where+1)*total_data_carriers/13]

        self.assertFloatTuplesAlmostEqual(reordered_expected_result*2, actual_result)



if __name__ == '__main__':
    gr_unittest.run(qa_tmcc_decoder, "qa_tmcc_decoder.xml")
