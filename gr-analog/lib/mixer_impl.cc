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
#include "mixer_impl.h"

namespace gr {
  namespace analog {


    mixer::sptr
    mixer::make(float sample_rate, bool conjugate, float init_frequency, float init_phase, enum change_mode mode, const std::string& freq_key, const std::string& phase_key)
    {
      return gnuradio::get_initial_sptr
        (new mixer_impl(sample_rate, conjugate, init_frequency, init_phase, mode, freq_key, phase_key));
    }

    /*
     * The private constructor
     */
    mixer_impl::mixer_impl(float sample_rate, bool conjugate, float init_frequency, float init_phase, enum change_mode mode, const std::string& freq_key, const std::string& phase_key)
      : gr::sync_block("mixer",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        d_sample_rate(sample_rate),
        d_conjugate(conjugate),
        d_mode(mode),
        d_freq_key_pmt(pmt::intern(freq_key)),
        d_phase_key_pmt(pmt::intern(phase_key))
    {
      update_frequency(init_frequency);
      update_phase(init_phase);

      const int alignment_multiple = volk_get_alignment()/sizeof(gr_complex);
      set_alignment(std::max(1, alignment_multiple));

      set_tag_propagation_policy(TPP_ONE_TO_ONE);

      if(d_mode == CHANGE_WITH_MESSAGES) {
        message_port_register_in(pmt::string_to_symbol("freq"));
        set_msg_handler(pmt::string_to_symbol("freq"), boost::bind(&mixer_impl::process_message, this, _1));
      }
    }

    /*
     * Our virtual destructor.
     */
    mixer_impl::~mixer_impl()
    {
    }

    void
    mixer_impl::set_frequency(float frequency)
    {
      gr::thread::scoped_lock guard(d_setlock);
      update_frequency(frequency);
    }

    void
    mixer_impl::set_phase(float phase)
    {
      gr::thread::scoped_lock guard(d_setlock);
      update_phase(phase);
    }

    void
    mixer_impl::update_frequency(float frequency)
    {
      if(d_conjugate) {
        d_frequency = -1.0f*frequency;
      }
      else {
        d_frequency = frequency;
      }
      d_nco.set_freq(2*M_PI*d_frequency/d_sample_rate);
    }

    void
    mixer_impl::update_phase(float phase)
    {
      d_phase = phase;
      d_nco.set_phase(d_phase);
    }

    void
    mixer_impl::process_tag(tag_t tag)
    {
      if(tag.key == d_freq_key_pmt && pmt::is_real(tag.value)) {
        update_frequency(pmt::to_double(tag.value));
        remove_item_tag(0, tag);
      }
      if(tag.key == d_phase_key_pmt && pmt::is_real(tag.value)) {
        update_phase(pmt::to_double(tag.value));
        remove_item_tag(0, tag);
      }
    }

    void
    mixer_impl::process_message(pmt::pmt_t msg)
    {
      if(pmt::is_pair(msg)) {
        if(pmt::car(msg) == d_freq_key_pmt && pmt::is_real(pmt::cdr(msg))) {
          set_frequency(pmt::to_double(pmt::cdr(msg)));
        }
        if(pmt::car(msg) == d_phase_key_pmt && pmt::is_real(pmt::cdr(msg))) {
          set_phase(pmt::to_double(pmt::cdr(msg)));
        }
      }
    }

    void
    mixer_impl::generate_lo_and_mix(const gr_complex* in, gr_complex* out, int len)
    {
      // Generate LO signal and temporarily put it in the output buffer
      d_nco.sincos(out, len);

      // Mix the input with the LO and store in the output buffer
      volk_32fc_x2_multiply_32fc(out, in, out, len);
    }

    int
    mixer_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock guard(d_setlock);

      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      
      if(d_mode == CHANGE_WITH_TAGS) {
        std::vector<tag_t> freq_tags;
        get_tags_in_range(freq_tags, 0, nitems_read(0), nitems_read(0) + noutput_items, d_freq_key_pmt);

        int start_idx = 0;
        int stop_idx;
        for(int i = 0; i < freq_tags.size(); i++) {
          // Use current freq and mix the samples that came before this tag
          stop_idx = (int) (freq_tags[i].offset - nitems_read(0));
          generate_lo_and_mix(&in[start_idx], &out[start_idx], stop_idx - start_idx);

          // Update freq or phase from this tag depending on the key
          process_tag(freq_tags[i]);

          start_idx = stop_idx;
        }
        stop_idx = noutput_items;

        // Final mix for the remaining samples in the buffer
        generate_lo_and_mix(&in[start_idx], &out[start_idx], stop_idx - start_idx);
      }
      else {
        generate_lo_and_mix(in, out, noutput_items);
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace analog */
} /* namespace gr */

