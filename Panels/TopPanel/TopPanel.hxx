////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


#ifndef TOPPANEL_H
#define TOPPANEL_H
#include "Panel.hxx"           // Do not remove

class PanelContainer;   // Do not remove
class CollectorInfo;

#include "TPTextEdit.hxx"
#include <qsplitter.h>
#include <qtextedit.h>
#include <qlistview.h>
#include <qcolor.h>
#include <qfile.h>
#include <qtextstream.h>

class TPChartForm;
class QHBoxLayout;

#undef PANEL_CLASS_NAME
#define PANEL_CLASS_NAME TopPanel   // Change the value of the define

                                         // to the name of your new class.
//! TopPanel displays the top 'n' metrics.
class TopPanel  : public Panel
{
  Q_OBJECT
public:
  //! Default construct - unused.
  TopPanel();

  //! Constructs a TopPanel consisting of a left side graph and a right side text.
  TopPanel(PanelContainer *pc, const char *n, void *argument);

  //! Destructor
  ~TopPanel();  // Active destructor


  //! Create a context sensitive menu.
  bool menu(QPopupMenu* contextMenu);

  //! Save the contents of the TopPanel
  void save();

  //! Listener routine for the TopPanel
  int listener(void *msg);

  //! prototype: get the top 'n' values
  int getValues(int values[], char *colors[], char *strings[], int n);

  //! prototype: get the top 5 data.
  void getTopFiveData();

  //! Item at index 'i' was selected.
  void itemSelected(int i);

  //! Create a context senstive menu for the TextEdit (right side).
  void createTextEditPopupMenu( QPopupMenu *popupMenu, const QPoint &pos );

  //! Create a context sensitive menu for the chart (left side).
  void createChartPopupMenu( QPopupMenu *popupMenu, const QPoint &pos );

  //! truncate the character string.
  char *truncate(char *str, int length);

  //! Highlight a line in the TextEdit.
  void highlightLine(int line, char *color, bool inverse=FALSE);

  //! Flag setting, indicating if we should be displaying the statistics.
  bool statsFLAG;

  //! Flag setting, indicating if we should be displaying the chart.
  bool chartFLAG;

  //! The main layout manager for the panel internals.
  QHBoxLayout * frameLayout;

  //! A splitter.
  QSplitter *splitterA; 

  //! The chart (left side)
  TPChartForm *cf;

  //! The data list (right side)
  QTextEdit *textEdit;

  //! If the line is highlighted, return its' description.
  QString getDescription(int line);

//! This is underlying save functionality.
void doSaveAs(QFile *f, QTextStream *ts);

public slots:
  void setOrientation();

  void saveAs();
  void aboutExperiment();
  void listClicked(int para, int offset);
  void details();

  void zoomIn();
  void zoomOut();

  void showStats();
  void showChart();

protected:
  //! prototype: dummy data.
  CollectorInfo *topFiveObject;

private:
  void info(QPoint, QObject *target=NULL );
};
#endif // TOPPANEL_H
