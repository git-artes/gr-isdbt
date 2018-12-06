# Copyright 2011 Free Software Foundation, Inc.
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
# Include python install macros
########################################################################
include(GrPython)
if(NOT PYTHONINTERP_FOUND)
    return()
endif()

########################################################################
# Install python sources
########################################################################
GR_PYTHON_INSTALL(
    FILES
    __init__.py
    isdbt_rf_channel_decoding.py
    isdbt_channel_decoding.py DESTINATION ${GR_PYTHON_DIR}/isdbt
)

########################################################################
# Handle the unit tests
########################################################################
include(GrTest)

set(GR_TEST_TARGET_DEPS gnuradio-isdbt)
set(GR_TEST_PYTHON_DIRS ${CMAKE_BINARY_DIR}/swig)
GR_ADD_TEST(qa_ofdm_sym_acquisition ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_ofdm_sym_acquisition.py)
GR_ADD_TEST(qa_sync_and_channel_estimation ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_sync_and_channel_estimation.py)
GR_ADD_TEST(qa_frequency_deinterleaver ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_frequency_deinterleaver.py)
GR_ADD_TEST(qa_time_deinterleaver ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_time_deinterleaver.py)
GR_ADD_TEST(qa_symbol_demapper ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_symbol_demapper.py)
GR_ADD_TEST(qa_tmcc_decoder ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_tmcc_decoder.py)
GR_ADD_TEST(qa_bit_deinterleaver ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_bit_deinterleaver.py)
#GR_ADD_TEST(qa_energy_descrambler ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_energy_descrambler.py)
GR_ADD_TEST(qa_byte_deinterleaver ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_byte_deinterleaver.py)
#GR_ADD_TEST(qa_ofdm_sym_acquisition_dvbt ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_ofdm_sym_acquisition_dvbt.py)
GR_ADD_TEST(qa_reed_solomon_dec_isdbt ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_reed_solomon_dec_isdbt.py)
GR_ADD_TEST(qa_frequency_deinterleaver_1seg ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_frequency_deinterleaver_1seg.py)
GR_ADD_TEST(qa_time_deinterleaver_1seg ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_time_deinterleaver_1seg.py)
GR_ADD_TEST(qa_symbol_demapper_1seg ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_symbol_demapper_1seg.py)
GR_ADD_TEST(qa_viterbi_decoder ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_viterbi_decoder.py)
GR_ADD_TEST(qa_isdbt_rf_channel_decoding ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_isdbt_rf_channel_decoding.py)
GR_ADD_TEST(qa_isdbt_channel_decoding ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_isdbt_channel_decoding.py)
GR_ADD_TEST(qa_ofdm_synchronization ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_ofdm_synchronization.py)
GR_ADD_TEST(qa_ofdm_synchronization_1seg ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_ofdm_synchronization_1seg.py)
GR_ADD_TEST(qa_tmcc_decoder_1seg ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_tmcc_decoder_1seg.py)
GR_ADD_TEST(qa_pilot_signals ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_pilot_signals.py)
GR_ADD_TEST(qa_frequency_interleaver ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_frequency_interleaver.py)
GR_ADD_TEST(qa_time_interleaver ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_time_interleaver.py)
GR_ADD_TEST(qa_hierarchical_combinator ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_hierarchical_combinator.py)
GR_ADD_TEST(qa_carrier_modulation ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_carrier_modulation.py)
GR_ADD_TEST(qa_byte_interleaver ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_byte_interleaver.py)
GR_ADD_TEST(qa_energy_dispersal ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_energy_dispersal.py)
GR_ADD_TEST(qa_tmcc_encoder ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_tmcc_encoder.py)
