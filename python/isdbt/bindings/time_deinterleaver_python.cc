/*
 * Copyright 2022 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(time_deinterleaver.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(10c96da390dc40084dd497c276f955b5)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/isdbt/time_deinterleaver.h>
// pydoc.h is automatically generated in the build directory
#include <time_deinterleaver_pydoc.h>

void bind_time_deinterleaver(py::module& m)
{

    using time_deinterleaver    = ::gr::isdbt::time_deinterleaver;


    py::class_<time_deinterleaver, gr::sync_block, gr::block, gr::basic_block,
        std::shared_ptr<time_deinterleaver>>(m, "time_deinterleaver", D(time_deinterleaver))

        .def(py::init(&time_deinterleaver::make),
           py::arg("mode"),
           py::arg("segments_A"),
           py::arg("length_A"),
           py::arg("segments_B"),
           py::arg("length_B"),
           py::arg("segments_C"),
           py::arg("length_C"),
           D(time_deinterleaver,make)
        )
        



        ;




}







