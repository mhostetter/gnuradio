/* -*- c++ -*- */
/* 
 * Copyright 2017 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_BLOCKS_TAG_MAX_H
#define INCLUDED_BLOCKS_TAG_MAX_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief <+description of block+>
     * \ingroup blocks
     *
     */
    class BLOCKS_API tag_max : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<tag_max> sptr;

      /*!
       * 
       */
      static sptr make(float threshold, int look_ahead, int tag_offset, int post_tag_blank, const std::string& tag_name);

      virtual void set_threshold(float threshold) = 0;
      virtual void set_look_ahead(int look_ahead) = 0;
      virtual void set_tag_offset(int tag_offset) = 0;
      virtual void set_post_tag_blank(int post_tag_blank) = 0;
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_TAG_MAX_H */

