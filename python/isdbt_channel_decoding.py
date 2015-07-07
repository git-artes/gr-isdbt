#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: ISDB-T Channel Decoding
# Generated: Thu Jul  2 17:41:32 2015
##################################################

from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes
import isdbt

class isdbt_channel_decoding(gr.hier_block2):

    def __init__(self, layer_segments=1, mode=3, constellation_size=4, rate=1):
        gr.hier_block2.__init__(
            self, "isdbt_channel_decoding",
            gr.io_signature(1, 1, gr.sizeof_char*layer_segments*96*2**(mode-1)),
            gr.io_signature(1, 1, gr.sizeof_char*1),
        )

        ##################################################
        # Parameters
        ##################################################
        self.layer_segments = layer_segments
        self.mode = mode
        self.constellation_size = constellation_size
        self.rate = rate

        ##################################################
        # Variables
        ##################################################
        self.total_carriers = total_carriers = 2**(10+mode)
        self.data_carriers = data_carriers = 13*96*2**(mode-1)
        self.active_carriers = active_carriers = 13*108*2**(mode-1)+1

        ##################################################
        # Blocks
        ##################################################
        self.isdbt_viterbi_decoder_0 = isdbt.viterbi_decoder(constellation_size, rate)
        self.isdbt_reed_solomon_dec_isdbt_0_0 = isdbt.reed_solomon_dec_isdbt()
        self.isdbt_energy_descrambler_0_0 = isdbt.energy_descrambler()
        self.isdbt_byte_deinterleaver_0_0 = isdbt.byte_deinterleaver()
        self.isdbt_bit_deinterleaver_0_0 = isdbt.bit_deinterleaver(mode, layer_segments, constellation_size)
        self.blocks_vector_to_stream_0_0 = blocks.vector_to_stream(gr.sizeof_char*1, 188)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_vector_to_stream_0_0, 0), (self, 0))    
        self.connect((self.isdbt_bit_deinterleaver_0_0, 0), (self.isdbt_viterbi_decoder_0, 0))    
        self.connect((self.isdbt_byte_deinterleaver_0_0, 0), (self.isdbt_energy_descrambler_0_0, 0))    
        self.connect((self.isdbt_energy_descrambler_0_0, 0), (self.isdbt_reed_solomon_dec_isdbt_0_0, 0))    
        self.connect((self.isdbt_reed_solomon_dec_isdbt_0_0, 0), (self.blocks_vector_to_stream_0_0, 0))    
        self.connect((self.isdbt_viterbi_decoder_0, 0), (self.isdbt_byte_deinterleaver_0_0, 0))    
        self.connect((self, 0), (self.isdbt_bit_deinterleaver_0_0, 0))    


    def get_layer_segments(self):
        return self.layer_segments

    def set_layer_segments(self, layer_segments):
        self.layer_segments = layer_segments

    def get_mode(self):
        return self.mode

    def set_mode(self, mode):
        self.mode = mode
        self.set_data_carriers(13*96*2**(self.mode-1))
        self.set_active_carriers(13*108*2**(self.mode-1)+1)
        self.set_total_carriers(2**(10+self.mode))

    def get_constellation_size(self):
        return self.constellation_size

    def set_constellation_size(self, constellation_size):
        self.constellation_size = constellation_size

    def get_rate(self):
        return self.rate

    def set_rate(self, rate):
        self.rate = rate

    def get_total_carriers(self):
        return self.total_carriers

    def set_total_carriers(self, total_carriers):
        self.total_carriers = total_carriers

    def get_data_carriers(self):
        return self.data_carriers

    def set_data_carriers(self, data_carriers):
        self.data_carriers = data_carriers

    def get_active_carriers(self):
        return self.active_carriers

    def set_active_carriers(self, active_carriers):
        self.active_carriers = active_carriers

