/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#include "barchart/vectordisplayform.h"

#include <cmath>
#include <iostream>
#include <QMessageBox>

#include "barchart/DataUpdateEvent.h"
#include "qcustomplot/qcustomplot.h"

namespace gr {
namespace barchart {

VectorDisplayForm::VectorDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  d_int_validator = new QIntValidator(this);
  d_int_validator->setBottom(0);

  d_layout = new QGridLayout(this);
  d_display_plot = new VectorDisplayPlot(nplots, this);
  d_layout->addWidget(d_display_plot, 0, 0);
  setLayout(d_layout);

  d_num_real_data_points = 1024;
  d_vecsize = 1024;
  d_vecavg = 1.0;
  d_ref_level = 0.0;
  d_clicked = false;
  d_clicked_x_level = 0;

  Reset();

  connect(d_display_plot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onPlotPointSelected(const QPointF)));
}

VectorDisplayForm::~VectorDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete d_int_validator;
}

VectorDisplayPlot*
VectorDisplayForm::getPlot()
{
  return ((VectorDisplayPlot*)d_display_plot);
}

void
VectorDisplayForm::newData(const QEvent *updateEvent)
{
  DataUpdateEvent *fevent = (DataUpdateEvent*)updateEvent;

  getPlot()->plotNewData(
      fevent->getPoints(),
      fevent->getNumDataPoints(),
      d_ref_level,
      d_update_time
  );
}

void
VectorDisplayForm::customEvent( QEvent * e)
{
  // We just re-use DataUpdateEvent as long as that works
  if(e->type() == DataUpdateEvent::Type()) {
    newData(e);
  }
}

int
VectorDisplayForm::getVecSize() const
{
  return d_vecsize;
}

float
VectorDisplayForm::getVecAverage() const
{
  return d_vecavg;
}

void VectorDisplayForm::setXAxisLabel(const QString &label)
{
  getPlot()->setXAxisLabel(label);
}

void VectorDisplayForm::setYAxisLabel(const QString &label)
{
  getPlot()->setYAxisLabel(label);
}

void VectorDisplayForm::setRefLevel(double refLevel)
{
  d_ref_level = refLevel;
}

void
VectorDisplayForm::setVecSize(const int newsize)
{
  d_vecsize = newsize;
  getPlot()->replot();
}

void VectorDisplayForm::setXaxis(double start, double step)
{
  getPlot()->setXAxisValues(start, step);
}

void
VectorDisplayForm::setYaxis(double min, double max)
{
  getPlot()->setYaxis(min, max);
}

void
VectorDisplayForm::setYMax(const QString &m)
{
  double new_max = m.toDouble();
  double cur_ymin = getPlot()->getYMin();
  if(new_max > cur_ymin)
    setYaxis(cur_ymin, new_max);
}

void
VectorDisplayForm::setYMin(const QString &m)
{
  double new_min = m.toDouble();
  double cur_ymax = getPlot()->getYMax();
  if(new_min < cur_ymax)
    setYaxis(new_min, cur_ymax);
}

void
VectorDisplayForm::autoScale(bool en)
{
  if(en) {
    d_autoscale_state = true;
  }
  else {
    d_autoscale_state = false;
  }

  d_autoscale_act->setChecked(en);
  getPlot()->setAutoScale(d_autoscale_state);
  getPlot()->replot();
}

void
VectorDisplayForm::onPlotPointSelected(const QPointF p)
{
  d_clicked = true;
  d_clicked_x_level = p.x();
}

bool
VectorDisplayForm::checkClicked()
{
  if(d_clicked) {
    d_clicked = false;
    return true;
  }
  else {
    return false;
  }
}

float
VectorDisplayForm::getClickedXVal() const
{
  return d_clicked_x_level;
}

} // end namespace barchart
} // end namespace gr
