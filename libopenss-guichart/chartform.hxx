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


#ifndef CHARTFORM_H
#define CHARTFORM_H

#include <stddef.h>
#include "element.hxx"

#include <qmainwindow.h>
#include <qstringlist.h>
#include <qvaluelist.h>
typedef QValueList<QString> ChartTextValueList;
typedef QValueList<int> ChartPercentValueList;

class CanvasView;

class QAction;
class QCanvas;
class QFont;
class QPrinter;
class QString;
class QLabel;

static const char *chart_colors[] = {
  "red",
  "magenta",
  "blue",
  "cyan",
  "green",
  "yellow",
  "gray",
  "lightGray"
  "darkRed",
  "darkMagenta",
  "darkBlue",
  "darkCyan",
  "darkGreen",
  "darkYellow",
};
#define MAX_COLOR_CNT 14


enum ChartType { PIEWITHSHADOW, VERTICAL_BAR, HORIZONTAL_BAR, PIEWITHNOSHADOW, PIEWITH3D, BARWITH3D };

//! The container class for the charts (pie and bar)
class ChartForm: public QWidget
{
    Q_OBJECT
public:
    enum { MAX_ELEMENTS = 512 };
    enum { MAX_RECENTFILES = 9 }; // Must not exceed 9
    enum AddValuesType { NO, YES, AS_PERCENTAGE };

    ChartForm( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ChartForm();

    int chartType() { return m_chartType; }
    void setChanged( bool changed = TRUE ) { m_changed = changed; }
    void drawElements();

    int getItemFromPos( QPoint pos );

    virtual int mouseClicked(int item);
    virtual void contentsContextMenuEvent( QContextMenuEvent *e );
    virtual QPopupMenu *createPopupMenu(const QPoint & pos);
    QPopupMenu *optionsMenu; // Why public? See canvasview.cpp

    void setValues(ChartPercentValueList values, ChartTextValueList strings, const  char **color_names=chart_colors, int max_color_cnt=MAX_COLOR_CNT);

    QCanvas *getCanvas() { return m_canvas; }

    bool fileSaveAsPixmap(QString filename);

    void init();

    void setHeader( QString );
    void setChartType( ChartType chartType );
protected:
    virtual void closeEvent( QCloseEvent * );

private slots:
    void fileNew();
    void fileOpen();
    void fileOpenRecent( int index );
    void fileSave();
    void fileSaveAs();
    void fileSaveAsPixmap();
    void filePrint();
    void fileQuit();
    void optionsSetData();
    void updateChartType( QAction *action );
    void optionsSetFont();
    void optionsSetOptions();
    void helpHelp();
    void helpAbout();
    void helpAboutQt();
    void saveOptions();

private:
    void load( const QString& filename );
    bool okToClear();
    void drawPieChart( const double scales[], double total, int count );
    void drawVerticalBarChart( const double scales[], double total, int count );
    void drawHorizontalBarChart( const double scales[], double total, int count );
    void draw3DBarChart( const double scales[], double total, int count );

    QString valueLabel( const QString& label, double value, double total );
    void updateRecentFiles( const QString& filename );
    void updateRecentFilesMenu();

    QPopupMenu *fileMenu;
    QAction *optionsPieChartActionWithShadow;
    QAction *optionsPieChartActionWithNoShadow;
    QAction *optionsPieChartActionWith3D;
    QAction *optionsBarChartActionWith3D;
    QAction *optionsHorizontalBarChartAction;
    QAction *optionsVerticalBarChartAction;


    QLabel *headerLabel;
    QString m_filename;
    QStringList m_recentFiles;
    QCanvas *m_canvas;
    CanvasView *m_canvasView;
    bool m_changed;
    QPrinter *m_printer;
    ChartType m_chartType;
    ElementVector m_elements;
    AddValuesType m_addValues;
    int m_decimalPlaces;
    QFont m_font;
};

#endif
