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
#include "tag_max_impl.h"
#include <volk/volk.h>

namespace gr {
  namespace blocks {

    tag_max::sptr
    tag_max::make(float threshold, int look_ahead, int tag_offset, int post_tag_blank, const std::string& tag_name)
    {
      return gnuradio::get_initial_sptr
        (new tag_max_impl(threshold, look_ahead, tag_offset, post_tag_blank, tag_name));
    }

    /*
     * The private constructor
     */
    tag_max_impl::tag_max_impl(float threshold, int look_ahead, int tag_offset, int post_tag_blank, const std::string& tag_name)
      : gr::sync_block("tag_max",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(float))),
        d_threshold(threshold),
        d_look_ahead(look_ahead),
        d_tag_offset(tag_offset),
        d_post_tag_blank(post_tag_blank),
        d_tag_name(tag_name),
        d_blank_until(0)
    {
      set_tag_offset(tag_offset);

      const int alignment_multiple = volk_get_alignment()/sizeof(float);
      set_alignment(std::max(1, alignment_multiple));

      set_tag_propagation_policy(TPP_ONE_TO_ONE);
    }

    /*
     * Our virtual destructor.
     */
    tag_max_impl::~tag_max_impl()
    {
    }

    void
    tag_max_impl::set_threshold(float threshold)
    {
      gr::thread::scoped_lock guard(d_setlock);
      d_threshold = threshold;
    }

    void
    tag_max_impl::set_look_ahead(int look_ahead)
    {
      gr::thread::scoped_lock guard(d_setlock);
      d_look_ahead = look_ahead;
    }

    void
    tag_max_impl::set_tag_offset(int tag_offset)
    {
      gr::thread::scoped_lock guard(d_setlock);
      d_tag_offset = tag_offset;
      if(d_tag_offset < 0) {
        // If we are going to apply the tag before the max is observed, we need to buffer
        // some samples in the work call. We cannot produce a sample before tagging it or 
        // there will be unexpected behavior. Therefore, we will search for a max starting
        // at d_start_idx in the work() call input_items and if a max is found, it will get
        // tagged before the work() call returns.
        d_start_idx = -1*d_tag_offset;
      }
      else {
        // Tagging in the future is always safe. As long as the sample is tagged before
        // the downstream blocks call get_tags_in_range().
        d_start_idx = 0;
      }
    }

    void
    tag_max_impl::set_post_tag_blank(int post_tag_blank)
    {
      gr::thread::scoped_lock guard(d_setlock);
      d_post_tag_blank = post_tag_blank;
    }

    void
    tag_max_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
    {
      for(unsigned int i = 0; i < ninput_items_required.size(); i++) {
        // We need d_start_idx items buffered at the beginning of the input buffer
        // and d_look_ahead items buffered at the end of the input buffer. The beginning
        // items are needed so our retroactive tag can still be made before that item is sent
        // downstream. The ending items are needed so if a threshold crossing is found, we can
        // search across the d_look_ahead range for the absolute max.
        ninput_items_required[i] = d_start_idx + noutput_items + d_look_ahead;
      }
    }

    int
    tag_max_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock guard(d_setlock);

      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];

      uint16_t max_idx;
      uint64_t max_offset;
      float max;

      for(int i = d_start_idx; i < d_start_idx + noutput_items; i++) {
        if(nitems_read(0) + i >= d_blank_until && in[i] > d_threshold) {
          // Found a threshold crossing. Now find absolute max in
          // d_look_ahead samples
          // d_look_ahead = 0 (1st threshold crossing is max)
          volk_32f_index_max_16u(&max_idx, &in[i], d_look_ahead + 1);
          max_offset = nitems_read(0) + i + max_idx;
          max = in[i + max_idx];

          // Add tag
          add_item_tag(
            0,
            max_offset + d_tag_offset,
            pmt::string_to_symbol(d_tag_name),
            pmt::from_double((double) max),
            alias_pmt()
          );

          d_blank_until = max_offset + d_tag_offset + d_post_tag_blank + 1;
        }
      }

      memcpy(out, in, sizeof(float)*noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */

