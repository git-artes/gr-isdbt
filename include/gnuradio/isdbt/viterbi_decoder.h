/* -*- c++ -*- */
/*  
 * Copyright 2013,2014,2015 <Bogdan Diaconescu, yo3iiu@yo3iiu.ro>.
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

#ifndef INCLUDED_ISDBT_VITERBI_DECODER_H
#define INCLUDED_ISDBT_VITERBI_DECODER_H

#include <gnuradio/isdbt/api.h>
#include <gnuradio/block.h>

namespace gr {
    namespace isdbt {

        /*!
         * \brief A very fast implementation of a Viterbi decoder.     
         *
         * \ingroup isdbt
         *
         * It receives unpacked bytes (in the case of for instance QPSK
         * of every 8 bits in each byte, 2 would be useful) and outputs 
         * decoded packed bytes (all bits are useful). Optionally, the 
         * bit-error rate (BER) is output (the output is already averaged
         * with a single-pole filter).   
         *
         */
        class ISDBT_API viterbi_decoder : virtual public gr::block
        {
            public:
                typedef std::shared_ptr<viterbi_decoder> sptr;
                /*!
                 * \brief A very fast implementation of a Viterbi decoder.     
                 *
                 * \param constellation_size The modulation used (4 for QPSK, 16 for 16QAM and 64 for 64QAM). 
                 * \param rate The coding rate (0 for 1/2, 1 for 2/3, 2 for 3/4, 3 for 5/6 and 4 for 7/8). 
                 *
                 */

                static sptr make(int constellation_size, int rate);
        };
    } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_VITERBI_DECODER_H */

