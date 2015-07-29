/* -*- c++ -*- */
/*  
 * Copyright 2013,2014,2015 <Bogdan Diaconescu, yo3iiu@yo3iiu.ro>.
 * Copyright 2015, several modifications.
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


#ifndef INCLUDED_ISDBT_OFDM_SYM_ACQUISITION_H
#define INCLUDED_ISDBT_OFDM_SYM_ACQUISITION_H

#include <isdbt/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief Performs one-shot OFDM symbol and fractional frequency synchronization. 
     * \ingroup isdbt 
     *
     * This block receives the stream of complex baseband samples corresponding to several 
     * OFDM symbols (cyclic prefix included), and outputs a vector corresponding to the 
     * useful part of the symbol. Furthermore, coarse (or fractional) frequency synchronization 
     * is applied. Optionally, the frequency error estimate is output. 
     *
     *
     */
    class ISDBT_API ofdm_sym_acquisition : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<ofdm_sym_acquisition> sptr;

      /*!
       \brief Performs one-shot OFDM symbol and fractional frequency synchronization. 

       \param fft_length The number of samples in a useful OFDM symbol (typically, a power of 2). 
       \param cp_length The number of samples in the cyclic prefix. Typically, a fraction of fft_length (however, 
       the actual number of samples is the parameter here). 
       \param snr An estimated SNR required by the synchronization algorithm. Improvements to the implementation should automatically calculate this. 
       \return a sptr pointer.
       *
       */
      static sptr make(int fft_length, int cp_length, float snr);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_OFDM_SYM_ACQUISITION_H */

