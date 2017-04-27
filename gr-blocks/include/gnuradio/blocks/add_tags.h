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


#ifndef INCLUDED_BLOCKS_ADD_TAGS_H
#define INCLUDED_BLOCKS_ADD_TAGS_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Adds an array of tags to the stream triggered by a tag key
     * \ingroup blocks
     *
     * \details
     * This block will add an array of tags upon detecting a trigger tag with a particular
     * key. The added tags will have a common key, but variable values and offsets. The values
     * are specified as a vector of floats. The offsets are specified as a vector of sample delays
     * relative to the trigger tag offset. All tag delays must be non-negative, i.e. downstream
     * from the trigger tag. A delay of zero will add the tag to the same sample as the trigger
     * tag. The value and delay vectors must be of same length.
     */
    class BLOCKS_API add_tags : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<add_tags> sptr;

      /*!
       * \param sizeof_io_item The size of the input/output item stream
       * \param trigger_tag_key The key of the tag to trigger off
       * \param tag_key The key of the added tags
       * \param tag_values The float values of the added tags
       * \param tag_delays The integer sample delays of each tag relative to the trigger tag, each
       * must be non-negative
       * \param vlen The vector size of the input and output stream items
       */
      static sptr make(size_t sizeof_io_item, const std::string& trigger_tag_key, const std::string& tag_key, const std::vector<float>& tag_values, const std::vector<int>& tag_delays, size_t vlen = 1);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_ADD_TAGS_H */

