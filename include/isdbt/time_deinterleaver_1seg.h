/* -*- c++ -*- */
/*  
 * Copyright 2015
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


#ifndef INCLUDED_ISDBT_TIME_DEINTERLEAVER_1SEG_H
#define INCLUDED_ISDBT_TIME_DEINTERLEAVER_1SEG_H

#include <isdbt/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief This a Forney interleaver as specified by the standard. However, this block 
     * is intended to be used in a 1-seg receiver (mainly to avoid unnecessary processing). 
     * \ingroup isdbt
     *
     * In other words, this block is simply one of the 13 sub-blocks that conform the 
     * time_deinterleaver block, which should be used for a full-seg receiver. 
     *
     */
    class ISDBT_API time_deinterleaver_1seg : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<time_deinterleaver_1seg> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of isdbt::time_deinterleaver_1seg.
       *
       * \param mode The transmission mode (either 1, 2 or 3). 
       * \param length The interleaver's length. 
       */
      static sptr make(int mode, int length);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_TIME_DEINTERLEAVER_1SEG_H */

