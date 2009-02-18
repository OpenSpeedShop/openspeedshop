////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
  

#include "SelectTimeSegmentDialog.hxx"

#include "debug.hxx"

#include <qvariant.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qslider.h>
#include <qsplitter.h>
#include <qtooltip.h>
#include <qapplication.h>

#include "SS_Input_Manager.hxx"

#include "chartform.hxx"
SelectTimeSegmentDialog::SelectTimeSegmentDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("SelectTimeSegmentDialog::SelectTimeSegmentDialog() constructor called.\n");

  mw = (OpenSpeedshop *)parent;
  cli = mw->cli;

  
  if ( !name ) setName( "SelectTimeSegmentDialog" );

  setSizeGripEnabled( TRUE );
  selectTimeSegmentDialogLayout = new QVBoxLayout( this, 11, 6, "selectTimeSegmentDialogLayout"); 

  headerLabel = new QLabel(this, "headerLabel");
  selectTimeSegmentDialogLayout->addWidget( headerLabel );


  splitter = new QSplitter( this, "timeSegmentSplitter");
  splitter->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, 0, 0, FALSE ) );
  splitter->setCaption("TimeSegmentSplitter");
  splitter->setOrientation(QSplitter::Vertical);
  selectTimeSegmentDialogLayout->addWidget(splitter);

  // Insert your skyline layout here.
#ifdef OLDWAY
  QWidget *topWidget = new QWidget( splitter );
  skylineLayout = new QVBoxLayout( topWidget );

QLabel *tempLabel = new QLabel(topWidget, "tempLabel");
tempLabel->setText("Sky line");
skylineLayout->addWidget(tempLabel);

QLabel *tempLabel2 = new QLabel(topWidget, "tempLabel");
tempLabel2->setText("Sky line2");
skylineLayout->addWidget(tempLabel2);
#else // OLDWAY
cf = new ChartForm(splitter, "skylineChartForm", 0);
cf->setChartType(VERTICAL_BAR);
cf->show();
#endif // OLDWAY


  // Insert your timesegment stuff here!
  QWidget *bottomWidget = new QWidget( splitter );
  bottomWidget->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, 0, 0, FALSE ) );
  sliderLayout = new QVBoxLayout( bottomWidget );

  int min = 0;
  int max = 100;
  int pageInterval = 10;
  startSlider = new QSlider(min, max, pageInterval, 0, Qt::Horizontal, bottomWidget, "startSlider");
startSlider->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, 0, 0, FALSE ) );
  startSlider->setTickmarks(QSlider::Below);
  startSlider->setTickInterval(max/pageInterval);
  connect( startSlider, SIGNAL( sliderMoved(int) ), this, SLOT( startSliderMoved(int) ) );
  sliderLayout->addWidget( startSlider );

  endSlider = new QSlider(min, max, pageInterval, 100, Qt::Horizontal, bottomWidget, "endSlider");
endSlider->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, 0, 0, FALSE ) );
  connect( endSlider, SIGNAL( sliderMoved(int) ), this, SLOT( endSliderMoved(int) ) );
  endSlider->setTickInterval(max/pageInterval);

  sliderLayout->addWidget( endSlider );

//  selectTimeSegmentDialogLayout->addLayout( sliderLayout );

  startStopLayout = new QHBoxLayout( 0, 0, 6, "startStopLayout"); 
  startLabel = new QLabel(bottomWidget, "startLabel");
startLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  startStopLayout->addWidget( startLabel );
  startValue = new QLineEdit(bottomWidget, "startValue");
startValue->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  startStopLayout->addWidget( startValue );

  endLabel = new QLabel(bottomWidget, "endLabel");
endLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  startStopLayout->addWidget( endLabel );
  endValue = new QLineEdit(bottomWidget, "endValue");
endValue->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  startStopLayout->addWidget( endValue );

  sliderLayout->addLayout( startStopLayout );

  buttonLayout = new QHBoxLayout( 0, 0, 6, "buttonLayout"); 

#if 0
  buttonHelp = new QPushButton( this, "buttonHelp" );
  buttonHelp->setAutoDefault( TRUE );
  buttonLayout->addWidget( buttonHelp );
  Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  buttonLayout->addItem( Horizontal_Spacing2 );
#endif // 0

  buttonDefaults = new QPushButton( this, "buttonDefaults" );
  buttonLayout->addWidget( buttonDefaults );
  Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  buttonLayout->addItem( Horizontal_Spacing2 );

  buttonOk = new QPushButton( this, "buttonOk" );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setDefault( TRUE );
  buttonLayout->addWidget( buttonOk );

  buttonCancel = new QPushButton( this, "buttonCancel" );
  buttonCancel->setAutoDefault( TRUE );
  buttonLayout->addWidget( buttonCancel );
  selectTimeSegmentDialogLayout->addLayout( buttonLayout );
  languageChange();
//  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( ok_accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( cancel_reject() ) );
  connect( buttonDefaults, SIGNAL( clicked() ), this, SLOT( buttonDefaultsSelected() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
SelectTimeSegmentDialog::~SelectTimeSegmentDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("SelectTimeSegmentDialog::SelectTimeSegmentDialog() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SelectTimeSegmentDialog::languageChange()
{
  setCaption( tr( name() ) );
  headerLabel->setText( tr( "Select the time segment:" ) );
  startLabel->setText( tr( "Start Value:" ) );
  endLabel->setText( tr( "End Value:" ) );
  startValue->setText( tr( "0") );
  endValue->setText( tr( "100" ) );
#if 0
  buttonHelp->setText( tr( "&Help" ) );
  buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
#endif // 0
  buttonOk->setText( tr( "&OK" ) );
  buttonOk->setAccel( QKeySequence( QString::null ) );
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );
  buttonDefaults->setText( tr( "&Defaults" ) );
  buttonDefaults->setAccel( QKeySequence( QString::null ) );
}

void
SelectTimeSegmentDialog::buttonDefaultsSelected()
{
  languageChange();
}

void
SelectTimeSegmentDialog::startSliderMoved(int val)
{
// printf("startSliderMoved(%d)\n", val);
  startValue->setText( tr( QString("%1").arg(val) ) );
}

void
SelectTimeSegmentDialog::endSliderMoved(int val)
{
// printf("endSliderMoved(%d)\n", val);
  endValue->setText( tr( QString("%1").arg(val) ) );
}


void SelectTimeSegmentDialog::ok_accept()
{
// printf("SelectTimeSegmentDialog::ok_accept() called.\n");
  QDialog::accept();
}


void SelectTimeSegmentDialog::cancel_reject()
{
// printf("SelectTimeSegmentDialog::cancel_reject() called.\n");
  QDialog::reject();
}

