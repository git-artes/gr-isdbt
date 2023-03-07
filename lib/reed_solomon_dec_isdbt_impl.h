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

#ifndef INCLUDED_ISDBT_REED_SOLOMON_DEC_ISDBT_IMPL_H
#define INCLUDED_ISDBT_REED_SOLOMON_DEC_ISDBT_IMPL_H

#include <gnuradio/isdbt/reed_solomon_dec_isdbt.h>


extern "C" {
#include <gnuradio/fec/rs.h>
}

namespace gr {
  namespace isdbt {

    /*!
     * \brief Reed Solomon decoder of the ISDB-Tb standard: RS(N=204,K=239,T=8). It is an instance of a more general 
     * decoder included with GNU Radio. 
     * \ingroup isdbt
     * \param p characteristic of GF(p^m) \n
     * \param m we use GF(p^m) \n
     * \param gfpoly Generator Polynomial \n
     * \param n length of codeword of RS coder \n
     * \param k length of information sequence of RS encoder \n
     * \param t number of corrected errors \n
     * \param s shortened length \n
     * \param blocks number of blocks to process at once\n
     */
    class reed_solomon_dec_isdbt_impl : public reed_solomon_dec_isdbt
    {
     private:

      const static int d_p;
      const static int d_m;
      const static int d_gfpoly;
      const static int d_n;
      const static int d_k;
      const static int d_t;
      const static int d_s;
      const static int d_blocks;
      const static int d_lambda;

      void *d_rs;    /* Reed-Solomon characteristics structure */
      int decode(unsigned char &out, const unsigned char &in);

      // Used to average the BER
      float d_alpha_avg;
      // the last ber_out, used to average
      float d_last_ber_out; 
      // the new ber_out, used to average
      float d_new_ber; 


     public:
      reed_solomon_dec_isdbt_impl();
      ~reed_solomon_dec_isdbt_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		  gr_vector_int &ninput_items,
	      gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_REED_SOLOMON_DEC_ISDBT_IMPL_H */


