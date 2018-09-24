/* -*- c++ -*- */
/*
 * Copyright 2018 Free Software Foundation, Inc.
 *
 * This file is part of gr-barchart
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

#ifndef BARCHART_DATAUPDATEEVENT_H_
#define BARCHART_DATAUPDATEEVENT_H_

#include <cstdint>
#include <complex>
#include <vector>

#include <QEvent>
#include <QString>

#include <gnuradio/high_res_timer.h>
#include <gnuradio/tags.h>

#include "barchart/api.h"


namespace gr {
namespace barchart {

// base gnuradio-qtgui 3.7.x uses enums starting from 10005,
// we need a unique enum so offset by a thousand for now.
static const int DataUpdateEventType = 11005;

class BARCHART_API DataUpdateEvent : public QEvent
{
public:
    DataUpdateEvent(const std::vector<std::vector<double>>& dataPoints)
    : QEvent(QEvent::Type(DataUpdateEventType)),
      _dataPoints(dataPoints)
    {}

    ~DataUpdateEvent()
    {}

    int which() const;

    const std::vector<std::vector<double>>& getPoints() const
    {
        return _dataPoints;
    }

    uint64_t getNumDataPoints() const
    {
        return _dataPoints.empty() ? 0 : _dataPoints.at(0).size();
    }

    bool getRepeatDataFlag() const;

    static QEvent::Type Type()
    { return QEvent::Type(DataUpdateEventType); }

protected:

private:
  std::vector<std::vector<double>> _dataPoints;
  uint64_t _numDataPoints;
};

} // end namespace barchart
} // end namespace gr

#endif /* !defined(BARCHART_DATAUPDATEEVENT_H_) */
