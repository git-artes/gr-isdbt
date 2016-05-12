/* -*- c++ -*- */

#define ISDBT_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "isdbt_swig_doc.i"

%{
#include "isdbt/ofdm_sym_acquisition.h"
#include "isdbt/sync_and_channel_estimation.h"
#include "isdbt/symbol_demapper.h"
#include "isdbt/tmcc_decoder.h"
#include "isdbt/subset_of_carriers.h"
#include "isdbt/frequency_deinterleaver.h"
#include "isdbt/time_deinterleaver.h"
#include "isdbt/bit_deinterleaver.h"
#include "isdbt/energy_descrambler.h"
#include "isdbt/byte_deinterleaver.h"
//#include "isdbt/bit_deinterleaver_streamed.h"
#include "isdbt/ofdm_sym_acquisition_dvbt.h"
#include "isdbt/reed_solomon_dec_isdbt.h"
#include "isdbt/frequency_deinterleaver_1seg.h"
#include "isdbt/time_deinterleaver_1seg.h"
#include "isdbt/symbol_demapper_1seg.h"
#include "isdbt/viterbi_decoder.h"
#include "isdbt/ofdm_synchronization.h"
#include "isdbt/ofdm_synchronization_1seg.h"
#include "isdbt/tmcc_decoder_1seg.h"
%}


%include "isdbt/ofdm_sym_acquisition.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, ofdm_sym_acquisition);
%include "isdbt/sync_and_channel_estimation.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, sync_and_channel_estimation);

%include "isdbt/subset_of_carriers.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, subset_of_carriers);
%include "isdbt/frequency_deinterleaver.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, frequency_deinterleaver);
%include "isdbt/time_deinterleaver.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, time_deinterleaver);
%include "isdbt/symbol_demapper.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, symbol_demapper);
%include "isdbt/tmcc_decoder.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, tmcc_decoder);
%include "isdbt/bit_deinterleaver.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, bit_deinterleaver);

%include "isdbt/energy_descrambler.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, energy_descrambler);
%include "isdbt/byte_deinterleaver.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, byte_deinterleaver);

//%include "isdbt/bit_deinterleaver_streamed.h"
//GR_SWIG_BLOCK_MAGIC2(isdbt, bit_deinterleaver_streamed);

%include "isdbt/ofdm_sym_acquisition_dvbt.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, ofdm_sym_acquisition_dvbt);
%include "isdbt/reed_solomon_dec_isdbt.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, reed_solomon_dec_isdbt);
%include "isdbt/frequency_deinterleaver_1seg.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, frequency_deinterleaver_1seg);
%include "isdbt/time_deinterleaver_1seg.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, time_deinterleaver_1seg);
%include "isdbt/symbol_demapper_1seg.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, symbol_demapper_1seg);


%include "isdbt/viterbi_decoder.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, viterbi_decoder);
%include "isdbt/ofdm_synchronization.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, ofdm_synchronization);
%include "isdbt/ofdm_synchronization_1seg.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, ofdm_synchronization_1seg);
%include "isdbt/tmcc_decoder_1seg.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, tmcc_decoder_1seg);
