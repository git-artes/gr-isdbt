# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Install public header files
########################################################################
install(FILES
    api.h
    ofdm_sym_acquisition.h
    sync_and_channel_estimaton.h
    subset_of_carriers.h
    frequency_deinterleaver.h
    time_deinterleaver.h
    symbol_demapper.h
    tmcc_decoder.h
    bit_deinterleaver.h
    viterbi_decoder_1seg.h
    energy_descrambler.h
    byte_deinterleaver.h
    viterbi_decoder_1seg_itpp.h
    bit_deinterleaver_streamed.h
    ofdm_sym_acquisition_dvbt.h
    ofdm_sym_acquisition_2.h
    reed_solomon.h
    reed_solomon_dec_isdbt.h DESTINATION include/isdbt
)
