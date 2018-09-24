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

#ifndef VECTOR_DISPLAY_PLOT
#define VECTOR_DISPLAY_PLOT

#include "barchart/VectorDisplayPlot.h"

#include <QColor>
#include <iostream>

#include "qcustomplot/qcustomplot.h"

/***********************************************************************
 * Main frequency display plotter widget
 **********************************************************************/
VectorDisplayPlot::VectorDisplayPlot(int nplots, QWidget* parent)
  : QCustomPlot(parent),
    d_nplots(nplots),
    d_x_axis_label("x"),
    d_y_axis_label("y")
{
  d_numPoints = 1024;
  d_x_axis_start = 0;
  d_x_axis_step = 1.0;
  d_ymin = -10;
  d_ymax = 10;

  QFont axisFont = font();
  axisFont.setBold(true);
  axisFont.setPointSize(12);

  QFont tickFont = font();
  tickFont.setBold(false);
  tickFont.setPointSize(11);

  QFont titleFont = font();
  titleFont.setBold(true);
  titleFont.setPointSize(18);

  d_xdata.resize(d_numPoints);
  std::fill(d_xdata.begin(), d_xdata.end(), 0x0);

  // Add all of our graphs
  for (int i = 0; i < nplots; i++)
  {
    addGraph();
  }

  this->xAxis->setLabel(d_x_axis_label);
  this->xAxis->setLabelFont(axisFont);
  this->xAxis->setTickLabelFont(tickFont);

  this->yAxis->setLabel(d_y_axis_label);
  this->yAxis->setLabelFont(axisFont);
  this->yAxis->setTickLabelFont(tickFont);

  this->xAxis->setRange(d_x_axis_start, d_x_axis_start + d_numPoints * d_x_axis_step);
  this->yAxis->setRange(d_ymin, d_ymax);

  // add title layout element, initially empty
  d_graph_title = new QCPTextElement(this, "", titleFont);
  plotLayout()->insertRow(0);
  plotLayout()->addElement(0, 0, d_graph_title);


  QList<QColor> default_colors;
  default_colors << QColor(Qt::blue) << QColor(Qt::red) << QColor(Qt::green)
	 << QColor(Qt::black) << QColor(Qt::cyan) << QColor(Qt::magenta)
	 << QColor(Qt::yellow) << QColor(Qt::gray) << QColor(Qt::darkRed)
	 << QColor(Qt::darkGreen) << QColor(Qt::darkBlue) << QColor(Qt::darkGray);

  // Create a curve for each input
  // Automatically deleted when parent is deleted
  d_ydata.clear();
  d_ydata.resize(d_nplots);
  for(int i = 0; i < d_nplots; i++) {
    d_ydata[i].resize(d_numPoints, 0x0);

#if 0
    d_plot_curve.push_back(new QwtPlotCurve(QString("Data %1").arg(i)));
    d_plot_curve[i]->attach(this);

    QwtSymbol *symbol = new QwtSymbol(QwtSymbol::NoSymbol, QBrush(default_colors[i]),
				      QPen(default_colors[i]), QSize(7,7));

#if QWT_VERSION < 0x060000
    d_plot_curve[i]->setRawData(d_xdata.data(), d_ydata[i].data(), d_numPoints);
    d_plot_curve[i]->setSymbol(*symbol);
#else
    d_plot_curve[i]->setRawSamples(d_xdata.data(), d_ydata[i].data(), d_numPoints);
    d_plot_curve[i]->setSymbol(symbol);
#endif
    setLineColor(i, default_colors[i]);
#endif
  }

  d_ref_level = -HUGE_VAL;

  _resetXAxisPoints();

  replot();
}

VectorDisplayPlot::~VectorDisplayPlot()
{}

QString VectorDisplayPlot::getTitle() const
{
    return d_graph_title->text();
}

void VectorDisplayPlot::setTitle(const QString title)
{
    d_graph_title->setText(title);
}

void
VectorDisplayPlot::setYaxis(double min, double max)
{
  // Get the new max/min values for the plot
  d_ymin = min;
  d_ymax = max;

  // Set the axis max/min to the new values
  this->yAxis->setRange(d_ymin, d_ymax);
}

double
VectorDisplayPlot::getYMin() const
{
  return d_ymin;
}

double
VectorDisplayPlot::getYMax() const
{
  return d_ymax;
}

void VectorDisplayPlot::setXAxisLabel(const QString &label)
{
  d_x_axis_label = label;
  this->xAxis->setLabel(label);
}

void VectorDisplayPlot::setYAxisLabel(const QString &label)
{
  d_y_axis_label = label;
  this->yAxis->setLabel(label);
}

void VectorDisplayPlot::setXAxisUnit(const QString &unit)
{
  if (!unit.isEmpty())
  {
    this->xAxis->setLabel(QString("%1 (%2)").arg(d_x_axis_label).arg(unit));
  }
  else
  {
    this->xAxis->setLabel(d_x_axis_label);
  }
}

void VectorDisplayPlot::setYAxisUnit(const QString &unit)
{
  if (!unit.isEmpty())
  {
    this->yAxis->setLabel(QString("%1 (%2)").arg(d_y_axis_label).arg(unit));
  }
  else
  {
    this->yAxis->setLabel(d_y_axis_label);
  }
}

// Change the display region of the x-axis (zoom) without changing
// the actual values that data is plotted at.
void VectorDisplayPlot::setXaxis(double min, double max)
{
    this->xAxis->setRange(min, max);
}

void
VectorDisplayPlot::setXAxisValues(
    const double start,
    const double step
) {
  bool reset = false;
  if((start != d_x_axis_start) || (step != d_x_axis_step))
    reset = true;

  d_x_axis_start = start;
  d_x_axis_step = step;

  if(reset) {
    _resetXAxisPoints();
  }
}

void
VectorDisplayPlot::plotNewData(
      const std::vector<std::vector<double>>& dataPoints,
      const int64_t numDataPoints,
      const double refLevel,
      const double timeInterval
) {
  if(!d_stop) {
    if(numDataPoints > 0) {

// FIXME IMPLEMENT
      _resetXAxisPoints();

      double bottom=1e20, top=-1e20;
      for(int n = 0; n < d_nplots; n++) {
        for(int64_t point = 0; point < numDataPoints; point++) {
            // Find overall top and bottom values in plot.
            // Used for autoscaling y-axis.
            bottom = std::min(bottom, dataPoints[n][point]);
            top    = std::max(top,    dataPoints[n][point]);
        }
      }

      if(d_autoscale_state)
        _autoScale(bottom, top);

      d_ref_level = refLevel;

      replot();
    }
  }
}

void
VectorDisplayPlot::_autoScale(double bottom, double top)
{
  // Auto scale the y-axis with a margin of 10 dB on either side.
  d_ymin = bottom-10;
  d_ymax = top+10;
  setYaxis(d_ymin, d_ymax);
}

void
VectorDisplayPlot::setAutoScale(bool state)
{
  d_autoscale_state = state;
}

void
VectorDisplayPlot::_resetXAxisPoints()
{
  for(int64_t loc = 0; loc < d_numPoints; loc++) {
    d_xdata[loc] = d_x_axis_start + loc * d_x_axis_step;
  }

  this->xAxis->setRange(d_xdata[0], d_xdata[d_numPoints-1]);
}

void
VectorDisplayPlot::setTraceColour(QColor c)
{
  graph(0)->setPen(QPen(c));
}

void
VectorDisplayPlot::setBGColour(QColor c)
{
  QPalette palette;
  palette.setColor(backgroundRole(), c);
  setPalette(palette);
}

void
VectorDisplayPlot::setStop(bool on)
{
  d_stop = on;
}

void
VectorDisplayPlot::setLineLabel(int which, QString label)
{
  if (which < d_nplots) {
    graph(which)->setName(label);
  }
}

QString
VectorDisplayPlot::getLineLabel(int which)
{
  return (which < d_nplots) ? graph(which)->name() : QString();
}

void
VectorDisplayPlot::setLineColor(int which, QColor color)
{
  if (which < d_nplots) {
    // Set the color of the pen
    QPen pen(graph(which)->pen());
    pen.setColor(color);
    graph(which)->setPen(pen);
  }
}

QColor
VectorDisplayPlot::getLineColor(int which) const
{
  // If that plot doesn't exist then return black.
  return (which < d_nplots) ? graph(which)->pen().color() : QColor("black");
}


void
VectorDisplayPlot::setLineWidth(int which, int width)
{
  if(which < d_nplots) {
    // Set the new line width
    QPen pen(graph(which)->pen());
    pen.setWidth(width);
    graph(which)->setPen(pen);
  }
}

int
VectorDisplayPlot::getLineWidth(int which) const
{
  return (which < d_nplots) ? graph(which)->pen().width() : 0;
}

void
VectorDisplayPlot::setLineStyle(int which, Qt::PenStyle style)
{
  if(which < d_nplots) {
    QPen pen(graph(which)->pen());
    pen.setStyle(style);
    graph(which)->setPen(pen);
  }
}

const Qt::PenStyle
VectorDisplayPlot::getLineStyle(int which) const
{
  return (which < d_nplots) ? graph(which)->pen().style() : Qt::SolidLine;
}

void
VectorDisplayPlot::setMarkerAlpha(int which, int alpha)
{
  if (which < d_nplots) {
    // Get the pen color
    QPen pen(graph(which)->pen());
    QColor color = pen.color();

    // Set new alpha and update pen
    color.setAlpha(alpha);
    pen.setColor(color);
    graph(which)->setPen(pen);
  }
}

int
VectorDisplayPlot::getMarkerAlpha(int which) const
{
  // If that plot doesn't exist then return transparent.
  return (which < d_nplots) ? graph(which)->pen().color().alpha() : 0;
}

void
VectorDisplayPlot::disableLegend()
{
  this->legend->setVisible(false);
}

void
VectorDisplayPlot::setAxisLabels(bool on)
{
  this->xAxis->setVisible(on);
  this->yAxis->setVisible(on);
}


#endif /* VECTOR_DISPLAY_PLOT */
