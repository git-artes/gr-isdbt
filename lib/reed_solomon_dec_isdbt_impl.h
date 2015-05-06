/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_ISDBT_REED_SOLOMON_DEC_ISDBT_IMPL_H
#define INCLUDED_ISDBT_REED_SOLOMON_DEC_ISDBT_IMPL_H

#include <isdbt/reed_solomon_dec_isdbt.h>
#include <isdbt/reed_solomon.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief Reed Solomon decoder of the ISDB-Tb standard: RS(N=204,K=239,T=8). It is an instance of a more general 
     * decoder by Bogdan Diaconescu. 
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

      unsigned char * d_in;

      reed_solomon d_rs;

     public:
      reed_solomon_dec_isdbt_impl();
      ~reed_solomon_dec_isdbt_impl();

      // Where all the action really happens
      int work(int noutput_items,
	      gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_REED_SOLOMON_DEC_ISDBT_IMPL_H */

