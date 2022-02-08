/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017, 2018, 2019, 2020, 2021
 *   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
 *   Pablo Belzarena 
 *   Gabriel Gomez Sena 
 *   Pablo Flores Guridi 
 *   Victor Gonzalez Barbone
 * Copyright 2021
 *   Lucas Inglés
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

#ifndef INCLUDED_ISDBT_SUBSET_OF_CARRIERS_H
#define INCLUDED_ISDBT_SUBSET_OF_CARRIERS_H

#include <isdbt/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief This block outputs the subset of "carriers" indicated by 
     * first and last. It is mostly used for debugging purposes, specially 
     * when we want to check a certain segment. 
     *
     * However, it may also be used to obtain certain segments in a ISDB-T
     * transmission. 
     *
     * \ingroup isdbt
     *
     */
    class ISDBT_API subset_of_carriers : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<subset_of_carriers> sptr;

      /*!
       * \brief This block outputs the subset of "carriers" indicated by 
       * first and last. It is mostly used for debugging purposes, specially 
       * when we want to check a certain segment. 
       *
       * \param total The total number of carriers in the input vector (0 is the first one). 
       * \param first The output will correspond to a vector beginning with this 
       * index
       * \param last The output will correspond to a vector ending with this index (last is 
       * thus included in the output). 
       *
       */
      static sptr make(int total, int first, int last);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_SUBSET_OF_CARRIERS_H */


