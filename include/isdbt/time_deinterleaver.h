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


#ifndef INCLUDED_ISDBT_TIME_DEINTERLEAVER_H
#define INCLUDED_ISDBT_TIME_DEINTERLEAVER_H

#include <isdbt/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief <+description of block+>
     * \ingroup isdbt
     *
     */
    class ISDBT_API time_deinterleaver : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<time_deinterleaver> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of isdbt::time_deinterleaver.
       *
       * To avoid accidental use of raw pointers, isdbt::time_deinterleaver's
       * constructor is in a private implementation
       * class. isdbt::time_deinterleaver::make is the public interface for
       * creating new instances.
       */
      static sptr make(int mode, int length);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_TIME_DEINTERLEAVER_H */

