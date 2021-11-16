/* -*- c++ -*- */

#define ISDBT_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "isdbt_swig_doc.i"

%{
#include "isdbt/ofdm_synchronization.h"
#include "isdbt/tmcc_decoder.h"
#include "isdbt/frequency_deinterleaver.h"
%}

%include "isdbt/ofdm_synchronization.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, ofdm_synchronization);
%include "isdbt/tmcc_decoder.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, tmcc_decoder);
%include "isdbt/frequency_deinterleaver.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, frequency_deinterleaver);
