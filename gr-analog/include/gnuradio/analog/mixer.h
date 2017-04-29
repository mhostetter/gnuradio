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


#ifndef INCLUDED_ANALOG_MIXER_H
#define INCLUDED_ANALOG_MIXER_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief <+description of block+>
     * \ingroup analog
     *
     */
    class ANALOG_API mixer : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<mixer> sptr;

      enum change_mode {
        CHANGE_NEVER            = 0,
        CHANGE_WITH_TAGS        = 1,
        CHANGE_WITH_MESSAGES    = 2,
      };

      /*!
       */
      static sptr make(float sample_rate, bool conjugate, float init_frequency, float init_phase, enum change_mode mode, const std::string& freq_key, const std::string& phase_key);

      virtual void set_frequency(float frequency) = 0;
      virtual void set_phase(float phase) = 0;
    };

  } // namespace analog
} // namespace gr

#endif /* INCLUDED_ANALOG_MIXER_H */

