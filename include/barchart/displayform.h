/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef DISPLAY_FORM_H
#define DISPLAY_FORM_H

#include <QtGui/QtGui>
#include <vector>

#include "barchart/api.h"
#include "barchart/VectorDisplayPlot.h"

namespace gr {
namespace barchart {

/*!
 * \brief Base class for setting up and  managing QTGUI plot forms.
 * \ingroup qtgui_blk
 */
class BARCHART_API DisplayForm : public QWidget
{
  Q_OBJECT

  public:
  DisplayForm(int nplots=1, QWidget* parent = 0);
  ~DisplayForm();

  virtual VectorDisplayPlot* getPlot() = 0;
  void Reset();
  bool isClosed() const;

  void enableMenu(bool en=true);

public slots:
  void resizeEvent( QResizeEvent * e );
  void mousePressEvent( QMouseEvent * e);
  virtual void customEvent( QEvent * e ) = 0;

  void closeEvent( QCloseEvent * e );

  void setUpdateTime(double t);

  void setTitle(const QString &title);
  void setFillColor(int which, const QString &color);
  void setLineLabel(int which, const QString &label);
  void setLineColor(int which, const QString &color);
  void setLineWidth(int which, int width);
  void setLineStyle(int which, Qt::PenStyle style);
  void setMarkerAlpha(int which, int alpha);

  QString title();
  QString fillColor(int which);
  QString lineLabel(int which);
  QString lineColor(int which);
  int lineWidth(int which);
  Qt::PenStyle lineStyle(int which);
  int markerAlpha(int which);

  void setStop(bool on);
  void setStop();

  void setGrid(bool on);
  void setAxisLabels(bool en);

  void saveFigure();

  void disableLegend();

private slots:
  virtual void newData(const QEvent*) = 0;
  virtual void autoScale(bool) = 0;
  void updateGuiTimer();

signals:
  void toggleGrid(bool en);

protected:
  bool d_isclosed;

  int d_nplots;

  QGridLayout *d_layout;
  VectorDisplayPlot* d_display_plot;
  bool d_system_specified_flag;

  bool   d_menu_on;
  QMenu *d_menu;

  QAction *d_stop_act;
  bool d_stop_state;
  QAction *d_grid_act;
  bool d_grid_state;
  QAction *d_axislabelsmenu;
  bool d_axislabels;

  QAction *d_autoscale_act;
  bool d_autoscale_state;

  QList<QMenu*> d_lines_menu;

  QAction *d_save_act;

  double d_update_time;
};

} // end namespace barchart
} // end namespace gr

#endif /* DISPLAY_FORM_H */
