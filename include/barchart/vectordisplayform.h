/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef BARCHART_VECTOR_DISPLAY_FORM_H
#define BARCHART_VECTOR_DISPLAY_FORM_H

#include "barchart/VectorDisplayPlot.h"

#include <vector>
#include <QtGui/QtGui>

#include "barchart/displayform.h"

namespace gr {
namespace barchart {

/*!
 * \brief DisplayForm child for managing vector plots.
 * \ingroup qtgui_blk
 */
class VectorDisplayForm : public gr::barchart::DisplayForm
{
  Q_OBJECT

public:
  VectorDisplayForm(int nplots=1, QWidget* parent = 0);
  ~VectorDisplayForm();

  VectorDisplayPlot* getPlot();

  int getVecSize() const;
  float getVecAverage() const;

  void setXAxisLabel(const QString &label);
  void setYAxisLabel(const QString &label);

  // returns the x-Value that was last double-clicked on by the user
  float getClickedXVal() const;

  // checks if there was a double-click event; reset if there was
  bool checkClicked();

public slots:
  void customEvent(QEvent *e);

  void setVecSize(const int);

  void setXaxis(double start, double step);
  void setYaxis(double min, double max);
  void setYMax(const QString &m);
  void setYMin(const QString &m);
  void autoScale(bool en);

private slots:
  void newData(const QEvent *updateEvent);

private:
  uint64_t d_num_real_data_points;
  QIntValidator* d_int_validator;

  int d_vecsize;
  float d_vecavg;

  // TODO do I keep these?
  bool d_clicked;
  double d_clicked_x_level;
};


} // end namespace barchart
} // end namespace gr

#endif /* BARCHART_VECTOR_DISPLAY_FORM_H */
