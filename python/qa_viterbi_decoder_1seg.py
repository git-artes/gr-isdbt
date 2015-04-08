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
import dvbt

class qa_viterbi_decoder_1seg (gr_unittest.TestCase):

    def setUp (self):
        self.tb1 = gr.top_block ()
        self.tb2 = gr.top_block ()

    def tearDown (self):
        self.tb1 = None
        self.tb2 = None

    def test_001_t (self):
        total_segments = 1
        mode = 3; 
        total_carriers = total_segments*96*2**(mode-1)

        dvbt_inner_coder = dvbt.inner_coder(1, total_carriers, dvbt.QPSK, dvbt.NH, dvbt.C2_3)
        viterbi_decoder = isdbt.viterbi_decoder_1seg()
        #viterbi_decoder = isdbt.viterbi_decoder_1seg_itpp()
       
        #the encoder has a set_output_multiple(4) and each byte generates 6 new symbols (12 bits in this case of QPSK)
        length_src = total_carriers*4/6*10
        src_data = range(256)*(length_src/256) + range(length_src % 256)
        src = blocks.vector_source_b(src_data, False, 1)
        dst = blocks.vector_sink_b(1)
        dst_coded = blocks.vector_sink_b(total_carriers)
        
        self.tb1.connect(src,dvbt_inner_coder)
        self.tb1.connect(dvbt_inner_coder,dst_coded)
        self.tb1.run()

        # I will use only the last coded bits to see what that gives
        slice_coded = dst_coded.data()
        length = 204*8*3/2/2
        offset = 6*70
        print "offset+length: ", offset+length
        slice_coded = slice_coded[0+offset:length+offset]
        
        
        src_sliced = blocks.vector_source_b(slice_coded, False, 1)
        self.tb2.connect(src_sliced,viterbi_decoder)
        self.tb2.connect(viterbi_decoder,dst)
        self.tb2.run()
        # check data
        actual_result = dst.data()
        print "len(src_data): ", len(src_data)
        print "len(coded_data): ", len(dst_coded.data())
        print "len(slice_coded_data): ", len(slice_coded)
        print "len(actual_result)", len(actual_result), "actual_result: ", actual_result


if __name__ == '__main__':
    gr_unittest.run(qa_viterbi_decoder_1seg, "qa_viterbi_decoder_1seg.xml")
