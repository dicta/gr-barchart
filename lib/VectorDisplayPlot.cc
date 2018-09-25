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

  QList<QColor> default_colors;
  default_colors << QColor(Qt::blue) << QColor(Qt::red) << QColor(Qt::green)
	 << QColor(Qt::black) << QColor(Qt::cyan) << QColor(Qt::magenta)
	 << QColor(Qt::yellow) << QColor(Qt::gray) << QColor(Qt::darkRed)
	 << QColor(Qt::darkGreen) << QColor(Qt::darkBlue) << QColor(Qt::darkGray);

  // Add all of our graphs
  for (int i = 0; i < nplots; i++)
  {
    int color_index = i % default_colors.size();

    auto plottable = new QCPBars(this->xAxis, this->yAxis);
    plottable->setName(QString("Data %1").arg(i));
    plottable->setBrush(QBrush(default_colors[color_index]));
    plottable->setPen(QColor(Qt::black));
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

  d_ref_level = -HUGE_VAL;

  // Setup legend. By default, the legend is in the inset layout of the
  // main axis rect, but we're going to place it outside the plot.
  // note, if only one plot, it's just a waste of space to draw the legend.
  if (nplots > 1)
  {
      QCPLayoutGrid *subLayout = new QCPLayoutGrid;
      plotLayout()->addElement(1, 1, subLayout);
      subLayout->addElement(0, 0, legend);
      subLayout->addElement(1, 0, new QCPLayoutElement());
      subLayout->setRowStretchFactor(0, 0.05);
      subLayout->setMargins(QMargins(5,5,5,5)); // 5px space around legend, else its borders can get clipped
      legend->setVisible(true);

      // set the stretch factor of the legend's column to something tiny
      // to keep it always at whatever its minimum size is.
      plotLayout()->setColumnStretchFactor(1, 0.01);

      QFont legendFont = font();
      legendFont.setPointSize(10);
      legend->setFont(legendFont);
  }

  // Enable user drag to pan and wheel to zoom interactions.
  // Axes can also be selected to allow the user to pan and zoom in one direction only.
  setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | iSelectAxes);

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
      if(numDataPoints != d_numPoints)
      {
          d_numPoints = numDataPoints;
          _resetXAxisPoints();
      }
      double bottom=1e20, top=-1e20;

      for(int n = 0; n < d_nplots; n++) {
        QCPBars* bars = dynamic_cast<QCPBars*>(plottable(n));
        if (!bars)
        {
            continue; // we're something else
        }
        QVector<double> keys(numDataPoints);
        QVector<double> values(numDataPoints);
        for(int64_t point = 0; point < numDataPoints; point++) {
            keys[point] = d_x_axis_start + point * d_x_axis_step;
            values[point] = dataPoints[n][point];

            // Find overall top and bottom values in plot.
            // Used for autoscaling y-axis.
            bottom = std::min(bottom, values[point]);
            top    = std::max(top,    values[point]);
        }
        bars->setWidth(0.66 * d_x_axis_step); // FIXME make configurable
        bars->setData(keys, values, true);
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
  this->xAxis->setRange(d_x_axis_start - 0.5 * d_x_axis_step, d_x_axis_start + d_numPoints * d_x_axis_step - 0.5 * d_x_axis_step);
}

void
VectorDisplayPlot::setTraceColour(QColor c)
{
  plottable(0)->setPen(QPen(c));
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
    plottable(which)->setName(label);
  }
}

QString
VectorDisplayPlot::getLineLabel(int which)
{
  return (which < d_nplots) ? plottable(which)->name() : QString();
}

void
VectorDisplayPlot::setFillColor(int which, QColor color)
{
  if (which < d_nplots) {
    // Set the color of the brush
    QBrush brush(plottable(which)->brush());
    brush.setColor(color);
    plottable(which)->setBrush(brush);
  }
}

QColor
VectorDisplayPlot::getFillColor(int which) const
{
  // If that plot doesn't exist then return black.
  return (which < d_nplots) ? plottable(which)->brush().color() : QColor("black");
}


void
VectorDisplayPlot::setLineColor(int which, QColor color)
{
  if (which < d_nplots) {
    // Set the color of the pen
    QPen pen(plottable(which)->pen());
    pen.setColor(color);
    plottable(which)->setPen(pen);
  }
}

QColor
VectorDisplayPlot::getLineColor(int which) const
{
  // If that plot doesn't exist then return black.
  return (which < d_nplots) ? plottable(which)->pen().color() : QColor("black");
}


void
VectorDisplayPlot::setLineWidth(int which, int width)
{
  if(which < d_nplots) {
    // Set the new line width
    QPen pen(plottable(which)->pen());
    pen.setWidth(width);
    plottable(which)->setPen(pen);
  }
}

int
VectorDisplayPlot::getLineWidth(int which) const
{
  return (which < d_nplots) ? plottable(which)->pen().width() : 0;
}

void
VectorDisplayPlot::setLineStyle(int which, Qt::PenStyle style)
{
  if(which < d_nplots) {
    QPen pen(plottable(which)->pen());
    pen.setStyle(style);
    plottable(which)->setPen(pen);
  }
}

const Qt::PenStyle
VectorDisplayPlot::getLineStyle(int which) const
{
  return (which < d_nplots) ? plottable(which)->pen().style() : Qt::SolidLine;
}

void
VectorDisplayPlot::setMarkerAlpha(int which, int alpha)
{
  if (which < d_nplots) {
    // Get the pen color
    QBrush brush(plottable(which)->brush());
    QColor color = brush.color();

    // Set new alpha and update pen
    color.setAlpha(alpha);
    brush.setColor(color);
    plottable(which)->setBrush(brush);
  }
}

int
VectorDisplayPlot::getMarkerAlpha(int which) const
{
  // If that plot doesn't exist then return transparent.
  return (which < d_nplots) ? plottable(which)->brush().color().alpha() : 0;
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
