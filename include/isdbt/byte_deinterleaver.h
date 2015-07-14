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


#ifndef INCLUDED_ISDBT_BYTE_DEINTERLEAVER_H
#define INCLUDED_ISDBT_BYTE_DEINTERLEAVER_H

#include <isdbt/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief A simple Forney byte de-interleaver, as specified in the standard. 
     * 
     * \ingroup isdbt
     *
     */
    class ISDBT_API byte_deinterleaver : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<byte_deinterleaver> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of isdbt::byte_deinterleaver.
       *
       */
      static sptr make();
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_BYTE_DEINTERLEAVER_H */

