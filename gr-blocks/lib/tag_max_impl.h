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

#ifndef INCLUDED_BLOCKS_TAG_MAX_IMPL_H
#define INCLUDED_BLOCKS_TAG_MAX_IMPL_H

#include <gnuradio/blocks/tag_max.h>

namespace gr {
  namespace blocks {

    class tag_max_impl : public tag_max
    {
     private:
      float d_threshold;
      int d_look_ahead;
      int d_tag_offset;
      int d_post_tag_blank;
      std::string d_tag_name;
      int d_start_idx;
      uint64_t d_blank_until;

     public:
      tag_max_impl(float threshold, int look_ahead, int tag_offset, int post_tag_blank, const std::string& tag_name);
      ~tag_max_impl();

      void set_threshold(float threshold);
      void set_look_ahead(int look_ahead);
      void set_tag_offset(int tag_offset);
      void set_post_tag_blank(int post_tag_blank);

      void forecast(int noutput_items, gr_vector_int& ninput_items_required);

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_TAG_MAX_IMPL_H */

