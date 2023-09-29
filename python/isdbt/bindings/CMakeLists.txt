# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT isdbt_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT isdbt_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND isdbt_python_files
    bit_deinterleaver_python.cc
    byte_deinterleaver_python.cc
    byte_interleaver_python.cc
    carrier_modulation_python.cc
    energy_descrambler_python.cc
    energy_dispersal_python.cc
    frequency_deinterleaver_python.cc
    frequency_interleaver_python.cc
    hierarchical_combinator_python.cc
    ofdm_synchronization_python.cc
    pilot_signals_python.cc
    reed_solomon_dec_isdbt_python.cc
    subset_of_carriers_python.cc
    symbol_demapper_python.cc
    time_deinterleaver_python.cc
    time_interleaver_python.cc
    tmcc_decoder_python.cc
    tmcc_encoder_python.cc
    viterbi_decoder_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_OOT(isdbt
   ../../..
   gr::isdbt
   "${isdbt_python_files}")


# copy in bindings .so / .lib file for use in QA test module
IF (WIN32)
  # windows uses .lib
  add_custom_target(
    copy_bindings_for_tests ALL
    COMMAND
      ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/isdbt_python.lib"
      ${CMAKE_BINARY_DIR}/test_modules/gnuradio/isdbt/
    DEPENDS isdbt_python)

ELSE()
  # linux uses .so
  add_custom_target(
    copy_bindings_for_tests ALL
    COMMAND
      ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/isdbt_python.lib"
      ${CMAKE_BINARY_DIR}/test_modules/gnuradio/isdbt/
    DEPENDS isdbt_python)
ENDIF()

install(TARGETS isdbt_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/isdbt COMPONENT pythonapi)
