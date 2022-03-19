/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py = pybind11;

// Headers for binding functions
/**************************************/
// The following comment block is used for
// gr_modtool to insert function prototypes
// Please do not delete
/**************************************/
// BINDING_FUNCTION_PROTOTYPES(
void bind_bit_deinterleaver_python(py::module& m);
void bind_byte_deinterleaver_python(py::module& m);
void bind_byte_interleaver_python(py::module& m);
void bind_carrier_modulation_python(py::module& m);
void bind_energy_descrambler_python(py::module& m);
void bind_energy_dispersal_python(py::module& m);
void bind_frequency_deinterleaver_python(py::module& m);
void bind_frequency_interleaver_python(py::module& m);
void bind_hierarchical_combinator_python(py::module& m);
void bind_ofdm_synchronization_python(py::module& m);
void bind_pilot_signals_python(py::module& m);
void bind_reed_solomon_dec_isdbt_python(py::module& m);
void bind_subset_of_carriers_python(py::module& m);
void bind_symbol_demapper_python(py::module& m);
void bind_time_deinterleaver_python(py::module& m);
void bind_time_interleaver_python(py::module& m);
void bind_tmcc_decoder_python(py::module& m);
void bind_tmcc_encoder_python(py::module& m);
void bind_viterbi_decoder_python(py::module& m);
// ) END BINDING_FUNCTION_PROTOTYPES


// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(isdbt_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    /**************************************/
    // The following comment block is used for
    // gr_modtool to insert binding function calls
    // Please do not delete
    /**************************************/
    // BINDING_FUNCTION_CALLS(
    bind_bit_deinterleaver_python(m);
    bind_byte_deinterleaver_python(m);
    bind_byte_interleaver_python(m);
    bind_carrier_modulation_python(m);
    bind_energy_descrambler_python(m);
    bind_energy_dispersal_python(m);
    bind_frequency_deinterleaver_python(m);
    bind_frequency_interleaver_python(m);
    bind_hierarchical_combinator_python(m);
    bind_ofdm_synchronization_python(m);
    bind_pilot_signals_python(m);
    bind_reed_solomon_dec_isdbt_python(m);
    bind_subset_of_carriers_python(m);
    bind_symbol_demapper_python(m);
    bind_time_deinterleaver_python(m);
    bind_time_interleaver_python(m);
    bind_tmcc_decoder_python(m);
    bind_tmcc_encoder_python(m);
    bind_viterbi_decoder_python(m);
    // ) END BINDING_FUNCTION_CALLS
}
