/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017, 2018
 *   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
 *   Santiago Castro
 *   Javier Hernández
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


#ifndef INCLUDED_ISDBT_HIERARCHICAL_COMBINATOR_H
#define INCLUDED_ISDBT_HIERARCHICAL_COMBINATOR_H

#include <isdbt/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief The block takes the complex streams of each layer and combines them into a vector of length 2**(10+mode).
     * \ingroup isdbt
     *
     */
    class ISDBT_API hierarchical_combinator : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<hierarchical_combinator> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of isdbt::hierarchical_combinator.
       *
       * To avoid accidental use of raw pointers, isdbt::hierarchical_combinator's
       * constructor is in a private implementation
       * class. isdbt::hierarchical_combinator::make is the public interface for
       * creating new instances.
       */
      static sptr make(int mode, int segments_A, int segments_B, int segments_C);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_HIERARCHICAL_COMBINATOR_H */

