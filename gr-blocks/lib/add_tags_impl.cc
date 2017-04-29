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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "add_tags_impl.h"

namespace gr {
  namespace blocks {

    add_tags::sptr
    add_tags::make(size_t sizeof_io_item, const std::string& trigger_tag_key, const std::string& tag_key, const std::vector<float>& tag_values, const std::vector<int>& tag_delays, size_t vlen)
    {
      return gnuradio::get_initial_sptr
        (new add_tags_impl(sizeof_io_item, trigger_tag_key, tag_key, tag_values, tag_delays, vlen));
    }

    /*
     * The private constructor
     */
    add_tags_impl::add_tags_impl(size_t sizeof_io_item, const std::string& trigger_tag_key, const std::string& tag_key, const std::vector<float>& tag_values, const std::vector<int>& tag_delays, size_t vlen)
      : gr::sync_block("add_tags",
              gr::io_signature::make(1, 1, sizeof(sizeof_io_item*vlen)),
              gr::io_signature::make(1, 1, sizeof(sizeof_io_item*vlen))),
        d_sizeof_io_item(sizeof_io_item),
        d_trigger_tag_key_pmt(pmt::intern(trigger_tag_key)),
        d_tag_key_pmt(pmt::intern(tag_key)),
        d_tag_delays(tag_delays),
        d_tag_values(tag_values),
        d_vlen(vlen)
    {
      if(d_tag_delays.size() != d_tag_values.size()) {
        GR_LOG_ERROR(d_logger, "Value vector length not equal to delay vector length");
        throw std::length_error("Value vector length not equal to delay vector length");
      }
      for(unsigned int i = 0; i < d_tag_delays.size(); i++) {
        if(d_tag_delays[i] < 0) {
          GR_LOG_ERROR(d_logger, "Not all tag delays are non-negative");
          throw std::range_error("Not all tag delays are non-negative");
        }
      }

      set_tag_propagation_policy(TPP_ONE_TO_ONE);
    }

    /*
     * Our virtual destructor.
     */
    add_tags_impl::~add_tags_impl()
    {
    }

    int
    add_tags_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const void *in = (const void *) input_items[0];
      void *out = (void *) output_items[0];

      // Get all trigger tags
      std::vector<tag_t> tags;
      get_tags_in_range(tags, 0, nitems_written(0), nitems_written(0) + noutput_items, d_trigger_tag_key_pmt);

      // Add new tags relative to each trigger tag
      for(unsigned int i = 0; i < tags.size(); i++) {
        for(unsigned int j = 0; j < d_tag_values.size(); j++) {
          add_item_tag(0, tags[i].offset + d_tag_delays[j], d_tag_key_pmt, pmt::from_double((double) d_tag_values[j]), pmt::intern(alias()));
        }
      }
           
      // Input 0 passes through to Output 0
      memcpy(out, in, d_sizeof_io_item*d_vlen*noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */

