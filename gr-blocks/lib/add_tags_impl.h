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

#ifndef INCLUDED_BLOCKS_ADD_TAGS_IMPL_H
#define INCLUDED_BLOCKS_ADD_TAGS_IMPL_H

#include <gnuradio/blocks/add_tags.h>

namespace gr {
  namespace blocks {

    class add_tags_impl : public add_tags
    {
     private:
      size_t d_sizeof_io_item;
      pmt::pmt_t d_trigger_tag_key_pmt;
      pmt::pmt_t d_tag_key_pmt;
      std::vector<float> d_tag_values;
      std::vector<int> d_tag_delays;
      size_t d_vlen;

     public:
      add_tags_impl(size_t sizeof_io_item, const std::string& trigger_tag_key, const std::string& tag_key, const std::vector<float>& tag_values, const std::vector<int>& tag_delays, size_t vlen);
      ~add_tags_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_ADD_TAGS_IMPL_H */

