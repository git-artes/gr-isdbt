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
void bind_bit_deinterleaver(py::module& m);
void bind_byte_deinterleaver(py::module& m);
void bind_byte_interleaver(py::module& m);
void bind_carrier_modulation(py::module& m);
void bind_energy_descrambler(py::module& m);
void bind_energy_dispersal(py::module& m);
void bind_frequency_deinterleaver(py::module& m);
void bind_frequency_interleaver(py::module& m);
void bind_hierarchical_combinator(py::module& m);
void bind_ofdm_synchronization(py::module& m);
void bind_pilot_signals(py::module& m);
void bind_reed_solomon_dec_isdbt(py::module& m);
void bind_subset_of_carriers(py::module& m);
void bind_symbol_demapper(py::module& m);
void bind_time_deinterleaver(py::module& m);
void bind_time_interleaver(py::module& m);
void bind_tmcc_decoder(py::module& m);
void bind_tmcc_encoder(py::module& m);
void bind_viterbi_decoder(py::module& m);
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
    bind_bit_deinterleaver(m);
    bind_byte_deinterleaver(m);
    bind_byte_interleaver(m);
    bind_carrier_modulation(m);
    bind_energy_descrambler(m);
    bind_energy_dispersal(m);
    bind_frequency_deinterleaver(m);
    bind_frequency_interleaver(m);
    bind_hierarchical_combinator(m);
    bind_ofdm_synchronization(m);
    bind_pilot_signals(m);
    bind_reed_solomon_dec_isdbt(m);
    bind_subset_of_carriers(m);
    bind_symbol_demapper(m);
    bind_time_deinterleaver(m);
    bind_time_interleaver(m);
    bind_tmcc_decoder(m);
    bind_tmcc_encoder(m);
    bind_viterbi_decoder(m);
    // ) END BINDING_FUNCTION_CALLS
}
