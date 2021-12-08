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

#ifndef INCLUDED_ISDBT_TMCC_ENCODER_H
#define INCLUDED_ISDBT_TMCC_ENCODER_H

#include <isdbt/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief This block calculates the TMCC word depending on the chosen parameters. Moreover, it multiplies the incoming OFDM symbol 
     *  with the corresponding value (i.e. it assumes that the w_i are already correctly set, and it simply multiplies by -1 or 1 
     *  depending on the bit, OFDM symbol, configuration (naturally), etc.)
     * \ingroup isdbt
     *
     */
    class ISDBT_API tmcc_encoder : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<tmcc_encoder> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of isdbt::tmcc_encoder.
       *
       * To avoid accidental use of raw pointers, isdbt::tmcc_encoder's
       * constructor is in a private implementation
       * class. isdbt::tmcc_encoder::make is the public interface for
       * creating new instances.
       */
      static sptr make(int mode, bool one_seg_present, int mod_scheme_A, int mod_scheme_B, int mod_scheme_C, int conv_code_A, int conv_code_B, int conv_code_C, int int_length_A, int int_length_B, int int_length_C, int nsegs_A, int nsegs_B, int nsegs_C);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_TMCC_ENCODER_H */

