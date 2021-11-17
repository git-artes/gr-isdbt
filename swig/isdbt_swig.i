/* -*- c++ -*- */

#define ISDBT_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "isdbt_swig_doc.i"

%{
#include "isdbt/ofdm_synchronization.h"
#include "isdbt/tmcc_decoder.h"
#include "isdbt/frequency_deinterleaver.h"
#include "isdbt/time_deinterleaver.h"
#include "isdbt/symbol_demapper.h"
#include "isdbt/bit_deinterleaver.h"
%}

%include "isdbt/ofdm_synchronization.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, ofdm_synchronization);
%include "isdbt/tmcc_decoder.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, tmcc_decoder);
%include "isdbt/frequency_deinterleaver.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, frequency_deinterleaver);
%include "isdbt/time_deinterleaver.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, time_deinterleaver);
%include "isdbt/symbol_demapper.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, symbol_demapper);
%include "isdbt/bit_deinterleaver.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, bit_deinterleaver);
