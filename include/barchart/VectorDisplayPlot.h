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

#ifndef VECTOR_DISPLAY_PLOT_HPP
#define VECTOR_DISPLAY_PLOT_HPP

#include <cstdint>
#include <cstdio>
#include <vector>

#include "barchart/api.h"
#include "qcustomplot/qcustomplot.h"

/*!
 * \brief QWidget for displaying 1D-vector plots.
 * \ingroup qtgui_blk
 */
class BARCHART_API VectorDisplayPlot : public QCustomPlot
{
Q_OBJECT

public:
  VectorDisplayPlot(int nplots, QWidget*);
  virtual ~VectorDisplayPlot();

  void setXAxisValues(const double start, const double step = 1.0);

  void plotNewData(
      const std::vector<std::vector<double>>& dataPoints,
      const int64_t numDataPoints,
      const double timeInterval
  );

  QString getTitle() const;
  void setTitle(const QString title);

  double getYMin() const;
  double getYMax() const;

  void setXAxisLabel(const QString &label);
  void setYAxisLabel(const QString &label);

  void setXAxisUnit(const QString &unit);
  void setYAxisUnit(const QString &unit);

  void setTraceColour (QColor);
  void setBGColour (QColor c);

public slots:

  void setStop(bool on);

  virtual void disableLegend();
  virtual void setAxisLabels(bool en);
  virtual void setYaxis(double min, double max);
  virtual void setXaxis(double min, double max);
  virtual void setFillColor(int which, QColor color);
  virtual QColor getFillColor(int which) const;
  virtual void setLineLabel(int which, QString label);
  virtual QString getLineLabel(int which);
  virtual void setLineColor(int which, QColor color);
  virtual QColor getLineColor(int which) const;
  virtual void setLineWidth(int which, int width);
  virtual int getLineWidth(int which) const;
  virtual void setLineStyle(int which, Qt::PenStyle style);
  virtual const Qt::PenStyle getLineStyle(int which) const;
  virtual void setMarkerAlpha(int which, int alpha);
  virtual int getMarkerAlpha(int which) const;

  void setAutoScale(bool state);

private:
  void _resetXAxisPoints();
  void _autoScale(double bottom, double top);

  int d_nplots;
  int64_t d_numPoints;

  std::vector<std::vector<double>> d_ydata;

  double d_x_axis_start;
  double d_x_axis_step;

  double d_ymax;
  double d_ymin;

  QString d_x_axis_label;
  QString d_y_axis_label;

  QCPTextElement* d_graph_title;

  bool d_stop = false;
  bool d_autoscale_state = false;

  QList<QColor> d_trace_colors;
};

#endif /* VECTOR_DISPLAY_PLOT_HPP */
