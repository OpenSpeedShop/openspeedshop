////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.
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


#include <stddef.h>
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
#include <qlabel.h>

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
  const QString filename;

   /*! Put all this in a layout so the resize does the right thing...  */
   setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, sizePolicy().hasHeightForWidth() ) );
   QVBoxLayout * localLayout = new QVBoxLayout( this, 0, -1, "ChartForm");

   headerLabel = new QLabel(this, "headerLabel");
   localLayout->addWidget(headerLabel);

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

#ifdef OLDWAY
    optionsSetDataAction = new QAction(
	    "Set Data", QPixmap( options_setdata ),
	    "Set &Data...", CTRL+Key_D, this, "set data" );
    connect( optionsSetDataAction, SIGNAL( activated() ),
	     this, SLOT( optionsSetData() ) );
#endif // OLDWAY


    QActionGroup *chartGroup = new QActionGroup( this ); // Connected later
    chartGroup->setExclusive( TRUE );


    optionsPieChartActionWithShadow = new QAction(
	    "Pie Chart", QPixmap( options_piechart ),
	    "&Pie Chart", CTRL+Key_I, chartGroup, "pie chart with shadow" );
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


    optionsMenu = new QPopupMenu( this );
#ifdef OLDWAY
    optionsSetDataAction->addTo( optionsMenu );
    optionsMenu->insertSeparator();
#endif // OLDWAY
    optionsPieChartActionWithShadow->addTo( optionsMenu );
#ifdef OLDWAY
    optionsPieChartActionWithNoShadow->addTo( optionsMenu );
    optionsPieChartActionWith3D->addTo( optionsMenu );
    optionsBarChartActionWith3D->addTo( optionsMenu );
#endif // OLDWAY
    optionsHorizontalBarChartAction->addTo( optionsMenu );
    optionsVerticalBarChartAction->addTo( optionsMenu );
#ifdef OLDWAY
    optionsMenu->insertSeparator();
    optionsSetFontAction->addTo( optionsMenu );
    optionsMenu->insertSeparator();
    optionsSetOptionsAction->addTo( optionsMenu );

    optionsMenu->insertSeparator();
    optionsMenu->insertSeparator();
    fileNewAction->addTo( optionsMenu );
    fileOpenAction->addTo( optionsMenu );
    fileSaveAction->addTo( optionsMenu );
    fileSaveAsAction->addTo( optionsMenu );
    fileSaveAsPixmapAction->addTo( optionsMenu );
    filePrintAction->addTo( optionsMenu );
// fileQuitAction->addTo( optionsMenu );
#endif // OLDWAY


    m_printer = 0;
    m_elements.resize( MAX_ELEMENTS );

    QSettings settings;
    settings.insertSearchPath( QSettings::Windows, WINDOWS_REGISTRY );
    int windowWidth = 460;
    int windowHeight = 530;
    int windowX = settings.readNumEntry( APP_KEY + "WindowX", -1 );
    int windowY = settings.readNumEntry( APP_KEY + "WindowY", -1 );
    setChartType( ChartType(
	    settings.readNumEntry( APP_KEY + "ChartType", int(PIEWITHNOSHADOW) ) ) );
    m_addValues = AddValuesType(
		    settings.readNumEntry( APP_KEY + "AddValues", int(NO) ));
    m_decimalPlaces = settings.readNumEntry( APP_KEY + "Decimals", 2 );
#ifdef OLDWAY
    m_font = QFont( "Bitstream Charter", 18, QFont::Bold );
    m_font.fromString(
	    settings.readEntry( APP_KEY + "Font", m_font.toString() ) );
#else // OLDWAY
    m_font = qApp->font();
#endif  // OLDWAY
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
    m_canvasView = new CanvasView( m_canvas, &m_elements, this );
    localLayout->addWidget(m_canvasView);
    m_canvasView->show();

    if ( !filename.isEmpty() )
	load( filename );
    else {
	init();
    }
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
}


void ChartForm::fileOpenRecent( int index )
{
    if ( !okToClear() )
	return;

    load( m_recentFiles[index] );
}


void ChartForm::updateRecentFiles( const QString& filename )
{
}


void ChartForm::updateRecentFilesMenu()
{
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
#ifdef OLDWAY
case PIEWITH3D:
  optionsPieChartActionWith3D->setOn( TRUE );
  break;
case BARWITH3D:
  optionsBarChartActionWith3D->setOn( TRUE );
  break;
#endif // OLDWAY
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
// printf("ChartForm::createPopupMenu() entered.\n");
  return(optionsMenu);
}

void
ChartForm::contentsContextMenuEvent( QContextMenuEvent *e )
{
// printf("ChartForm::contentsContextMenuEvent() called.\n");
}


void ChartForm::helpAboutQt()
{
    QMessageBox::aboutQt( this, "Chart -- About Qt" );
}

int ChartForm::mouseClicked(int item)
{
// printf("ChartForm::mouseClicked(%d) entered.\n", item);
}


void ChartForm::setValues(ChartPercentValueList values, ChartTextValueList strings, const char **color_names, int max_color_cnt)
{
int i = 0;
for ( i = 0; i < MAX_ELEMENTS; ++i )
{
  m_elements[i].setValue(EPSILON);
}

i =0;

for( ChartPercentValueList::iterator vi = values.begin();vi != values.end(); vi++)
{
  ChartTextValueList::iterator ti = strings.begin();
  int value = (int)*vi;
  ti += i;
  QString value_str = (QString)*ti;
// printf("ChartForm::setValues(%d)=(%s) (%d)\n", i, value_str.ascii(), value);
  int color_index = i;
  if( i>15 ) color_index = max_color_cnt-1;
//  if( value_str.isEmpty() )
  if( value == Element::INVALID )
  {
    break;
  }
// printf("ChartForm::setValues[%d] (%d) %s\n", i, value, value_str.ascii() );
  m_elements[i].set( value,
      QColor(color_names[color_index]), 1, value_str );
  i++;
}
// printf("i=%d \n", i);

  drawElements();
}

void
ChartForm::setHeader( QString header )
{
  headerLabel->setText(header);
}

