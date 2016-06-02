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
import random

class qa_ofdm_synchronization (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    # Testing this block is simply out of question by now. A valid OFDM
    # complete frame should be constructed, which I'm not doing in the 
    # short term. 
    # TODO test it. 
    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_ofdm_synchronization, "qa_ofdm_synchronization.xml")
