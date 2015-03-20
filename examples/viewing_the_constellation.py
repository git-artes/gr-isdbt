#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Viewing The Constellation
# Generated: Wed Mar 18 16:42:23 2015
##################################################

from gnuradio import blocks
from gnuradio import channels
from gnuradio import eng_notation
from gnuradio import fft
from gnuradio import filter
from gnuradio import gr
from gnuradio import wxgui
from gnuradio.eng_option import eng_option
from gnuradio.fft import window
from gnuradio.filter import firdes
from gnuradio.wxgui import forms
from gnuradio.wxgui import scopesink2
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import isdbt
import wx

class viewing_the_constellation(grc_wxgui.top_block_gui):

    def __init__(self):
        grc_wxgui.top_block_gui.__init__(self, title="Viewing The Constellation")
        _icon_path = "/usr/share/icons/hicolor/32x32/apps/gnuradio-grc.png"
        self.SetIcon(wx.Icon(_icon_path, wx.BITMAP_TYPE_ANY))

        ##################################################
        # Variables
        ##################################################
        self.mode = mode = 3
        self.total_carriers = total_carriers = 2**(10+mode)
        self.samp_rate = samp_rate = 6.4e6*80/63
        self.offset_freq = offset_freq = 0
        self.noise = noise = 0
        self.guard = guard = 1.0/16
        self.active_carriers = active_carriers = 13*108*2**(mode-1)+1

        ##################################################
        # Blocks
        ##################################################
        _offset_freq_sizer = wx.BoxSizer(wx.VERTICAL)
        self._offset_freq_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_offset_freq_sizer,
        	value=self.offset_freq,
        	callback=self.set_offset_freq,
        	label="offset_freq",
        	converter=forms.float_converter(),
        	proportion=0,
        )
        self._offset_freq_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_offset_freq_sizer,
        	value=self.offset_freq,
        	callback=self.set_offset_freq,
        	minimum=0,
        	maximum=0.1,
        	num_steps=100,
        	style=wx.SL_HORIZONTAL,
        	cast=float,
        	proportion=1,
        )
        self.Add(_offset_freq_sizer)
        _noise_sizer = wx.BoxSizer(wx.VERTICAL)
        self._noise_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_noise_sizer,
        	value=self.noise,
        	callback=self.set_noise,
        	label="Noise variance",
        	converter=forms.float_converter(),
        	proportion=0,
        )
        self._noise_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_noise_sizer,
        	value=self.noise,
        	callback=self.set_noise,
        	minimum=0,
        	maximum=0.1,
        	num_steps=100,
        	style=wx.SL_HORIZONTAL,
        	cast=float,
        	proportion=1,
        )
        self.Add(_noise_sizer)
        self._guard_chooser = forms.drop_down(
        	parent=self.GetWin(),
        	value=self.guard,
        	callback=self.set_guard,
        	label="Guard Interval",
        	choices=[1.0/4, 1.0/8, 1.0/16, 1.0/32],
        	labels=["1/4", "1/8", "1/16", "1/32"],
        )
        self.Add(self._guard_chooser)
        self.wxgui_scopesink2_0_1_0 = scopesink2.scope_sink_c(
        	self.GetWin(),
        	title="Constellation (all carriers)",
        	sample_rate=float(samp_rate*(1-guard)),
        	v_scale=0,
        	v_offset=0,
        	t_scale=0,
        	ac_couple=False,
        	xy_mode=True,
        	num_inputs=1,
        	trig_mode=wxgui.TRIG_MODE_AUTO,
        	y_axis_label="Counts",
        )
        self.Add(self.wxgui_scopesink2_0_1_0.win)
        self._mode_chooser = forms.drop_down(
        	parent=self.GetWin(),
        	value=self.mode,
        	callback=self.set_mode,
        	label="Mode",
        	choices=[1,2,3],
        	labels=["1 (2k)", "2 (4k)", "3 (8k)"],
        )
        self.Add(self._mode_chooser)
        self.low_pass_filter_0 = filter.fir_filter_ccf(1, firdes.low_pass(
        	1, samp_rate, 5.6e6/2.0, 0.5e6, firdes.WIN_HAMMING, 6.76))
        self.isdbt_sync_and_channel_estimaton_0 = isdbt.sync_and_channel_estimaton(8192, 5617, 200)
        self.isdbt_ofdm_sym_acquisition_0 = isdbt.ofdm_sym_acquisition(total_carriers, int(guard*total_carriers), 10)
        self.fft_vxx_0 = fft.fft_vcc(total_carriers, True, (window.rectangular(total_carriers)), True, 1)
        self.channels_channel_model_0_0 = channels.channel_model(
        	noise_voltage=noise,
        	frequency_offset=offset_freq,
        	epsilon=1,
        	taps=(1, ),
        	noise_seed=0,
        	block_tags=False
        )
        self.blocks_vector_to_stream_0_2 = blocks.vector_to_stream(gr.sizeof_gr_complex*1, active_carriers)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.blocks_file_source_1 = blocks.file_source(gr.sizeof_gr_complex*1, "/home/flarroca/facultad/proyectos/2014_fstv/grabaciones/grabacion_laboratorio2", True)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_file_source_1, 0), (self.blocks_throttle_0, 0))
        self.connect((self.low_pass_filter_0, 0), (self.channels_channel_model_0_0, 0))
        self.connect((self.channels_channel_model_0_0, 0), (self.isdbt_ofdm_sym_acquisition_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.low_pass_filter_0, 0))
        self.connect((self.fft_vxx_0, 0), (self.isdbt_sync_and_channel_estimaton_0, 0))
        self.connect((self.blocks_vector_to_stream_0_2, 0), (self.wxgui_scopesink2_0_1_0, 0))
        self.connect((self.isdbt_ofdm_sym_acquisition_0, 0), (self.fft_vxx_0, 0))
        self.connect((self.isdbt_sync_and_channel_estimaton_0, 0), (self.blocks_vector_to_stream_0_2, 0))



    def get_mode(self):
        return self.mode

    def set_mode(self, mode):
        self.mode = mode
        self.set_total_carriers(2**(10+self.mode))
        self.set_active_carriers(13*108*2**(self.mode-1)+1)
        self._mode_chooser.set_value(self.mode)

    def get_total_carriers(self):
        return self.total_carriers

    def set_total_carriers(self, total_carriers):
        self.total_carriers = total_carriers

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, 5.6e6/2.0, 0.5e6, firdes.WIN_HAMMING, 6.76))
        self.wxgui_scopesink2_0_1_0.set_sample_rate(float(self.samp_rate*(1-self.guard)))

    def get_offset_freq(self):
        return self.offset_freq

    def set_offset_freq(self, offset_freq):
        self.offset_freq = offset_freq
        self.channels_channel_model_0_0.set_frequency_offset(self.offset_freq)
        self._offset_freq_slider.set_value(self.offset_freq)
        self._offset_freq_text_box.set_value(self.offset_freq)

    def get_noise(self):
        return self.noise

    def set_noise(self, noise):
        self.noise = noise
        self.channels_channel_model_0_0.set_noise_voltage(self.noise)
        self._noise_slider.set_value(self.noise)
        self._noise_text_box.set_value(self.noise)

    def get_guard(self):
        return self.guard

    def set_guard(self, guard):
        self.guard = guard
        self._guard_chooser.set_value(self.guard)
        self.wxgui_scopesink2_0_1_0.set_sample_rate(float(self.samp_rate*(1-self.guard)))

    def get_active_carriers(self):
        return self.active_carriers

    def set_active_carriers(self, active_carriers):
        self.active_carriers = active_carriers

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    if gr.enable_realtime_scheduling() != gr.RT_OK:
        print "Error: failed to enable realtime scheduling."
    tb = viewing_the_constellation()
    tb.Start(True)
    tb.Wait()
