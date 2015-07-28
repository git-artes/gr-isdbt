/* -*- c++ -*- */
/*  
 * Copyright 2013,2014,2015 <Bogdan Diaconescu, yo3iiu@yo3iiu.ro>.
 * Copyright 2015, minor modifications
 *   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
 *   Pablo Belzarena 
 *   Gabriel Gomez Sena 
 *   Pablo Flores Guridi 
 *  Victor Gonzalez Barbone
 *
 *  Instituto de Ingenieria Electrica, Facultad de Ingenieria,
 *  Universidad de la Republica, Uruguay.
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
 */


#ifndef INCLUDED_ISDBT_REED_SOLOMON_DEC_ISDBT_H
#define INCLUDED_ISDBT_REED_SOLOMON_DEC_ISDBT_H

#include <isdbt/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief It receives blocks of 204 bytes (corresponding to a TSP), 
     * and performs a Reed-Solomon decoding as specified by the standard. 
     *
     * \ingroup isdbt
     *
     * Optionally, bit-error rate (BER) is output (which is already averaged 
     * by a single-root filter).
     *
     */
    class ISDBT_API reed_solomon_dec_isdbt : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<reed_solomon_dec_isdbt> sptr;

      /*!
       * \brief It receives blocks of 204 bytes (corresponding to a TSP), 
       * and performs a Reed-Solomon decoding as specified by the standard. 
       */
      static sptr make();
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_REED_SOLOMON_DEC_ISDBT_H */

