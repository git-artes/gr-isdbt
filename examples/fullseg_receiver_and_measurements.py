#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Fullseg Receiver And Measurements
# Generated: Wed Jul 15 16:44:39 2015
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio import wxgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.wxgui import forms
from gnuradio.wxgui import numbersink2
from gnuradio.wxgui import scopesink2
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import isdbt
import mer
import numpy as np
import wx

class fullseg_receiver_and_measurements(grc_wxgui.top_block_gui):

    def __init__(self):
        grc_wxgui.top_block_gui.__init__(self, title="Fullseg Receiver And Measurements")
        _icon_path = "/usr/share/icons/hicolor/32x32/apps/gnuradio-grc.png"
        self.SetIcon(wx.Icon(_icon_path, wx.BITMAP_TYPE_ANY))

        ##################################################
        # Variables
        ##################################################
        self.mode = mode = 3
        self.total_carriers = total_carriers = 2**(10+mode)
        self.symbol_table_QPSK = symbol_table_QPSK = (1+1j, 1-1j, -1+1j, -1-1j)/np.sqrt(2)
        self.symbol_table_64QAM = symbol_table_64QAM = (7+7j, 7+5j, 5+7j, 5+5j, 7+1j, 7+3j, 5+1j, 5+3j, 1+7j, 1+5j, 3+7j, 3+5j, 1+1j, 1+3j, 3+1j, 3+3j, 7-7j, 7-5j, 5-7j, 5-5j, 7-1j, 7-3j, 5-1j, 5-3j, 1-7j, 1-5j, 3-7j, 3-5j, 1-1j, 1-3j, 3-1j, 3-3j, -7+7j, -7+5j, -5+7j, -5+5j, -7+1j, -7+3j, -5+1j, -5+3j, -1+7j, -1+5j, -3+7j, -3+5j, -1+1j, -1+3j, -3+1j, -3+3j, -7-7j, -7-5j, -5-7j, -5-5j, -7-1j, -7-3j, -5-1j, -5-3j, -1-7j, -1-5j, -3-7j, -3-5j, -1-1j, -1-3j, -3-1j, -3-3j)/np.sqrt(42)
        self.symbol_table_16QAM = symbol_table_16QAM = (3+3j, 3+1j, 1+3j, 1+1j, 3-3j, 3-1j, 1-3j, 1-1j, -3+3j, -3+1j, -1+3j, -1+1j, -3-3j, -3-1j, -1-3j, -1-1j)/np.sqrt(10)
        self.samp_usrp = samp_usrp = 6400000
        self.samp_rate = samp_rate = 6.4e6*80/63
        self.inter = inter = 80
        self.guard = guard = 1.0/8
        self.decim = decim = 63
        self.data_carriers = data_carriers = 13*96*2**(mode-1)
        self.center_freq = center_freq = 575143000
        self.active_carriers = active_carriers = 13*108*2**(mode-1)+1

        ##################################################
        # Blocks
        ##################################################
        self.n_0 = self.n_0 = wx.Notebook(self.GetWin(), style=wx.NB_TOP)
        self.n_0.AddPage(grc_wxgui.Panel(self.n_0), "Measurements")
        self.Add(self.n_0)
        self.wxgui_scopesink2_0_1_0 = scopesink2.scope_sink_c(
        	self.GetWin(),
        	title="Constellation",
        	sample_rate=float(samp_rate*(1-guard))/13,
        	v_scale=0.5,
        	v_offset=0,
        	t_scale=0,
        	ac_couple=False,
        	xy_mode=True,
        	num_inputs=1,
        	trig_mode=wxgui.TRIG_MODE_AUTO,
        	y_axis_label="Counts",
        )
        self.Add(self.wxgui_scopesink2_0_1_0.win)
        self.wxgui_numbersink2_1_0_2 = numbersink2.number_sink_f(
        	self.n_0.GetPage(0).GetWin(),
        	unit="voltage",
        	minval=0,
        	maxval=1,
        	factor=1.0,
        	decimal_places=3,
        	ref_level=0,
        	sample_rate=samp_rate,
        	number_rate=15,
        	average=True,
        	avg_alpha=0.03,
        	label="STE",
        	peak_hold=False,
        	show_gauge=True,
        )
        self.n_0.GetPage(0).GridAdd(self.wxgui_numbersink2_1_0_2.win, 0, 1, 1, 1)
        self.wxgui_numbersink2_1_0_1 = numbersink2.number_sink_f(
        	self.n_0.GetPage(0).GetWin(),
        	unit="normalized to tx power",
        	minval=0,
        	maxval=10,
        	factor=1.0,
        	decimal_places=3,
        	ref_level=0,
        	sample_rate=samp_rate,
        	number_rate=15,
        	average=True,
        	avg_alpha=0.03,
        	label="STE average ",
        	peak_hold=False,
        	show_gauge=True,
        )
        self.n_0.GetPage(0).GridAdd(self.wxgui_numbersink2_1_0_1.win, 1, 0, 1, 1)
        self.wxgui_numbersink2_1_0_0 = numbersink2.number_sink_f(
        	self.n_0.GetPage(0).GetWin(),
        	unit="normalized to tx power",
        	minval=0,
        	maxval=10,
        	factor=1.0,
        	decimal_places=3,
        	ref_level=0,
        	sample_rate=samp_rate,
        	number_rate=15,
        	average=True,
        	avg_alpha=0.03,
        	label="STE desviation",
        	peak_hold=False,
        	show_gauge=True,
        )
        self.n_0.GetPage(0).GridAdd(self.wxgui_numbersink2_1_0_0.win, 1, 1, 1, 1)
        self.wxgui_numbersink2_1_0 = numbersink2.number_sink_f(
        	self.n_0.GetPage(0).GetWin(),
        	unit="db",
        	minval=0,
        	maxval=100,
        	factor=1.0,
        	decimal_places=1,
        	ref_level=0,
        	sample_rate=samp_rate,
        	number_rate=15,
        	average=True,
        	avg_alpha=0.03,
        	label="MER",
        	peak_hold=False,
        	show_gauge=True,
        )
        self.n_0.GetPage(0).GridAdd(self.wxgui_numbersink2_1_0.win, 0, 0, 1, 1)
        self.mer_probe_ste_cf_0_0 = mer.probe_ste_cf((0.9487+0.9487j,0.9487+0.3162j, 0.3162+0.9487j, 0.3162 +0.3162j,0.9487-0.9487j,0.9487- 0.3162j, 0.3162-0.9487j, 0.3162- 0.3162j,-0.9487+0.9487j,-0.9487+ 0.3162j,- 0.3162+0.9487j,- 0.3162+ 0.3162j,-0.9487-0.9487j,-0.9487- 0.3162j,-0.3162-0.9487j,-0.3162- 0.3162j), 0.05)
        self.mer_probe_mer_c_0_0 = mer.probe_mer_c((symbol_table_64QAM),0.05)
        self.low_pass_filter_0 = filter.fir_filter_ccf(1, firdes.low_pass(
        	1, samp_rate, 5.6e6/2.0, 0.5e6, firdes.WIN_HAMMING, 6.76))
        self.isdbt_time_deinterleaver_0 = isdbt.time_deinterleaver(3, 1, 4, 12, 4, 0, 0)
        self.isdbt_symbol_demapper_0 = isdbt.symbol_demapper(3, 1, 4, 12, 64, 0, 64)
        self.isdbt_subset_of_carriers_0 = isdbt.subset_of_carriers(96*4*13, 96*4, 3*96*4-1)
        self.isdbt_isdbt_rf_channel_decoding_0 = isdbt.isdbt_rf_channel_decoding(
            max_freq_offset=200,
            guard=0.125,
            mode=3,
            snr=10,
            tmcc_print=False,
        )
        self.isdbt_frequency_deinterleaver_0 = isdbt.frequency_deinterleaver(True, 3)
        self.isdbt_channel_decoding_0 = isdbt.isdbt_channel_decoding(
            layer_segments=12,
            mode=3,
            constellation_size=64,
            rate=1,
        )
        self._center_freq_text_box = forms.text_box(
        	parent=self.GetWin(),
        	value=self.center_freq,
        	callback=self.set_center_freq,
        	label="frecuencia central",
        	converter=forms.float_converter(),
        )
        self.Add(self._center_freq_text_box)
        self.blocks_vector_to_stream_0_2 = blocks.vector_to_stream(gr.sizeof_gr_complex*1, 96*4*2)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.blocks_null_sink_1_1 = blocks.null_sink(gr.sizeof_char*384)
        self.blocks_file_source_1_0 = blocks.file_source(gr.sizeof_gr_complex*1, "/home/pablofloresguridi/Artes/grabacion_casa_montecarlo2", True)
        self.blocks_file_sink_0_1 = blocks.file_sink(gr.sizeof_char*1, "/home/pablofloresguridi/Artes/test_out_HD.ts", False)
        self.blocks_file_sink_0_1.set_unbuffered(False)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_file_source_1_0, 0), (self.blocks_throttle_0, 0))    
        self.connect((self.blocks_throttle_0, 0), (self.low_pass_filter_0, 0))    
        self.connect((self.blocks_vector_to_stream_0_2, 0), (self.mer_probe_mer_c_0_0, 0))    
        self.connect((self.blocks_vector_to_stream_0_2, 0), (self.mer_probe_ste_cf_0_0, 0))    
        self.connect((self.blocks_vector_to_stream_0_2, 0), (self.wxgui_scopesink2_0_1_0, 0))    
        self.connect((self.isdbt_channel_decoding_0, 0), (self.blocks_file_sink_0_1, 0))    
        self.connect((self.isdbt_frequency_deinterleaver_0, 0), (self.isdbt_time_deinterleaver_0, 0))    
        self.connect((self.isdbt_isdbt_rf_channel_decoding_0, 0), (self.isdbt_frequency_deinterleaver_0, 0))    
        self.connect((self.isdbt_isdbt_rf_channel_decoding_0, 0), (self.isdbt_subset_of_carriers_0, 0))    
        self.connect((self.isdbt_subset_of_carriers_0, 0), (self.blocks_vector_to_stream_0_2, 0))    
        self.connect((self.isdbt_symbol_demapper_0, 0), (self.blocks_null_sink_1_1, 0))    
        self.connect((self.isdbt_symbol_demapper_0, 1), (self.isdbt_channel_decoding_0, 0))    
        self.connect((self.isdbt_time_deinterleaver_0, 0), (self.isdbt_symbol_demapper_0, 0))    
        self.connect((self.low_pass_filter_0, 0), (self.isdbt_isdbt_rf_channel_decoding_0, 0))    
        self.connect((self.mer_probe_mer_c_0_0, 0), (self.wxgui_numbersink2_1_0, 0))    
        self.connect((self.mer_probe_ste_cf_0_0, 2), (self.wxgui_numbersink2_1_0_0, 0))    
        self.connect((self.mer_probe_ste_cf_0_0, 1), (self.wxgui_numbersink2_1_0_1, 0))    
        self.connect((self.mer_probe_ste_cf_0_0, 0), (self.wxgui_numbersink2_1_0_2, 0))    


    def get_mode(self):
        return self.mode

    def set_mode(self, mode):
        self.mode = mode
        self.set_data_carriers(13*96*2**(self.mode-1))
        self.set_active_carriers(13*108*2**(self.mode-1)+1)
        self.set_total_carriers(2**(10+self.mode))

    def get_total_carriers(self):
        return self.total_carriers

    def set_total_carriers(self, total_carriers):
        self.total_carriers = total_carriers

    def get_symbol_table_QPSK(self):
        return self.symbol_table_QPSK

    def set_symbol_table_QPSK(self, symbol_table_QPSK):
        self.symbol_table_QPSK = symbol_table_QPSK

    def get_symbol_table_64QAM(self):
        return self.symbol_table_64QAM

    def set_symbol_table_64QAM(self, symbol_table_64QAM):
        self.symbol_table_64QAM = symbol_table_64QAM

    def get_symbol_table_16QAM(self):
        return self.symbol_table_16QAM

    def set_symbol_table_16QAM(self, symbol_table_16QAM):
        self.symbol_table_16QAM = symbol_table_16QAM

    def get_samp_usrp(self):
        return self.samp_usrp

    def set_samp_usrp(self, samp_usrp):
        self.samp_usrp = samp_usrp

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, 5.6e6/2.0, 0.5e6, firdes.WIN_HAMMING, 6.76))
        self.wxgui_scopesink2_0_1_0.set_sample_rate(float(self.samp_rate*(1-self.guard))/13)

    def get_inter(self):
        return self.inter

    def set_inter(self, inter):
        self.inter = inter

    def get_guard(self):
        return self.guard

    def set_guard(self, guard):
        self.guard = guard
        self.wxgui_scopesink2_0_1_0.set_sample_rate(float(self.samp_rate*(1-self.guard))/13)

    def get_decim(self):
        return self.decim

    def set_decim(self, decim):
        self.decim = decim

    def get_data_carriers(self):
        return self.data_carriers

    def set_data_carriers(self, data_carriers):
        self.data_carriers = data_carriers

    def get_center_freq(self):
        return self.center_freq

    def set_center_freq(self, center_freq):
        self.center_freq = center_freq
        self._center_freq_text_box.set_value(self.center_freq)

    def get_active_carriers(self):
        return self.active_carriers

    def set_active_carriers(self, active_carriers):
        self.active_carriers = active_carriers


if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    if gr.enable_realtime_scheduling() != gr.RT_OK:
        print "Error: failed to enable realtime scheduling."
    tb = fullseg_receiver_and_measurements()
    tb.Start(True)
    tb.Wait()
