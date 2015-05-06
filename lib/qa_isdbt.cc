/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*
 * This class gathers together all the test cases for the gr-filter
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include "qa_isdbt.h"
#include "qa_ofdm_sym_acquisition.h"
//#include "qa_frequency_deinterleaver.h"
//#include "qa_time_deinterleaver.h"
#include "qa_symbol_demapper.h"
#include "qa_bit_deinterleaver.h"

//#include "qa_viterbi_decoder_1seg.h"
#include "qa_energy_descrambler.h"
#include "qa_byte_deinterleaver.h"
//#include "qa_viterbi_decoder_1seg_itpp.h"
#include "qa_bit_deinterleaver_streamed.h"
#include "qa_reed_solomon_dec_isdbt.h"

CppUnit::TestSuite *
qa_isdbt::suite()
{
  CppUnit::TestSuite *s = new CppUnit::TestSuite("isdbt");
  s->addTest(gr::isdbt::qa_ofdm_sym_acquisition::suite());
//  s->addTest(gr::isdbt::qa_frequency_deinterleaver::suite());
//  s->addTest(gr::isdbt::qa_time_deinterleaver::suite());
  s->addTest(gr::isdbt::qa_symbol_demapper::suite());
  s->addTest(gr::isdbt::qa_bit_deinterleaver::suite());

//  s->addTest(gr::isdbt::qa_viterbi_decoder_1seg::suite());
  s->addTest(gr::isdbt::qa_energy_descrambler::suite());
  s->addTest(gr::isdbt::qa_byte_deinterleaver::suite());
//  s->addTest(gr::isdbt::qa_viterbi_decoder_1seg_itpp::suite());
  s->addTest(gr::isdbt::qa_bit_deinterleaver_streamed::suite());
  s->addTest(gr::isdbt::qa_reed_solomon_dec_isdbt::suite());

  return s;
}
