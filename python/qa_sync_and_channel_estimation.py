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

class qa_sync_and_channel_estimation (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg

        total_segments = 13; 
        mode = 1; 
        total_carriers = total_segments*108*2**(mode-1)+1
        fft_size = 2**(10+mode)
        sync = isdbt.sync_and_channel_estimation(fft_size, total_carriers,200)

        src_data = range(fft_size)*2
        
        src = blocks.vector_source_c(src_data, False, fft_size)
        dst = blocks.vector_sink_c(total_carriers)

        self.tb.connect(src, sync)
        self.tb.connect(sync,dst)
        self.tb.run()

        actual_result = dst.data()
        #print src_data
        #print actual_result



if __name__ == '__main__':
    gr_unittest.run(qa_sync_and_channel_estimation, "qa_sync_and_channel_estimation.xml")
