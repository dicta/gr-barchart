/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include "barchart/displayform.h"

#include <iostream>
#include <QPixmap>
#include <QFileDialog>


namespace gr {
namespace barchart {


DisplayForm::DisplayForm(int nplots, QWidget* parent)
  : QWidget(parent), d_nplots(nplots), d_system_specified_flag(false)
{
  d_isclosed = false;
  d_axislabels = true;

  // Set the initial plot size
  resize(QSize(800, 600));

  // Set up a grid that can be turned on/off
  d_grid = new QwtPlotGrid();
  QPen *gridpen = new QPen(Qt::DashLine);
  gridpen->setWidthF(0.25);
  gridpen->setColor(Qt::gray);
  d_grid->setPen(*gridpen);

  // Create a set of actions for the menu
  d_stop_act = new QAction("Stop", this);
  d_stop_act->setStatusTip(tr("Start/Stop"));
  connect(d_stop_act, SIGNAL(triggered()), this, SLOT(setStop()));
  d_stop_state = false;

  d_grid_act = new QAction("Grid", this);
  d_grid_act->setCheckable(true);
  d_grid_act->setStatusTip(tr("Toggle Grid on/off"));
  connect(d_grid_act, SIGNAL(triggered(bool)),
          this, SLOT(setGrid(bool)));
  d_grid_state = false;

  d_axislabelsmenu = new QAction("Axis Labels", this);
  d_axislabelsmenu->setCheckable(true);
  d_axislabelsmenu->setStatusTip(tr("Toggle Axis Labels on/off"));
  connect(d_axislabelsmenu, SIGNAL(triggered(bool)),
      this, SLOT(setAxisLabels(bool)));

  // Create a pop-up menu for manipulating the figure
  d_menu_on = true;
  d_menu = new QMenu(this);
  d_menu->addAction(d_stop_act);
  d_menu->addAction(d_grid_act);
  d_menu->addAction(d_axislabelsmenu);

  d_autoscale_act = new QAction("Auto Scale", this);
  d_autoscale_act->setStatusTip(tr("Autoscale Plot"));
  d_autoscale_act->setCheckable(true);
  connect(d_autoscale_act, SIGNAL(triggered(bool)),
	  this, SLOT(autoScale(bool)));
  d_autoscale_state = false;
  d_menu->addAction(d_autoscale_act);

  d_save_act = new QAction("Save", this);
  d_save_act->setStatusTip(tr("Save Figure"));
  connect(d_save_act, SIGNAL(triggered()), this, SLOT(saveFigure()));
  d_menu->addAction(d_save_act);

  Reset();
}

DisplayForm::~DisplayForm()
{
  d_isclosed = true;

  // Qt deletes children when parent is deleted
  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
}

void
DisplayForm::resizeEvent( QResizeEvent *e )
{
  //QSize s = size();
  //emit d_display_plot->resizeSlot(&s);
}

void
DisplayForm::mousePressEvent( QMouseEvent * e)
{
  bool ctrloff = Qt::ControlModifier != QApplication::keyboardModifiers();
  if((e->button() == Qt::MidButton) && ctrloff && (d_menu_on)) {
    if(d_stop_state == false)
      d_stop_act->setText(tr("Stop"));
    else
      d_stop_act->setText(tr("Start"));

    // Update the line titles if changed externally
    for(int i = 0; i < d_nplots; i++) {
      d_lines_menu[i]->setTitle(d_display_plot->getLineLabel(i));
    }
    d_menu->exec(e->globalPos());
  }
}

void
DisplayForm::updateGuiTimer()
{
  d_display_plot->canvas()->update();
}

void
DisplayForm::onPlotPointSelected(const QPointF p)
{
  emit plotPointSelected(p, 3);
}

void
DisplayForm::Reset()
{
}

bool
DisplayForm::isClosed() const
{
  return d_isclosed;
}

void
DisplayForm::enableMenu(bool en)
{
  d_menu_on = en;
}

void
DisplayForm::closeEvent(QCloseEvent *e)
{
  d_isclosed = true;
  qApp->processEvents();
  QWidget::closeEvent(e);
}

void
DisplayForm::setUpdateTime(double t)
{
  d_update_time = t;
}

void
DisplayForm::setTitle(const QString &title)
{
  d_display_plot->setTitle(title);
}

void
DisplayForm::setLineLabel(int which, const QString &label)
{
  d_display_plot->setLineLabel(which, label);
}

void
DisplayForm::setLineColor(int which, const QString &color)
{
  QColor c = QColor(color);
  d_display_plot->setLineColor(which, c);
  d_display_plot->replot();
}

void
DisplayForm::setLineWidth(int which, int width)
{
  d_display_plot->setLineWidth(which, width);
  d_display_plot->replot();
}

void
DisplayForm::setLineStyle(int which, Qt::PenStyle style)
{
  d_display_plot->setLineStyle(which, style);
  d_display_plot->replot();
}

void
DisplayForm::setLineMarker(int which, QwtSymbol::Style marker)
{
  d_display_plot->setLineMarker(which, marker);
  d_display_plot->replot();
}

void
DisplayForm::setMarkerAlpha(int which, int alpha)
{
  d_display_plot->setMarkerAlpha(which, alpha);
  d_display_plot->replot();
}

QString
DisplayForm::title()
{
  return d_display_plot->title().text();
}

QString
DisplayForm::lineLabel(int which)
{
  return d_display_plot->getLineLabel(which);
}

QString
DisplayForm::lineColor(int which)
{
  return d_display_plot->getLineColor(which).name();
}

int
DisplayForm::lineWidth(int which)
{
  return d_display_plot->getLineWidth(which);
}

Qt::PenStyle
DisplayForm::lineStyle(int which)
{
  return d_display_plot->getLineStyle(which);
}

QwtSymbol::Style
DisplayForm::lineMarker(int which)
{
  return d_display_plot->getLineMarker(which);
}

int
DisplayForm::markerAlpha(int which)
{
  return d_display_plot->getMarkerAlpha(which);
}


void
DisplayForm::setStop(bool on)
{
  if(!on) {
    // will auto-detach if already attached.
    d_display_plot->setStop(false);
    d_stop_state = false;
  }
  else {
    d_display_plot->setStop(true);
    d_stop_state = true;
  }
  d_display_plot->replot();
}

void
DisplayForm::setStop()
{
  if(d_stop_state == false)
    setStop(true);
  else
    setStop(false);
}

void
DisplayForm::setGrid(bool on)
{
  if(on) {
    // will auto-detach if already attached.
    d_grid->attach(d_display_plot);
    d_grid_state = true;
  }
  else {
    d_grid->detach();
    d_grid_state = false;
  }
  d_grid_act->setChecked(on);
  d_display_plot->replot();
}

void
DisplayForm::setAxisLabels(bool en)
{
  d_axislabels = en;
  d_axislabelsmenu->setChecked(en);
  getPlot()->setAxisLabels(d_axislabels);
}

void
DisplayForm::saveFigure()
{
  QPixmap qpix = QPixmap::grabWidget(this);

  QString types = QString(tr("JPEG file (*.jpg);;Portable Network Graphics file (*.png);;Bitmap file (*.bmp);;TIFF file (*.tiff)"));

  QString filename, filetype;
  QFileDialog *filebox = new QFileDialog(0, "Save Image", "./", types);
  filebox->setViewMode(QFileDialog::Detail);
  if(filebox->exec()) {
    filename = filebox->selectedFiles()[0];
    filetype = filebox->selectedNameFilter();
  }
  else {
    return;
  }

  if(filetype.contains(".jpg")) {
    qpix.save(filename, "JPEG");
  }
  else if(filetype.contains(".png")) {
    qpix.save(filename, "PNG");
  }
  else if(filetype.contains(".bmp")) {
    qpix.save(filename, "BMP");
  }
  else if(filetype.contains(".tiff")) {
    qpix.save(filename, "TIFF");
  }
  else {
    qpix.save(filename, "JPEG");
  }

  delete filebox;
}

void
DisplayForm::disableLegend()
{
  d_display_plot->disableLegend();
}


} // end namespace barchart
} // end namespace gr
