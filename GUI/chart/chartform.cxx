#include "canvasview.hxx"
#include "chartform.hxx"
#include "optionsform.hxx"
#include "setdataform.hxx"

#include <qaction.h>
#include <qapplication.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qfont.h>
#include <qfontdialog.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qradiobutton.h>
#include <qsettings.h>
#include <qspinbox.h>
#include <qstatusbar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>

#include <qtextedit.h> // For testing...
#include <qlayout.h> // For testing...

#include "images/file_new.xpm"
#include "images/file_open.xpm"
#include "images/file_save.xpm"
#include "images/file_print.xpm"
#include "images/options_setdata.xpm"
#include "images/options_setfont.xpm"
#include "images/options_setoptions.xpm"
#include "images/options_horizontalbarchart.xpm"
#include "images/options_piechart.xpm"
#include "images/options_barchart.xpm"
#include "images/options_verticalbarchart.xpm"


#include "debug.hxx"

const QString WINDOWS_REGISTRY = "/Trolltech/QtExamples";
const QString APP_KEY = "/Chart/";

ChartForm::ChartForm(  QWidget* parent, const char* name, WFlags fl) : QWidget( parent, name, fl )
{
#ifndef  MAINWINDOW
const QString filename;
#endif //  MAINWINDOW

   /*! Put all this in a layout so the resize does the right thing...  */
   setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, sizePolicy().hasHeightForWidth() ) );
   QHBoxLayout * localLayout = new QHBoxLayout( this, 0, -1, "ChartForm");

    setIcon( QPixmap( options_piechart ) );

    QAction *fileNewAction;
    QAction *fileOpenAction;
    QAction *fileSaveAction;
    QAction *fileSaveAsAction;
    QAction *fileSaveAsPixmapAction;
    QAction *filePrintAction;
    QAction *fileQuitAction;
    QAction *optionsSetDataAction;
    QAction *optionsSetFontAction;
    QAction *optionsSetOptionsAction;

    fileNewAction = new QAction(
	    "New Chart", QPixmap( file_new ),
	    "&New", CTRL+Key_N, this, "new" );
    connect( fileNewAction, SIGNAL( activated() ), this, SLOT( fileNew() ) );

    fileOpenAction = new QAction(
	    "Open Chart", QPixmap( file_open ),
	    "&Open...", CTRL+Key_O, this, "open" );
    connect( fileOpenAction, SIGNAL( activated() ), this, SLOT( fileOpen() ) );

    fileSaveAction = new QAction(
	    "Save Chart", QPixmap( file_save ),
	    "&Save", CTRL+Key_S, this, "save" );
    connect( fileSaveAction, SIGNAL( activated() ), this, SLOT( fileSave() ) );

    fileSaveAsAction = new QAction(
	    "Save Chart As", QPixmap( file_save ),
	    "Save &As...", 0, this, "save as" );
    connect( fileSaveAsAction, SIGNAL( activated() ),
	     this, SLOT( fileSaveAs() ) );

    fileSaveAsPixmapAction = new QAction(
	    "Save Chart As Bitmap", QPixmap( file_save ),
	    "Save As &Bitmap...", CTRL+Key_B, this, "save as bitmap" );
    connect( fileSaveAsPixmapAction, SIGNAL( activated() ),
	     this, SLOT( fileSaveAsPixmap() ) );

    filePrintAction = new QAction(
	    "Print Chart", QPixmap( file_print ),
	    "&Print Chart...", CTRL+Key_P, this, "print chart" );
    connect( filePrintAction, SIGNAL( activated() ),
	     this, SLOT( filePrint() ) );

    optionsSetDataAction = new QAction(
	    "Set Data", QPixmap( options_setdata ),
	    "Set &Data...", CTRL+Key_D, this, "set data" );
    connect( optionsSetDataAction, SIGNAL( activated() ),
	     this, SLOT( optionsSetData() ) );


    QActionGroup *chartGroup = new QActionGroup( this ); // Connected later
    chartGroup->setExclusive( TRUE );

    optionsPieChartActionWithShadow = new QAction(
	    "Pie Chart with shadow", QPixmap( options_piechart ),
	    "&Pie Chart with shadow", CTRL+Key_I, chartGroup, "pie chart with shadow" );
    optionsPieChartActionWithShadow->setToggleAction( TRUE );

    optionsPieChartActionWithNoShadow = new QAction(
	    "Pie Chart with no shadow", QPixmap( options_piechart ),
	    "&Pie Chart with no shadow", CTRL+Key_I, chartGroup, "pie chart with no shadow" );
    optionsPieChartActionWithNoShadow->setToggleAction( TRUE );

    optionsPieChartActionWith3D = new QAction(
	    "Pie Chart with 3D", QPixmap( options_piechart ),
	    "&Pie Chart with 3D", CTRL+Key_I, chartGroup, "pie chart with 3D" );
    optionsPieChartActionWith3D->setToggleAction( TRUE );

optionsBarChartActionWith3D = new QAction(
	    "Bar Chart with 3D", QPixmap( options_barchart ),
	    "&Bar Chart with 3D", CTRL+Key_I, chartGroup, "bar chart with 3D" );
    optionsBarChartActionWith3D->setToggleAction( TRUE );

    optionsHorizontalBarChartAction = new QAction(
	    "Horizontal Bar Chart", QPixmap( options_horizontalbarchart ),
	    "&Horizontal Bar Chart", CTRL+Key_H, chartGroup,
	    "horizontal bar chart" );
    optionsHorizontalBarChartAction->setToggleAction( TRUE );

    optionsVerticalBarChartAction = new QAction(
	    "Vertical Bar Chart", QPixmap( options_verticalbarchart ),
	    "&Vertical Bar Chart", CTRL+Key_V, chartGroup, "Vertical bar chart" );
    optionsVerticalBarChartAction->setToggleAction( TRUE );


    optionsSetFontAction = new QAction(
	    "Set Font", QPixmap( options_setfont ),
	    "Set &Font...", CTRL+Key_F, this, "set font" );
    connect( optionsSetFontAction, SIGNAL( activated() ),
	     this, SLOT( optionsSetFont() ) );

    optionsSetOptionsAction = new QAction(
	    "Set Options", QPixmap( options_setoptions ),
	    "Set &Options...", 0, this, "set options" );
    connect( optionsSetOptionsAction, SIGNAL( activated() ),
	     this, SLOT( optionsSetOptions() ) );

    fileQuitAction = new QAction( "Quit", "&Quit", CTRL+Key_Q, this, "quit" );
    connect( fileQuitAction, SIGNAL( activated() ), this, SLOT( fileQuit() ) );


#ifdef MAINWINDOW
    QToolBar* fileTools = new QToolBar( this, "file operations" );
    fileTools->setLabel( "File Operations" );
    fileNewAction->addTo( fileTools );
    fileOpenAction->addTo( fileTools );
    fileSaveAction->addTo( fileTools );
    fileTools->addSeparator();
    filePrintAction->addTo( fileTools );

    QToolBar *optionsTools = new QToolBar( this, "options operations" );
    optionsTools->setLabel( "Options Operations" );
    optionsSetDataAction->addTo( optionsTools );
    optionsTools->addSeparator();
    optionsPieChartActionWithShadow->addTo( optionsTools );
optionsPieChartActionWithNoShadow->addTo( optionsTools );
optionsPieChartActionWith3D->addTo( optionsTools );
optionsBarChartActionWith3D->addTo( optionsTools );
    optionsHorizontalBarChartAction->addTo( optionsTools );
    optionsVerticalBarChartAction->addTo( optionsTools );
    optionsTools->addSeparator();
    optionsSetFontAction->addTo( optionsTools );
    optionsTools->addSeparator();
    optionsSetOptionsAction->addTo( optionsTools );

    fileMenu = new QPopupMenu( this );
    menuBar()->insertItem( "&File", fileMenu );
    fileNewAction->addTo( fileMenu );
    fileOpenAction->addTo( fileMenu );
    fileSaveAction->addTo( fileMenu );
    fileSaveAsAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileSaveAsPixmapAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    filePrintAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileQuitAction->addTo( fileMenu );
#endif // MAINWINDOW

    optionsMenu = new QPopupMenu( this );
    optionsSetDataAction->addTo( optionsMenu );
    optionsMenu->insertSeparator();
    optionsPieChartActionWithShadow->addTo( optionsMenu );
optionsPieChartActionWithNoShadow->addTo( optionsMenu );
optionsPieChartActionWith3D->addTo( optionsMenu );
optionsBarChartActionWith3D->addTo( optionsMenu );
    optionsHorizontalBarChartAction->addTo( optionsMenu );
    optionsVerticalBarChartAction->addTo( optionsMenu );
    optionsMenu->insertSeparator();
    optionsSetFontAction->addTo( optionsMenu );
    optionsMenu->insertSeparator();
    optionsSetOptionsAction->addTo( optionsMenu );

#ifdef MAINWINDOW
    menuBar()->insertItem( "&Options", optionsMenu );
    menuBar()->insertSeparator();

    QPopupMenu *helpMenu = new QPopupMenu( this );
    menuBar()->insertItem( "&Help", helpMenu );
    helpMenu->insertItem( "&Help", this, SLOT(helpHelp()), Key_F1 );
    helpMenu->insertItem( "&About", this, SLOT(helpAbout()) );
    helpMenu->insertItem( "About &Qt", this, SLOT(helpAboutQt()) );
#else // MAINWINDOW
    optionsMenu->insertSeparator();
    optionsMenu->insertSeparator();
fileNewAction->addTo( optionsMenu );
fileOpenAction->addTo( optionsMenu );
fileSaveAction->addTo( optionsMenu );
fileSaveAsAction->addTo( optionsMenu );
fileSaveAsPixmapAction->addTo( optionsMenu );
filePrintAction->addTo( optionsMenu );
// fileQuitAction->addTo( optionsMenu );
#endif // MAINWINDOW


    m_printer = 0;
    m_elements.resize( MAX_ELEMENTS );

    QSettings settings;
    settings.insertSearchPath( QSettings::Windows, WINDOWS_REGISTRY );
#ifdef MAINWINDOW
    int windowWidth = settings.readNumEntry( APP_KEY + "WindowWidth", 460 );
    int windowHeight = settings.readNumEntry( APP_KEY + "WindowHeight", 530 );
#else // MAINWINDOW
    int windowWidth = 460;
    int windowHeight = 530;
#endif // MAINWINDOW
    int windowX = settings.readNumEntry( APP_KEY + "WindowX", -1 );
    int windowY = settings.readNumEntry( APP_KEY + "WindowY", -1 );
    setChartType( ChartType(
	    settings.readNumEntry( APP_KEY + "ChartType", int(PIEWITHSHADOW) ) ) );
    m_addValues = AddValuesType(
		    settings.readNumEntry( APP_KEY + "AddValues", int(NO) ));
    m_decimalPlaces = settings.readNumEntry( APP_KEY + "Decimals", 2 );
    m_font = QFont( "Helvetica", 18, QFont::Bold );
    m_font.fromString(
	    settings.readEntry( APP_KEY + "Font", m_font.toString() ) );
    for ( int i = 0; i < MAX_RECENTFILES; ++i ) {
	QString filename = settings.readEntry( APP_KEY + "File" +
					       QString::number( i + 1 ) );
	if ( !filename.isEmpty() )
	    m_recentFiles.push_back( filename );
    }
    if ( m_recentFiles.count() )
	updateRecentFilesMenu();


    // Connect *after* we've set the chart type on so we don't call
    // drawElements() prematurely.
    connect( chartGroup, SIGNAL( selected(QAction*) ),
	     this, SLOT( updateChartType(QAction*) ) );

    resize( windowWidth, windowHeight );
    nprintf(DEBUG_PANELS) ("Try to size things to %d %d\n", windowWidth, windowHeight);
    if ( windowX != -1 || windowY != -1 )
	move( windowX, windowY );

    m_canvas = new QCanvas( this );
    m_canvas->setBackgroundColor(parent->backgroundColor());
#ifdef NORESIZE
    m_canvas->resize( width(), height() );
#endif // NORESIZE
    m_canvasView = new CanvasView( m_canvas, &m_elements, this );
#ifdef MAINWINDOW
    setCentralWidget( m_canvasView );
#else // MAINWINDOW
    localLayout->addWidget(m_canvasView);
#endif // MAINWINDOW
    m_canvasView->show();

    if ( !filename.isEmpty() )
	load( filename );
    else {
	init();
    }

#ifdef MAINWINDOW
    statusBar()->message( "Ready", 2000 );
#endif // MAINWINDOW
}


ChartForm::~ChartForm()
{
    delete m_printer;
}


void ChartForm::init()
{
    setCaption( "Chart" );
    m_filename = QString::null;
    m_changed = FALSE;

    m_elements[0]  = Element( Element::INVALID, red );
    m_elements[1]  = Element( Element::INVALID, cyan );
    m_elements[2]  = Element( Element::INVALID, blue );
    m_elements[3]  = Element( Element::INVALID, yellow );
    m_elements[4]  = Element( Element::INVALID, green );
    m_elements[5]  = Element( Element::INVALID, magenta );
    m_elements[6]  = Element( Element::INVALID, darkYellow );
    m_elements[7]  = Element( Element::INVALID, darkRed );
    m_elements[8]  = Element( Element::INVALID, darkCyan );
    m_elements[9]  = Element( Element::INVALID, darkGreen );
    m_elements[10] = Element( Element::INVALID, darkMagenta );
    m_elements[11] = Element( Element::INVALID, darkBlue );
    for ( int i = 12; i < MAX_ELEMENTS; ++i ) {
	double x = (double(i) / MAX_ELEMENTS) * 360;
	int y = (int(x * 256) % 105) + 151;
	int z = ((i * 17) % 105) + 151;
	m_elements[i] = Element( Element::INVALID, QColor( int(x), y, z, QColor::Hsv ) );
    }
}

void ChartForm::closeEvent( QCloseEvent * )
{
    fileQuit();
}


void ChartForm::fileNew()
{
    if ( okToClear() ) {
	init();
	drawElements();
    }
}


void ChartForm::fileOpen()
{
    if ( !okToClear() )
	return;

    QString filename = QFileDialog::getOpenFileName(
			    QString::null, "Charts (*.cht)", this,
			    "file open", "Chart -- File Open" );
    if ( !filename.isEmpty() )
    {
	  load( filename );
    } else
    {
#ifdef MAINWINDOW
	  statusBar()->message( "File Open abandoned", 2000 );
#endif // MAINWINDOW
    }
}


void ChartForm::fileSaveAs()
{
  QString filename = QFileDialog::getSaveFileName(
                          QString::null, "Charts (*.cht)", this,
                          "file save as", "Chart -- File Save As" );
  if( !filename.isEmpty() )
  {
    int answer = 0;
    if( QFile::exists( filename ) )
    {
      answer = QMessageBox::warning( this, "Chart -- Overwrite File",
                                     QString( "Overwrite\n\'%1\'?" ).
                                     arg( filename ),
                                     "&Yes", "&No", QString::null, 1, 1 );
    }
    if( answer == 0 )
    {
      m_filename = filename;
      updateRecentFiles( filename );
      fileSave();
      return;
    }
  }
#ifdef MAINWINDOW
  statusBar()->message( "Saving abandoned", 2000 );
#endif // MAINWINDOW
}


void ChartForm::fileOpenRecent( int index )
{
    if ( !okToClear() )
	return;

    load( m_recentFiles[index] );
}


void ChartForm::updateRecentFiles( const QString& filename )
{
#ifdef MAINWINDOW
    if ( m_recentFiles.find( filename ) != m_recentFiles.end() )
	return;

    m_recentFiles.push_back( filename );
    if ( m_recentFiles.count() > MAX_RECENTFILES )
	m_recentFiles.pop_front();

    updateRecentFilesMenu();
#endif // MAINWINDOW
}


void ChartForm::updateRecentFilesMenu()
{
#ifdef MAINWINDOW
    if ( m_recentFiles.find( filename ) != m_recentFiles.end() )
    for ( int i = 0; i < MAX_RECENTFILES; ++i ) {
	if ( fileMenu->findItem( i ) )
	    fileMenu->removeItem( i );
	if ( i < int(m_recentFiles.count()) )
	    fileMenu->insertItem( QString( "&%1 %2" ).
				    arg( i + 1 ).arg( m_recentFiles[i] ),
				  this, SLOT( fileOpenRecent(int) ),
				  0, i );
    }
#endif // MAINWINDOW
}


void ChartForm::fileQuit()
{
    if ( okToClear() ) {
	saveOptions();
        qApp->exit( 0 );
    }
}


bool ChartForm::okToClear()
{
    if ( m_changed ) {
	QString msg;
	if ( m_filename.isEmpty() )
	    msg = "Unnamed chart ";
	else
	    msg = QString( "Chart '%1'\n" ).arg( m_filename );
	msg += "has been changed.";

	int x = QMessageBox::information( this, "Chart -- Unsaved Changes",
					  msg, "&Save", "Cancel", "&Abandon",
					  0, 1 );
	switch( x ) {
	    case 0: // Save
		fileSave();
		break;
	    case 1: // Cancel
	    default:
		return FALSE;
	    case 2: // Abandon
		break;
	}
    }

    return TRUE;
}


void ChartForm::saveOptions()
{
    QSettings settings;
    settings.insertSearchPath( QSettings::Windows, WINDOWS_REGISTRY );
    settings.writeEntry( APP_KEY + "WindowWidth", width() );
    settings.writeEntry( APP_KEY + "WindowHeight", height() );
    settings.writeEntry( APP_KEY + "WindowX", x() );
    settings.writeEntry( APP_KEY + "WindowY", y() );
    settings.writeEntry( APP_KEY + "ChartType", int(m_chartType) );
    settings.writeEntry( APP_KEY + "AddValues", int(m_addValues) );
    settings.writeEntry( APP_KEY + "Decimals", m_decimalPlaces );
    settings.writeEntry( APP_KEY + "Font", m_font.toString() );
    for ( int i = 0; i < int(m_recentFiles.count()); ++i )
	settings.writeEntry( APP_KEY + "File" + QString::number( i + 1 ),
			     m_recentFiles[i] );
}


void ChartForm::optionsSetData()
{
    SetDataForm *setDataForm = new SetDataForm( &m_elements, m_decimalPlaces, this );
    if ( setDataForm->exec() ) {
	m_changed = TRUE;
	drawElements();
    }
    delete setDataForm;
}


void ChartForm::setChartType( ChartType chartType )
{
    m_chartType = chartType;
    switch ( m_chartType ) {
	case PIEWITHSHADOW:
	    optionsPieChartActionWithShadow->setOn( TRUE );
	    break;
case PIEWITHNOSHADOW:
  optionsPieChartActionWithNoShadow->setOn( TRUE );
  break;
case PIEWITH3D:
  optionsPieChartActionWith3D->setOn( TRUE );
  break;
case BARWITH3D:
  optionsBarChartActionWith3D->setOn( TRUE );
  break;
	case VERTICAL_BAR:
	    optionsVerticalBarChartAction->setOn( TRUE );
	    break;
	case HORIZONTAL_BAR:
	    optionsHorizontalBarChartAction->setOn( TRUE );
	    break;
    }
}


void ChartForm::updateChartType( QAction *action )
{
    if ( action == optionsPieChartActionWithShadow ) {
	m_chartType = PIEWITHSHADOW;
} else if ( action == optionsPieChartActionWithNoShadow ) {
  m_chartType = PIEWITHNOSHADOW;
} else if ( action == optionsPieChartActionWith3D ) {
  m_chartType = PIEWITH3D;
} else if ( action == optionsBarChartActionWith3D ) {
  m_chartType = BARWITH3D;
}
    else if ( action == optionsHorizontalBarChartAction ) {
	m_chartType = HORIZONTAL_BAR;
    }
    else if ( action == optionsVerticalBarChartAction ) {
	m_chartType = VERTICAL_BAR;
    }

    drawElements();
}


void ChartForm::optionsSetFont()
{
    bool ok;
    QFont font = QFontDialog::getFont( &ok, m_font, this );
    if ( ok ) {
	m_font = font;
	drawElements();
    }
}


void ChartForm::optionsSetOptions()
{
    OptionsForm *optionsForm = new OptionsForm( this );
    optionsForm->chartTypeComboBox->setCurrentItem( m_chartType );
    optionsForm->setFont( m_font );
    switch ( m_addValues ) {
	case NO:
	    optionsForm->noRadioButton->setChecked( TRUE );
	    break;
	case YES:
	    optionsForm->yesRadioButton->setChecked( TRUE );
	    break;
	case AS_PERCENTAGE:
	    optionsForm->asPercentageRadioButton->setChecked( TRUE );
	    break;
    }
    optionsForm->decimalPlacesSpinBox->setValue( m_decimalPlaces );
    if ( optionsForm->exec() ) {
	setChartType( ChartType(
		optionsForm->chartTypeComboBox->currentItem()) );
	m_font = optionsForm->font();
	if ( optionsForm->noRadioButton->isChecked() )
	    m_addValues = NO;
	else if ( optionsForm->yesRadioButton->isChecked() )
	    m_addValues = YES;
	else if ( optionsForm->asPercentageRadioButton->isChecked() )
	    m_addValues = AS_PERCENTAGE;
	m_decimalPlaces = optionsForm->decimalPlacesSpinBox->value();
	drawElements();
    }
    delete optionsForm;
}


void ChartForm::helpHelp()
{
#ifdef MAINWINDOW
    statusBar()->message( "Help is not implemented yet", 2000 );
#endif // MAINWINDOW
}


void ChartForm::helpAbout()
{
    QMessageBox::about( this, "Chart -- About",
			"<center><h1><font color=blue>Chart<font></h1></center>"
			"<p>Chart your data with <i>chart</i>.</p>"
			);
}

QPopupMenu *
ChartForm::createPopupMenu(const QPoint & /*pos */)
{
  printf("ChartForm::createPopupMenu() entered.\n");
  return(optionsMenu);
}

void
ChartForm::contentsContextMenuEvent( QContextMenuEvent *e )
{
  printf("ChartForm::contentsContextMenuEvent() called.\n");
}


void ChartForm::helpAboutQt()
{
    QMessageBox::aboutQt( this, "Chart -- About Qt" );
}

int ChartForm::mouseClicked(int item)
{
  printf("ChartForm::mouseClicked(%d) entered.\n", item);
}

void ChartForm::setValues(int values[], char *color_names[], char *strings[], int n )
{
#ifndef OLDWAY
  for(int i = 0; i < n; i++ )
  {
	m_elements[i].set( values[i],
      QColor(color_names[i]), 1, QString(strings[i]) );
  }
#else // OLDWAY
    int i = 0;
    for(i = 0; i < n; i++ )
    {
	  m_elements[i].set( values[i],
        QColor(color_names[i]), 1, QString(strings[i]) );
    }
    for(; i < MAX_ELEMENTS; i++ )
    {
      char *color_name = color_names[4];
  
      color_name = "sky blue";
      if( i == MAX_ELEMENTS * .25  )
      {
        m_elements[i].set( 30,
            QColor("red"), 1, QString(strings[0]) );
      } else if( i == MAX_ELEMENTS * .50 )
      {
        m_elements[i].set( 20,
            QColor("purple"), 1, QString(strings[0]) );
      } else
      {
        m_elements[i].set( 1,
          QColor(color_name), 1, QString(strings[0]) );
      }
    }
#endif // OLDWAY
  drawElements();
}
