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

#ifndef INCLUDED_ISDBT_TMCC_DECODER_H
#define INCLUDED_ISDBT_TMCC_DECODER_H

#include <gnuradio/isdbt/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief Given the useful carriers, this block decodes the TMCC carriers, and outputs those carriers 
     * corresponding to actual DTV data (i.e. not including pilots nor AC). Plus, it re-orders the segments 
     * so as to provide them in the ascending (and correct in terms of nomenclature) order. 
     *
     * \ingroup isdbt
     *
     */
    class ISDBT_API tmcc_decoder : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<tmcc_decoder> sptr;

      /*!
       * \brief Given the useful carriers, this block decodes the TMCC carriers, and outputs those carriers corresponding to actual DTV data (i.e. not including pilots nor AC). Plus, it re-orders the segments 
     * so as to provide them in the ascending (and correct in terms of nomenclature) order.
     
       * \param mode The transmission mode (either 1, 2 or 3). 
       * \param print_params Whether or not to print the decoded TMCC parameters. 
       * This is useful when first watching a channel, since several parameters in the 
       * receiver have to be found first. 
       *
       */
      static sptr make(int mode, bool print_params);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_TMCC_DECODER_H */

