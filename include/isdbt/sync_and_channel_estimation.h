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



#ifndef INCLUDED_ISDBT_SYNC_AND_CHANNEL_ESTIMATION_H
#define INCLUDED_ISDBT_SYNC_AND_CHANNEL_ESTIMATION_H

#include <isdbt/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief This block performs the integer frequency estimation (and correction),
     * estimates the channel taps and equalizes the output with this estimation. 
     *
     * \ingroup isdbt
     *
     * Optionally, it outputs the estimation of the channel taps. 
     *
     */
    class ISDBT_API sync_and_channel_estimation : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<sync_and_channel_estimation> sptr;

      /*!
       * \brief This block performs the integer frequency estimation (and correction),
       * estimates the channel taps and equalizes the output with this estimation. 
       *
       * \param fft_length The samples per OFDM symbol (or the FFT size). Typically a power of 2. In 
       * the case of isdb-t it may be derived from the transmission mode. 
       * \param payload_length The number of carrier that actually have data. In the case of ISDB-T
       * it may be derived from the transmission mode. 
       * \param offset_max The algorithm implemented will assume that the frequency offset is no 
       * bigger than offset_max. 
       *
       */
      static sptr make(int fft_length, int payload_length, int offset_max);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_SYNC_AND_CHANNEL_estimation_H */

