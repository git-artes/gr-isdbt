/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017, 2018
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


#ifndef INCLUDED_ISDBT_BYTE_INTERLEAVER_H
#define INCLUDED_ISDBT_BYTE_INTERLEAVER_H

#include <isdbt/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief A byte interleaver as per the standard, plus the required delay-adjustment.
     * \ingroup isdbt
     *
     */
    class ISDBT_API byte_interleaver : virtual public gr::sync_interpolator
    {
     public:
      typedef boost::shared_ptr<byte_interleaver> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of isdbt::byte_interleaver.
       *
       * To avoid accidental use of raw pointers, isdbt::byte_interleaver's
       * constructor is in a private implementation
       * class. isdbt::byte_interleaver::make is the public interface for
       * creating new instances.
       */
      static sptr make(int mode, int constellation_size, int rate, int nsegments);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_BYTE_INTERLEAVER_H */

