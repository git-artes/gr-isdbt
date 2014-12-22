/* -*- c++ -*- */

#define ISDBT_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "isdbt_swig_doc.i"

%{
#include "isdbt/ofdm_sym_acquisition.h"
#include "isdbt/sync_and_channel_estimaton.h"
%}


%include "isdbt/ofdm_sym_acquisition.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, ofdm_sym_acquisition);
%include "isdbt/sync_and_channel_estimaton.h"
GR_SWIG_BLOCK_MAGIC2(isdbt, sync_and_channel_estimaton);
