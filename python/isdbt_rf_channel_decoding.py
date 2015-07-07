#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: ISDB-T RF Channel Decoding
# Generated: Thu Jul  2 16:56:54 2015
##################################################

from gnuradio import fft
from gnuradio import gr
from gnuradio.fft import window
from gnuradio.filter import firdes
import isdbt

class isdbt_rf_channel_decoding(gr.hier_block2):

    def __init__(self, max_freq_offset=10, guard=0.125, mode=3, snr=10, tmcc_print=False):
        gr.hier_block2.__init__(
           # self, "ISDB-T RF Channel Decoding",
            self, "isdbt_rf_channel_decoding",
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
            gr.io_signature(1, 1, gr.sizeof_gr_complex*13*96*2**(mode-1)),
        )

        ##################################################
        # Parameters
        ##################################################
        self.max_freq_offset = max_freq_offset
        self.guard = guard
        self.mode = mode
        self.snr = snr
        self.tmcc_print = tmcc_print

        ##################################################
        # Variables
        ##################################################
        self.total_carriers = total_carriers = 2**(10+mode)
        self.data_carriers = data_carriers = 13*96*2**(mode-1)
        self.active_carriers = active_carriers = 13*108*2**(mode-1)+1

        ##################################################
        # Blocks
        ##################################################
        self.isdbt_tmcc_decoder_0 = isdbt.tmcc_decoder(mode, tmcc_print)
        self.isdbt_sync_and_channel_estimaton_0 = isdbt.sync_and_channel_estimaton(total_carriers, active_carriers, max_freq_offset)
        self.isdbt_ofdm_sym_acquisition_0 = isdbt.ofdm_sym_acquisition(total_carriers, int(guard*total_carriers), snr)
        self.fft_vxx_0 = fft.fft_vcc(total_carriers, True, (window.rectangular(total_carriers)), True, 1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.fft_vxx_0, 0), (self.isdbt_sync_and_channel_estimaton_0, 0))    
        self.connect((self.isdbt_ofdm_sym_acquisition_0, 0), (self.fft_vxx_0, 0))    
        self.connect((self.isdbt_sync_and_channel_estimaton_0, 0), (self.isdbt_tmcc_decoder_0, 0))    
        self.connect((self.isdbt_tmcc_decoder_0, 0), (self, 0))    
        self.connect((self, 0), (self.isdbt_ofdm_sym_acquisition_0, 0))    

    def get_tmcc_print(self):
        return self.tmcc_print

    def set_tmcc_print(self, tmcc_print):
        self.tmcc_print = tmcc_print

    def get_max_freq_offset(self):
        return self.max_freq_offset

    def set_max_freq_offset(self, max_freq_offset):
        self.max_freq_offset = max_freq_offset

    def get_guard(self):
        return self.guard

    def set_guard(self, guard):
        self.guard = guard

    def get_mode(self):
        return self.mode

    def set_mode(self, mode):
        self.mode = mode
        self.set_total_carriers(2**(10+self.mode))
        self.set_data_carriers(13*96*2**(self.mode-1))
        self.set_active_carriers(13*108*2**(self.mode-1)+1)

    def get_snr(self):
        return self.snr

    def set_snr(self, snr):
        self.snr = snr

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

