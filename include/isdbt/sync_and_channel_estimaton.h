/* -*- c++ -*- */
/* 
 * Copyright 2014 <+YOU OR YOUR COMPANY+>.
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


#ifndef INCLUDED_ISDBT_SYNC_AND_CHANNEL_ESTIMATON_H
#define INCLUDED_ISDBT_SYNC_AND_CHANNEL_ESTIMATON_H

#include <isdbt/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace isdbt {

    /*!
     * \brief <+description of block+>
     * \ingroup isdbt
     *
     */
    class ISDBT_API sync_and_channel_estimaton : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<sync_and_channel_estimaton> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of isdbt::sync_and_channel_estimaton.
       *
       * To avoid accidental use of raw pointers, isdbt::sync_and_channel_estimaton's
       * constructor is in a private implementation
       * class. isdbt::sync_and_channel_estimaton::make is the public interface for
       * creating new instances.
       */
      static sptr make(int fft_length, int payload_length, int offset_max);
    };

  } // namespace isdbt
} // namespace gr

#endif /* INCLUDED_ISDBT_SYNC_AND_CHANNEL_ESTIMATON_H */

