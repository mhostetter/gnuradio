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

#ifndef INCLUDED_ANALOG_MIXER_IMPL_H
#define INCLUDED_ANALOG_MIXER_IMPL_H

#include <gnuradio/analog/mixer.h>
#include <gnuradio/fxpt_nco.h>

namespace gr {
  namespace analog {

    class mixer_impl : public mixer
    {
     private:
      float d_sample_rate;
      bool d_conjugate;
      enum change_mode d_mode;
      std::string d_freq_key;
      std::string d_phase_key;
      float d_frequency;
      float d_phase;
      gr::fxpt_nco d_nco;

     public:
      mixer_impl(float sample_rate, bool conjugate, float init_frequency, float init_phase, enum change_mode mode, const std::string& freq_key, const std::string& phase_key);
      ~mixer_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

      void set_frequency(float frequency);
      void set_phase(float phase);
      
    };

  } // namespace analog
} // namespace gr

#endif /* INCLUDED_ANALOG_MIXER_IMPL_H */

