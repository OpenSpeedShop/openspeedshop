#ifndef CHARTFORM_H
#define CHARTFORM_H

#include "element.hxx"

#include <qmainwindow.h>
#include <qstringlist.h>

class CanvasView;

class QAction;
class QCanvas;
class QFont;
class QPrinter;
class QString;


enum ChartType { PIEWITHSHADOW, PIEWITHNOSHADOW, PIEWITH3D, VERTICAL_BAR, HORIZONTAL_BAR, BARWITH3D };

// class ChartForm: public QMainWindow
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

    virtual int mouseClicked(int item);
virtual void contentsContextMenuEvent( QContextMenuEvent *e );
    virtual QPopupMenu *createPopupMenu(const QPoint & pos);
    QPopupMenu *optionsMenu; // Why public? See canvasview.cpp

    void setValues(int values[], char *color_names[], char *strings[], int n);

    QCanvas *getCanvas() { return m_canvas; }

    bool fileSaveAsPixmap(QString filename);

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
    void init();
    void load( const QString& filename );
    bool okToClear();
    void drawPieChart( const double scales[], double total, int count );
    void drawVerticalBarChart( const double scales[], double total, int count );
    void drawHorizontalBarChart( const double scales[], double total, int count );
void draw3DBarChart( const double scales[], double total, int count );

    QString valueLabel( const QString& label, double value, double total );
    void updateRecentFiles( const QString& filename );
    void updateRecentFilesMenu();
    void setChartType( ChartType chartType );

    QPopupMenu *fileMenu;
    QAction *optionsPieChartActionWithShadow;
QAction *optionsPieChartActionWithNoShadow;
QAction *optionsPieChartActionWith3D;
QAction *optionsBarChartActionWith3D;
    QAction *optionsHorizontalBarChartAction;
    QAction *optionsVerticalBarChartAction;


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
