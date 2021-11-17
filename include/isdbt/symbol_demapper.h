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

#ifndef INCLUDED_ISDBT_SYMBOL_DEMAPPER_H
#define INCLUDED_ISDBT_SYMBOL_DEMAPPER_H

#include <isdbt/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief It demaps the symbol in each carrier and then outputs each code-word
     * serially. 
     *
     * \ingroup isdbt
     *
     * The implementation is optimized for the constellation used in ISDB-T. 
     *
     */
    class ISDBT_API symbol_demapper : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<symbol_demapper> sptr;

      /*!
       * \brief It demaps the symbol in each carrier and then outputs each 
       * code-word serially. 
       *
       * \param mode The transmission mode (either 1, 2 or 3). 
       * \param segments_A The number of segments in layer A. 
       * \param constellation_size_A The constellation used in layer A (4 for QPSK, 16 for 16QAM 
       * and 64 for 64QAM). 
       * \param segments_B The number of segments in layer B. 
       * \param constellation_size_B The constellation used in layer B (4 for QPSK, 16 for 16QAM 
       * and 64 for 64QAM). 
       * \param segments_C The number of segments in layer C. 
       * \param constellation_size_C The constellation used in layer C (4 for QPSK, 16 for 16QAM 
       * and 64 for 64QAM). 
       *
       */
      static sptr make(int mode, int segments_A, int constellation_size_A, int segments_B, int constellation_size_B, int segments_C, int constellation_size_C);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_SYMBOL_DEMAPPER_H */

