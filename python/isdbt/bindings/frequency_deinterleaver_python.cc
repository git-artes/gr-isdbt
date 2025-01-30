/*
 * Copyright 2025 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually
 * edited  */
/* The following lines can be configured to regenerate this file during cmake */
/* If manual edits are made, the following tags should be modified accordingly.
 */
/* BINDTOOL_GEN_AUTOMATIC(0) */
/* BINDTOOL_USE_PYGCCXML(0) */
/* BINDTOOL_HEADER_FILE(frequency_deinterleaver.h) */
/* BINDTOOL_HEADER_FILE_HASH(875b8b92244b1f16f80ed935df0d5d36) */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/isdbt/frequency_deinterleaver.h>
// pydoc.h is automatically generated in the build directory
#include <frequency_deinterleaver_pydoc.h>

void bind_frequency_deinterleaver(py::module &m) {

  using frequency_deinterleaver = ::gr::isdbt::frequency_deinterleaver;

  py::class_<frequency_deinterleaver, gr::sync_block, gr::block,
             gr::basic_block, std::shared_ptr<frequency_deinterleaver>>(
      m, "frequency_deinterleaver", D(frequency_deinterleaver))

      .def(py::init(&frequency_deinterleaver::make), py::arg("oneseg"),
           py::arg("mode"), D(frequency_deinterleaver, make))

      ;
}
