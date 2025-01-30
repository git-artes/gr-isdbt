/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017, 2018, 2019, 2020, 2021
 *   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
 *   Pablo Belzarena 
 *   Gabriel Gomez Sena 
 *   Pablo Flores Guridi 
 *   Victor Gonzalez Barbone
 *
 *   Instituto de Ingenieria Electrica, Facultad de Ingenieria,
 *   Universidad de la Republica, Uruguay.
 *  
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *  
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef INCLUDED_ISDBT_TIME_DEINTERLEAVER_H
#define INCLUDED_ISDBT_TIME_DEINTERLEAVER_H

#include <gnuradio/isdbt/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief This is simply a Forney interleaver as specified by the standard. 
     *
     * There are a total of 13 identical interleaving sub-blocks, one for each segment. 
     *
     * \ingroup isdbt
     *
     */
    class ISDBT_API time_deinterleaver : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<time_deinterleaver> sptr;

      /*!
       * \brief This is simply a Forney interleaver as specified by the standard. 
       *
       * Since each layer may have its own interleaving length, we require the number of 
       * segments of each layer, and its associated parmeters. 
       *
       * \param mode The transmission mode (either 1, 2 or 3). 
       * \param segments_A The number of segments in layer A. 
       * \param length_A The interleaver's length configured in layer A. 
       * \param segments_B The number of segments in layer B. 
       * \param length_B The interleaver's length configured in layer B. 
       * \param segments_C The number of segments in layer C. 
       * \param length_C The interleaver's length configured in layer C. 
       *
       */
      //static sptr make(int mode, int length);
      static sptr make(int mode, int segments_A, int length_A, int segments_B, int length_B, int segments_C, int length_C); 
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_TIME_DEINTERLEAVER_H */

