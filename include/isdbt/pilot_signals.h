/* -*- c++ -*- */
/*  
 * Copyright 2015, 2016, 2017, 2018
 *   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
 *   Santiago Castro
 *   Javier Hernandez
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


#ifndef INCLUDED_ISDBT_PILOT_SIGNALS_H
#define INCLUDED_ISDBT_PILOT_SIGNALS_H

#include <isdbt/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief This block's input is a vector representing only data carriers already allocated (i.e. excluding 
     *  TMCC and AC carriers) and the output is an OFDM frame with the pilots correctly set (i.e. SP and CP), the null 
     *  carriers correctly set in zero, plus the value of wi in the positions corresponding to the TMCC (which is expected 
     *  to be set downstream by a multiplication). 
     *  It thus expects a vector of length 4992 (in mode 3) and outputs a vector of length 8192 (in mode 3). 
     *
     * \ingroup isdbt
     *
     */
    class ISDBT_API pilot_signals : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<pilot_signals> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of isdbt::pilot_signals.
       *
       * To avoid accidental use of raw pointers, isdbt::pilot_signals's
       * constructor is in a private implementation
       * class. isdbt::pilot_signals::make is the public interface for
       * creating new instances.
       */
      static sptr make(int mode);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_PILOT_SIGNALS_H */

