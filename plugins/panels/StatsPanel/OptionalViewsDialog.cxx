////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014  Krell Institute  All Rights Reserved.
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
#include <qsettings.h>
#include <qapplication.h>
#include <qvalidator.h>
#include <qmessagebox.h>

#include "openspeedshop.hxx"
#include "PluginInfo.hxx"
#include <assert.h>

#include "PreferencesChangedObject.hxx"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qframe.h>
#include <qheader.h>
#include <qlistview.h>
#include <qwidgetstack.h>
#include <qwidget.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qvalidator.h>
#include <fstream>
#include <iostream>
//#include "preferencesdialog.ui.hxx"
#include "OptionalViewsDialog.hxx"
#include "StatsPanel.hxx"

// enable the viewing of pathnames for iot experiments
#define PATHNAME_READY 1

//#define DEBUG_optional 1


/*
 *  Constructs a OptionalViewsDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
OptionalViewsDialog::OptionalViewsDialog( QWidget* parent, 
                                          const char* name, 
                                          QString collectorString, 
                                          std::list<std::string> *current_modifiers, 
                                          bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
   panelContainer = (PanelContainer *)parent;
   globalCurrentModifiers = current_modifiers;

   if( globalCollectorString.contains("pcsamp") ) {
     pcsamp_percent = FALSE;
     pcsamp_time = FALSE;
     pcsamp_ThreadAverage = FALSE;
     pcsamp_ThreadMin = FALSE;
     pcsamp_ThreadMax = FALSE;
   } else if ( globalCollectorString.contains("usertime") ) {
     usertime_exclusive_times = FALSE;
     usertime_inclusive_times = FALSE;
     usertime_percent = FALSE;
     usertime_count = FALSE;
     usertime_ThreadAverage = FALSE;
     usertime_ThreadMin = FALSE;
     usertime_ThreadMax = FALSE;
   } else if ( globalCollectorString.contains("omptp") ) {
     omptp_exclusive_times = FALSE;
     omptp_inclusive_times = FALSE;
     omptp_percent = FALSE;
     omptp_count = FALSE;
     omptp_ThreadAverage = FALSE;
     omptp_ThreadMin = FALSE;
     omptp_ThreadMax = FALSE;
   } else if ( globalCollectorString.contains("hwcsamp") ) {
     hwcsamp_time = FALSE;
     hwcsamp_allEvents = FALSE;
     hwcsamp_percent = FALSE;
     hwcsamp_ThreadAverage = FALSE;
     hwcsamp_ThreadMin = FALSE;
     hwcsamp_ThreadMax = FALSE;
   } else if ( globalCollectorString.contains("hwctime") ) {
     hwctime_exclusive_counts = FALSE;
     hwctime_exclusive_overflows = FALSE;
     hwctime_inclusive_overflows = FALSE;
     hwctime_inclusive_counts = FALSE;
     hwctime_percent = FALSE;
     hwctime_ThreadAverage = FALSE;
     hwctime_ThreadMin = FALSE;
     hwctime_ThreadMax = FALSE;
   } else if ( globalCollectorString.contains("hwc") ) {
     hwc_overflows = FALSE;
     hwc_counts = FALSE;
     hwc_percent = FALSE;
     hwc_ThreadAverage = FALSE;
     hwc_ThreadMin = FALSE;
     hwc_ThreadMax = FALSE;
   } else if ( globalCollectorString.contains("iot") ) {
     iot_exclusive_times = FALSE;
     iot_inclusive_times = FALSE;
     iot_min = FALSE;
     iot_max = FALSE;
     iot_average = FALSE;
     iot_count = FALSE;
     iot_percent = FALSE;
     iot_stddev = FALSE;
     iot_start_time = FALSE;
     iot_stop_time = FALSE;
     iot_syscallno = FALSE;
     iot_nsysargs = FALSE;
     iot_retval = FALSE;
#if PATHNAME_READY
     iot_pathname = FALSE;
#endif
   } else if (globalCollectorString.contains("io") ) {
     io_exclusive_times = FALSE;
     io_min = FALSE;
     io_max = FALSE;
     io_average = FALSE;
     io_count = FALSE;
     io_percent = FALSE;
     io_stddev = FALSE;
     io_ThreadAverage = FALSE;
     io_ThreadMin = FALSE;
     io_ThreadMax = FALSE;
   } else if ( globalCollectorString.contains("iop") ) {
     iop_exclusive_times = FALSE;
     iop_inclusive_times = FALSE;
     iop_percent = FALSE;
     iop_count = FALSE;
     iop_ThreadAverage = FALSE;
     iop_ThreadMin = FALSE;
     iop_ThreadMax = FALSE;
   } else if (globalCollectorString.contains("mem") ) {
     mem_exclusive_times = FALSE;
     mem_min = FALSE;
     mem_max = FALSE;
     mem_average = FALSE;
     mem_count = FALSE;
     mem_percent = FALSE;
     mem_stddev = FALSE;
     mem_ThreadAverage = FALSE;
     mem_ThreadMin = FALSE;
     mem_ThreadMax = FALSE;
   } else if ( globalCollectorString.contains("mpip") ) {
     mpip_exclusive_times = FALSE;
     mpip_inclusive_times = FALSE;
     mpip_percent = FALSE;
     mpip_count = FALSE;
     mpip_ThreadAverage = FALSE;
     mpip_ThreadMin = FALSE;
     mpip_ThreadMax = FALSE;
   } else if ( globalCollectorString.contains("mpit") ) {
     mpit_exclusive_times = FALSE;
     mpit_inclusive_times = FALSE;
     mpit_min = FALSE;
     mpit_max = FALSE;
     mpit_average = FALSE;
     mpit_count = FALSE;
     mpit_percent = FALSE;
     mpit_stddev = FALSE;
     mpit_size = FALSE;
     mpit_start_time = FALSE;
     mpit_stop_time = FALSE;
     mpit_source = FALSE;
     mpit_dest = FALSE;
     mpit_tag = FALSE;
     mpit_communicator = FALSE;
     mpit_datatype = FALSE;
     mpit_retval = FALSE;
   } else if ( globalCollectorString.contains("mpi") ) {
     mpi_exclusive_times = FALSE;
     mpi_inclusive_times = FALSE;
     mpi_min = FALSE;
     mpi_max = FALSE;
     mpi_average = FALSE;
     mpi_count = FALSE;
     mpi_percent = FALSE;
     mpi_stddev = FALSE;
   } else if (globalCollectorString.contains("pthreads") ) {
     pthreads_exclusive_times = FALSE;
     pthreads_min = FALSE;
     pthreads_max = FALSE;
     pthreads_average = FALSE;
     pthreads_count = FALSE;
     pthreads_percent = FALSE;
     pthreads_stddev = FALSE;
     pthreads_ThreadAverage = FALSE;
     pthreads_ThreadMin = FALSE;
     pthreads_ThreadMax = FALSE;
   } else if ( globalCollectorString.contains("fpe") ) {
#if 0
     fpe_time = FALSE;
#endif
     fpe_counts = FALSE;
     fpe_inclusive_counts = FALSE;
     fpe_percent = FALSE;
     fpe_ThreadAverage = FALSE;
     fpe_ThreadMin = FALSE;
     fpe_ThreadMax = FALSE;
     fpe_inexact_result_count = FALSE;
     fpe_underflow_count = FALSE;
     fpe_overflow_count = FALSE;
     fpe_division_by_zero_count = FALSE;
     fpe_unnormal_count = FALSE;
     fpe_invalid_count = FALSE;
     fpe_unknown_count = FALSE;
   } else {
   }


#ifdef DEBUG_optional
   std::cerr << " OptionalViewsDialog::OptionalViewsDialog" << " name="  << name << " collectorString=" << collectorString << std::endl;
#endif

    if ( !name )
	setName( "OptionalViewsDialog" );

    globalCollectorString = collectorString;

    setSizeGripEnabled( TRUE );

    OptionalViewsDialogLayout = new QVBoxLayout( this, 11, 6, "OptionalViewsDialogLayout"); 

    OptionalViewsDialogWidgetStackLayout = new QHBoxLayout( 0, 0, 6, "OptionalViewsDialogWidgetStackLayout"); 

    preferenceDialogListLayout = new QVBoxLayout( 0, 0, 6, "preferenceDialogListLayout");

    mainSplitter = new QSplitter( this, "mainSplitter" );
    mainSplitter->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)5, 0, 0, mainSplitter->sizePolicy().hasHeightForWidth() ) );
    mainSplitter->setOrientation( QSplitter::Horizontal );
    mainSplitter->setChildrenCollapsible( FALSE );

    preferenceDialogLeftFrame = new QFrame( mainSplitter, "preferenceDialogLeftFrame" );
    preferenceDialogLeftFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)5, 0, 0, preferenceDialogLeftFrame->sizePolicy().hasHeightForWidth() ) );
    preferenceDialogLeftFrame->setMinimumSize( QSize( 15, 0 ) );
    preferenceDialogLeftFrame->setFrameShape( QFrame::StyledPanel );
    preferenceDialogLeftFrame->setFrameShadow( QFrame::Raised );
    preferenceDialogLeftFrameLayout = new QVBoxLayout( preferenceDialogLeftFrame, 11, 6, "preferenceDialogLeftFrameLayout");

    categoryListView = new QListView( preferenceDialogLeftFrame, "categoryListView" );
    categoryListView->addColumn( tr( "Categories" ) );
    // Don't sort this list... Add the General first and the rest as they
    //     // come.   If you're going to sort, then sort ascending, alphabetic, but
    //         // leave the "General" category at the top of the list.
    categoryListView->setSortColumn( -1 );
    //                 // There's only one column header, hide it unless its shows up as a problem
    //                     // during usability studies.
    categoryListView->header()->hide();
    //
    categoryListView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)7, 0, 0, categoryListView->sizePolicy().hasHeightForWidth() ) );
    preferenceDialogLeftFrameLayout->addWidget( categoryListView );

    preferenceDialogLeftFrame->hide();

    preferenceDialogRightFrame = new QFrame( mainSplitter, "preferenceDialogRightFrame" );

    preferenceDialogRightFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, preferenceDialogRightFrame->sizePolicy().hasHeightForWidth() ) );
    preferenceDialogRightFrame->setFrameShape( QFrame::StyledPanel );
    preferenceDialogRightFrame->setFrameShadow( QFrame::Raised );
    preferenceDialogRightFrameLayout = new QVBoxLayout( preferenceDialogRightFrame, 11, 6, "preferenceDialogRightFrameLayout");

    preferenceDialogWidgetStack = new QWidgetStack( preferenceDialogRightFrame, "preferenceDialogWidgetStack" );
    preferenceDialogWidgetStack->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, preferenceDialogWidgetStack->sizePolicy().hasHeightForWidth() ) );

    createExperimentDependentOptionalView(preferenceDialogWidgetStack, name );

    preferenceDialogRightFrameLayout->addWidget( preferenceDialogWidgetStack );
    preferenceDialogListLayout->addWidget( mainSplitter );

    OptionalViewsDialogWidgetStackLayout = new QHBoxLayout( 0, 0, 6, "OptionalViewsDialogWidgetStackLayout"); 
    

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setAutoDefault( TRUE );
    OptionalViewsDialogWidgetStackLayout->addWidget( buttonHelp );
    Horizontal_Spacing2 = new QSpacerItem( 120, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    OptionalViewsDialogWidgetStackLayout->addItem( Horizontal_Spacing2 );

    buttonDefaults = new QPushButton( this, "buttonDefaults" );
    OptionalViewsDialogWidgetStackLayout->addWidget( buttonDefaults );

    buttonApply = new QPushButton( this, "buttonApply" );
    buttonApply->setAutoDefault( TRUE );
    buttonApply->setDefault( TRUE );
    OptionalViewsDialogWidgetStackLayout->addWidget( buttonApply );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    OptionalViewsDialogWidgetStackLayout->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    OptionalViewsDialogWidgetStackLayout->addWidget( buttonCancel );
    preferenceDialogListLayout->addLayout( OptionalViewsDialogWidgetStackLayout );
    OptionalViewsDialogLayout->addLayout( preferenceDialogListLayout );

    languageChange();

    resize( QSize(620, 390).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonDefaults, SIGNAL( clicked() ), this,
      SLOT( resetPreferenceDefaults() ) );
    connect( buttonApply, SIGNAL( clicked() ), this,
      SLOT( buttonApplySelected() ) );
    connect( buttonOk, SIGNAL( clicked() ), this,
      SLOT( buttonOkSelected() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this,
      SLOT( reject() ) );

    preferencesAvailable = FALSE;

    settings = new QSettings();
// printf("settings initailize to 0x%x\n", settings );
//    settings->insertSearchPath( QSettings::Unix, "openspeedshop" );

    readPreferencesOnEntry();
    // Set the values to the defaults upon 
    resetPreferenceDefaults();
}

/*
 *  Destroys the object and frees any allocated resources
 */
OptionalViewsDialog::~OptionalViewsDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

bool OptionalViewsDialog::isInCurrentModifierList(std::string modifier) 
{

#ifdef DEBUG_optional
    printf("OptionalViewsDialog::isInCurrentModifierList, modifier to be checked is=%s\n", modifier.c_str());
#endif
    for( std::list<std::string>::const_iterator it = globalCurrentModifiers->begin();
         it != globalCurrentModifiers->end(); it++ )
    {
      std::string cModifier = *it;
      if (modifier == cModifier) {
#ifdef DEBUG_optional
        printf("OptionalViewsDialog::isInCurrentModifierList, result is TRUE\n");
#endif
        return TRUE;
      }
      ++it;
    }
#ifdef DEBUG_optional
    printf("OptionalViewsDialog::isInCurrentModifierList, result is FALSE\n");
#endif
    return FALSE;
}


void
OptionalViewsDialog::createExperimentDependentOptionalView(QWidgetStack* stack, const char* name)
{
    generalStackPage = new QWidget( stack, name );
    generalStackPageLayout = new QVBoxLayout( generalStackPage, 11, 6, "generalStackPageLayout"); 

    GeneralGroupBox = new QGroupBox( generalStackPage, "GeneralGroupBox" );
    GeneralGroupBox->setColumnLayout(0, Qt::Vertical );
    GeneralGroupBox->layout()->setSpacing( 6 );
    GeneralGroupBox->layout()->setMargin( 11 );

    rightSideLayout = new QVBoxLayout( GeneralGroupBox->layout(), 11, "rightSideLayout"); 

    VTraceGroupBox = new QGroupBox( generalStackPage, "VTraceGroupBox" );
    VTraceGroupBox->setColumnLayout(0, Qt::Vertical );
    VTraceGroupBox->layout()->setSpacing( 6 );
    VTraceGroupBox->layout()->setMargin( 11 );

    rightSideVTraceLayout = new QVBoxLayout( VTraceGroupBox->layout(), 11, "rightSideVTraceLayout"); 

  if( globalCollectorString.contains("pcsamp") ) {

    VTraceGroupBox->hide();
    if (isInCurrentModifierList("pcsamp::time")) {
       pcsamp_time = TRUE;
    } else {
       pcsamp_time = FALSE;
    }

    { // pcsamp_time
    pcsamp_time_CheckBox = new QCheckBox( GeneralGroupBox, "pcsamp_time_CheckBox" );
    pcsamp_time_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pcsamp_time_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pcsamp_time_CheckBox->setChecked( pcsamp_time );
    pcsamp_time_CheckBox->setText( tr( "PCSAMP Time" ) );
    rightSideLayout->addWidget( pcsamp_time_CheckBox );
    }

    if (isInCurrentModifierList("pcsamp::percent")) {
       pcsamp_percent = TRUE;
    } else {
       pcsamp_percent = FALSE;
    }

    { // pcsamp_percent
    pcsamp_percent_CheckBox = new QCheckBox( GeneralGroupBox, "pcsamp_percent_CheckBox" );
    pcsamp_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pcsamp_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pcsamp_percent_CheckBox->setChecked( TRUE );
    pcsamp_percent_CheckBox->setText( tr( "PCSAMP Percent" ) );
    rightSideLayout->addWidget( pcsamp_percent_CheckBox );
    }

    if (isInCurrentModifierList("pcsamp::ThreadAverage")) {
       pcsamp_ThreadAverage = TRUE;
    } else {
       pcsamp_ThreadAverage = FALSE;
    }

    { // pcsamp_ThreadAverage
    pcsamp_ThreadAverage_CheckBox = new QCheckBox( GeneralGroupBox, "pcsamp_ThreadAverage_CheckBox" );
    pcsamp_ThreadAverage_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pcsamp_ThreadAverage_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pcsamp_ThreadAverage_CheckBox->setChecked( TRUE );
    pcsamp_ThreadAverage_CheckBox->setText( tr( "PCSAMP ThreadAverage" ) );
    rightSideLayout->addWidget( pcsamp_ThreadAverage_CheckBox );
    }

    if (isInCurrentModifierList("pcsamp::ThreadMin")) {
       pcsamp_ThreadMin = TRUE;
    } else {
       pcsamp_ThreadMin = FALSE;
    }

    { // pcsamp_ThreadMin
    pcsamp_ThreadMin_CheckBox = new QCheckBox( GeneralGroupBox, "pcsamp_ThreadMin_CheckBox" );
    pcsamp_ThreadMin_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pcsamp_ThreadMin_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pcsamp_ThreadMin_CheckBox->setChecked( TRUE );
    pcsamp_ThreadMin_CheckBox->setText( tr( "PCSAMP ThreadMin" ) );
    rightSideLayout->addWidget( pcsamp_ThreadMin_CheckBox );
    }

    if (isInCurrentModifierList("pcsamp::ThreadMax")) {
       pcsamp_ThreadMax = TRUE;
    } else {
       pcsamp_ThreadMax = FALSE;
    }

    { // pcsamp_ThreadMax
    pcsamp_ThreadMax_CheckBox = new QCheckBox( GeneralGroupBox, "pcsamp_ThreadMax_CheckBox" );
    pcsamp_ThreadMax_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pcsamp_ThreadMax_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pcsamp_ThreadMax_CheckBox->setChecked( TRUE );
    pcsamp_ThreadMax_CheckBox->setText( tr( "PCSAMP ThreadMax" ) );
    rightSideLayout->addWidget( pcsamp_ThreadMax_CheckBox );
    }

  } else if ( globalCollectorString.contains("usertime") ) {

    VTraceGroupBox->hide();

    if (isInCurrentModifierList("usertime::exclusive_times")) {
       usertime_exclusive_times = TRUE;
    } else {
       usertime_exclusive_times = FALSE;
    }

    { // usertime_exclusive_times
    usertime_exclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "usertime_exclusive_times_CheckBox" );
    usertime_exclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, usertime_exclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    usertime_exclusive_times_CheckBox->setChecked( TRUE );
    usertime_exclusive_times_CheckBox->setText( tr( "USERTIME exclusive_times" ) );
    rightSideLayout->addWidget( usertime_exclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("usertime::inclusive_times")) {
       usertime_inclusive_times = TRUE;
    } else {
       usertime_inclusive_times = FALSE;
    }

    { // usertime_inclusive_times
    usertime_inclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "usertime_inclusive_times_CheckBox" );
    usertime_inclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, usertime_inclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    usertime_inclusive_times_CheckBox->setChecked( TRUE );
    usertime_inclusive_times_CheckBox->setText( tr( "USERTIME inclusive_times" ) );
    rightSideLayout->addWidget( usertime_inclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("usertime::percent")) {
       usertime_percent = TRUE;
    } else {
       usertime_percent = FALSE;
    }

    { // usertime_percent
    usertime_percent_CheckBox = new QCheckBox( GeneralGroupBox, "usertime_percent_CheckBox" );
    usertime_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, usertime_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    usertime_percent_CheckBox->setChecked( TRUE );
    usertime_percent_CheckBox->setText( tr( "USERTIME percent" ) );
    rightSideLayout->addWidget( usertime_percent_CheckBox );
    }

    if (isInCurrentModifierList("usertime::count")) {
       usertime_count = TRUE;
    } else {
       usertime_count = FALSE;
    }

    { // usertime_count
    usertime_count_CheckBox = new QCheckBox( GeneralGroupBox, "usertime_count_CheckBox" );
    usertime_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, usertime_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    usertime_count_CheckBox->setChecked( TRUE );
    usertime_count_CheckBox->setText( tr( "USERTIME count" ) );
    rightSideLayout->addWidget( usertime_count_CheckBox );
    }

    if (isInCurrentModifierList("usertime::ThreadAverage")) {
       usertime_ThreadAverage = TRUE;
    } else {
       usertime_ThreadAverage = FALSE;
    }

    { // usertime_ThreadAverage
    usertime_ThreadAverage_CheckBox = new QCheckBox( GeneralGroupBox, "usertime_ThreadAverage_CheckBox" );
    usertime_ThreadAverage_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, usertime_ThreadAverage_CheckBox->sizePolicy().hasHeightForWidth() ) );
    usertime_ThreadAverage_CheckBox->setChecked( TRUE );
    usertime_ThreadAverage_CheckBox->setText( tr( "USERTIME ThreadAverage" ) );
    rightSideLayout->addWidget( usertime_ThreadAverage_CheckBox );
    }

    if (isInCurrentModifierList("usertime::ThreadMin")) {
       usertime_ThreadMin = TRUE;
    } else {
       usertime_ThreadMin = FALSE;
    }

    { // usertime_ThreadMin
    usertime_ThreadMin_CheckBox = new QCheckBox( GeneralGroupBox, "usertime_ThreadMin_CheckBox" );
    usertime_ThreadMin_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, usertime_ThreadMin_CheckBox->sizePolicy().hasHeightForWidth() ) );
    usertime_ThreadMin_CheckBox->setChecked( TRUE );
    usertime_ThreadMin_CheckBox->setText( tr( "USERTIME ThreadMin" ) );
    rightSideLayout->addWidget( usertime_ThreadMin_CheckBox );
    }

    if (isInCurrentModifierList("usertime::ThreadMax")) {
       usertime_ThreadMax = TRUE;
    } else {
       usertime_ThreadMax = FALSE;
    }

    { // usertime_ThreadMax
    usertime_ThreadMax_CheckBox = new QCheckBox( GeneralGroupBox, "usertime_ThreadMax_CheckBox" );
    usertime_ThreadMax_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, usertime_ThreadMax_CheckBox->sizePolicy().hasHeightForWidth() ) );
    usertime_ThreadMax_CheckBox->setChecked( TRUE );
    usertime_ThreadMax_CheckBox->setText( tr( "USERTIME ThreadMax" ) );
    rightSideLayout->addWidget( usertime_ThreadMax_CheckBox );
    }
  } else if ( globalCollectorString.contains("omptp") ) {

    VTraceGroupBox->hide();

    if (isInCurrentModifierList("omptp::exclusive_times")) {
       omptp_exclusive_times = TRUE;
    } else {
       omptp_exclusive_times = FALSE;
    }

    { // omptp_exclusive_times
    omptp_exclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "omptp_exclusive_times_CheckBox" );
    omptp_exclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, omptp_exclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    omptp_exclusive_times_CheckBox->setChecked( TRUE );
    omptp_exclusive_times_CheckBox->setText( tr( "OMPTP exclusive_times" ) );
    rightSideLayout->addWidget( omptp_exclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("omptp::inclusive_times")) {
       omptp_inclusive_times = TRUE;
    } else {
       omptp_inclusive_times = FALSE;
    }

    { // omptp_inclusive_times
    omptp_inclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "omptp_inclusive_times_CheckBox" );
    omptp_inclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, omptp_inclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    omptp_inclusive_times_CheckBox->setChecked( TRUE );
    omptp_inclusive_times_CheckBox->setText( tr( "OMPTP inclusive_times" ) );
    rightSideLayout->addWidget( omptp_inclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("omptp::percent")) {
       omptp_percent = TRUE;
    } else {
       omptp_percent = FALSE;
    }

    { // omptp_percent
    omptp_percent_CheckBox = new QCheckBox( GeneralGroupBox, "omptp_percent_CheckBox" );
    omptp_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, omptp_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    omptp_percent_CheckBox->setChecked( TRUE );
    omptp_percent_CheckBox->setText( tr( "OMPTP percent" ) );
    rightSideLayout->addWidget( omptp_percent_CheckBox );
    }

    if (isInCurrentModifierList("omptp::count")) {
       omptp_count = TRUE;
    } else {
       omptp_count = FALSE;
    }

    { // omptp_count
    omptp_count_CheckBox = new QCheckBox( GeneralGroupBox, "omptp_count_CheckBox" );
    omptp_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, omptp_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    omptp_count_CheckBox->setChecked( TRUE );
    omptp_count_CheckBox->setText( tr( "OMPTP count" ) );
    rightSideLayout->addWidget( omptp_count_CheckBox );
    }

    if (isInCurrentModifierList("omptp::ThreadAverage")) {
       omptp_ThreadAverage = TRUE;
    } else {
       omptp_ThreadAverage = FALSE;
    }

    { // omptp_ThreadAverage
    omptp_ThreadAverage_CheckBox = new QCheckBox( GeneralGroupBox, "omptp_ThreadAverage_CheckBox" );
    omptp_ThreadAverage_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, omptp_ThreadAverage_CheckBox->sizePolicy().hasHeightForWidth() ) );
    omptp_ThreadAverage_CheckBox->setChecked( TRUE );
    omptp_ThreadAverage_CheckBox->setText( tr( "OMPTP ThreadAverage" ) );
    rightSideLayout->addWidget( omptp_ThreadAverage_CheckBox );
    }

    if (isInCurrentModifierList("omptp::ThreadMin")) {
       omptp_ThreadMin = TRUE;
    } else {
       omptp_ThreadMin = FALSE;
    }

    { // omptp_ThreadMin
    omptp_ThreadMin_CheckBox = new QCheckBox( GeneralGroupBox, "omptp_ThreadMin_CheckBox" );
    omptp_ThreadMin_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, omptp_ThreadMin_CheckBox->sizePolicy().hasHeightForWidth() ) );
    omptp_ThreadMin_CheckBox->setChecked( TRUE );
    omptp_ThreadMin_CheckBox->setText( tr( "OMPTP ThreadMin" ) );
    rightSideLayout->addWidget( omptp_ThreadMin_CheckBox );
    }

    if (isInCurrentModifierList("omptp::ThreadMax")) {
       omptp_ThreadMax = TRUE;
    } else {
       omptp_ThreadMax = FALSE;
    }

    { // omptp_ThreadMax
    omptp_ThreadMax_CheckBox = new QCheckBox( GeneralGroupBox, "omptp_ThreadMax_CheckBox" );
    omptp_ThreadMax_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, omptp_ThreadMax_CheckBox->sizePolicy().hasHeightForWidth() ) );
    omptp_ThreadMax_CheckBox->setChecked( TRUE );
    omptp_ThreadMax_CheckBox->setText( tr( "OMPTP ThreadMax" ) );
    rightSideLayout->addWidget( omptp_ThreadMax_CheckBox );
    }

  } else if ( globalCollectorString.contains("hwcsamp") ) {

    VTraceGroupBox->hide();

    if (isInCurrentModifierList("hwcsamp::time")) {
       hwcsamp_time = TRUE;
    } else {
       hwcsamp_time = FALSE;
    }

    { // hwcsamp_time
    hwcsamp_time_CheckBox = new QCheckBox( GeneralGroupBox, "hwcsamp_time_CheckBox" );
    hwcsamp_time_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwcsamp_time_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwcsamp_time_CheckBox->setChecked( TRUE );
    hwcsamp_time_CheckBox->setText( tr( "HWCSAMP time" ) );
    rightSideLayout->addWidget( hwcsamp_time_CheckBox );
    }

    if (isInCurrentModifierList("hwcsamp::allEvents")) {
       hwcsamp_allEvents = TRUE;
    } else {
       hwcsamp_allEvents = FALSE;
    }

    { // hwcsamp_allEvents
    hwcsamp_allEvents_CheckBox = new QCheckBox( GeneralGroupBox, "hwcsamp_allEvents_CheckBox" );
    hwcsamp_allEvents_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwcsamp_allEvents_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwcsamp_allEvents_CheckBox->setChecked( TRUE );
    hwcsamp_allEvents_CheckBox->setText( tr( "HWCSAMP all events" ) );
    rightSideLayout->addWidget( hwcsamp_allEvents_CheckBox );
    }

    if (isInCurrentModifierList("hwcsamp::percent")) {
       hwcsamp_percent = TRUE;
    } else {
       hwcsamp_percent = FALSE;
    }

    { // hwcsamp_percent
    hwcsamp_percent_CheckBox = new QCheckBox( GeneralGroupBox, "hwcsamp_percent_CheckBox" );
    hwcsamp_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwcsamp_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwcsamp_percent_CheckBox->setChecked( TRUE );
    hwcsamp_percent_CheckBox->setText( tr( "HWCSAMP percent" ) );
    rightSideLayout->addWidget( hwcsamp_percent_CheckBox );
    }

    if (isInCurrentModifierList("hwcsamp::ThreadAverage")) {
       hwcsamp_ThreadAverage = TRUE;
    } else {
       hwcsamp_ThreadAverage = FALSE;
    }

    { // hwcsamp_ThreadAverage
    hwcsamp_ThreadAverage_CheckBox = new QCheckBox( GeneralGroupBox, "hwcsamp_ThreadAverage_CheckBox" );
    hwcsamp_ThreadAverage_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwcsamp_ThreadAverage_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwcsamp_ThreadAverage_CheckBox->setChecked( TRUE );
    hwcsamp_ThreadAverage_CheckBox->setText( tr( "HWCSAMP ThreadAverage" ) );
    rightSideLayout->addWidget( hwcsamp_ThreadAverage_CheckBox );
    }

    if (isInCurrentModifierList("hwcsamp::ThreadMin")) {
       hwcsamp_ThreadMin = TRUE;
    } else {
       hwcsamp_ThreadMin = FALSE;
    }

    { // hwcsamp_ThreadMin
    hwcsamp_ThreadMin_CheckBox = new QCheckBox( GeneralGroupBox, "hwcsamp_ThreadMin_CheckBox" );
    hwcsamp_ThreadMin_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwcsamp_ThreadMin_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwcsamp_ThreadMin_CheckBox->setChecked( TRUE );
    hwcsamp_ThreadMin_CheckBox->setText( tr( "HWCSAMP ThreadMin" ) );
    rightSideLayout->addWidget( hwcsamp_ThreadMin_CheckBox );
    }

    if (isInCurrentModifierList("hwcsamp::ThreadMax")) {
       hwcsamp_ThreadMax = TRUE;
    } else {
       hwcsamp_ThreadMax = FALSE;
    }

    { // hwcsamp_ThreadMax
    hwcsamp_ThreadMax_CheckBox = new QCheckBox( GeneralGroupBox, "hwcsamp_ThreadMax_CheckBox" );
    hwcsamp_ThreadMax_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwcsamp_ThreadMax_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwcsamp_ThreadMax_CheckBox->setChecked( TRUE );
    hwcsamp_ThreadMax_CheckBox->setText( tr( "HWCSAMP ThreadMax" ) );
    rightSideLayout->addWidget( hwcsamp_ThreadMax_CheckBox );
    }

  } else if ( globalCollectorString.contains("hwctime") ) {

    VTraceGroupBox->hide();

    if (isInCurrentModifierList("hwctime::exclusive_counts")) {
       hwctime_exclusive_counts = TRUE;
    } else {
       hwctime_exclusive_counts = FALSE;
    }

    { // hwctime_exclusive_counts
    hwctime_exclusive_counts_CheckBox = new QCheckBox( GeneralGroupBox, "hwctime_exclusive_counts_CheckBox" );
    hwctime_exclusive_counts_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwctime_exclusive_counts_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwctime_exclusive_counts_CheckBox->setChecked( TRUE );
    hwctime_exclusive_counts_CheckBox->setText( tr( "HWCTIME exclusive counts" ) );
    rightSideLayout->addWidget( hwctime_exclusive_counts_CheckBox );
    }

    if (isInCurrentModifierList("hwctime::exclusive_overflows")) {
       hwctime_exclusive_overflows = TRUE;
    } else {
       hwctime_exclusive_overflows = FALSE;
    }

    { // hwctime_exclusive_overflows
    hwctime_exclusive_overflows_CheckBox = new QCheckBox( GeneralGroupBox, "hwctime_exclusive_overflows_CheckBox" );
    hwctime_exclusive_overflows_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwctime_exclusive_overflows_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwctime_exclusive_overflows_CheckBox->setChecked( TRUE );
    hwctime_exclusive_overflows_CheckBox->setText( tr( "HWCTIME exclusive_overflows" ) );
    rightSideLayout->addWidget( hwctime_exclusive_overflows_CheckBox );
    }

    if (isInCurrentModifierList("hwctime::inclusive_overflows")) {
       hwctime_inclusive_overflows = TRUE;
    } else {
       hwctime_inclusive_overflows = FALSE;
    }

    { // hwctime_inclusive_overflows
    hwctime_inclusive_overflows_CheckBox = new QCheckBox( GeneralGroupBox, "hwctime_inclusive_overflows_CheckBox" );
    hwctime_inclusive_overflows_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwctime_inclusive_overflows_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwctime_inclusive_overflows_CheckBox->setChecked( TRUE );
    hwctime_inclusive_overflows_CheckBox->setText( tr( "HWCTIME inclusive_overflows" ) );
    rightSideLayout->addWidget( hwctime_inclusive_overflows_CheckBox );
    }

    if (isInCurrentModifierList("hwctime::inclusive_counts")) {
       hwctime_inclusive_counts = TRUE;
    } else {
       hwctime_inclusive_counts = FALSE;
    }

    { // hwctime_inclusive_counts
    hwctime_inclusive_counts_CheckBox = new QCheckBox( GeneralGroupBox, "hwctime_inclusive_counts_CheckBox" );
    hwctime_inclusive_counts_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwctime_inclusive_counts_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwctime_inclusive_counts_CheckBox->setChecked( TRUE );
    hwctime_inclusive_counts_CheckBox->setText( tr( "HWCTIME inclusive counts" ) );
    rightSideLayout->addWidget( hwctime_inclusive_counts_CheckBox );
    }

    if (isInCurrentModifierList("hwctime::percent")) {
       hwctime_percent = TRUE;
    } else {
       hwctime_percent = FALSE;
    }

    { // hwctime_percent
    hwctime_percent_CheckBox = new QCheckBox( GeneralGroupBox, "hwctime_percent_CheckBox" );
    hwctime_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwctime_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwctime_percent_CheckBox->setChecked( TRUE );
    hwctime_percent_CheckBox->setText( tr( "HWCTIME percent" ) );
    rightSideLayout->addWidget( hwctime_percent_CheckBox );
    }

    if (isInCurrentModifierList("hwctime::ThreadAverage")) {
       hwctime_ThreadAverage = TRUE;
    } else {
       hwctime_ThreadAverage = FALSE;
    }

    { // hwctime_ThreadAverage
    hwctime_ThreadAverage_CheckBox = new QCheckBox( GeneralGroupBox, "hwctime_ThreadAverage_CheckBox" );
    hwctime_ThreadAverage_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwctime_ThreadAverage_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwctime_ThreadAverage_CheckBox->setChecked( TRUE );
    hwctime_ThreadAverage_CheckBox->setText( tr( "HWCTIME ThreadAverage" ) );
    rightSideLayout->addWidget( hwctime_ThreadAverage_CheckBox );
    }

    if (isInCurrentModifierList("hwctime::ThreadMin")) {
       hwctime_ThreadMin = TRUE;
    } else {
       hwctime_ThreadMin = FALSE;
    }

    { // hwctime_ThreadMin
    hwctime_ThreadMin_CheckBox = new QCheckBox( GeneralGroupBox, "hwctime_ThreadMin_CheckBox" );
    hwctime_ThreadMin_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwctime_ThreadMin_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwctime_ThreadMin_CheckBox->setChecked( TRUE );
    hwctime_ThreadMin_CheckBox->setText( tr( "HWCTIME ThreadMin" ) );
    rightSideLayout->addWidget( hwctime_ThreadMin_CheckBox );
    }

    if (isInCurrentModifierList("hwctime::ThreadMax")) {
       hwctime_ThreadMax = TRUE;
    } else {
       hwctime_ThreadMax = FALSE;
    }

    { // hwctime_ThreadMax
    hwctime_ThreadMax_CheckBox = new QCheckBox( GeneralGroupBox, "hwctime_ThreadMax_CheckBox" );
    hwctime_ThreadMax_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwctime_ThreadMax_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwctime_ThreadMax_CheckBox->setChecked( TRUE );
    hwctime_ThreadMax_CheckBox->setText( tr( "HWCTIME ThreadMax" ) );
    rightSideLayout->addWidget( hwctime_ThreadMax_CheckBox );
    }

  } else if ( globalCollectorString.contains("hwc") ) {
    VTraceGroupBox->hide();

    if (isInCurrentModifierList("hwc::overflows")) {
       hwc_overflows = TRUE;
    } else {
       hwc_overflows = FALSE;
    }

    { // hwc_overflows
    hwc_overflows_CheckBox = new QCheckBox( GeneralGroupBox, "hwc_overflows_CheckBox" );
    hwc_overflows_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwc_overflows_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwc_overflows_CheckBox->setChecked( TRUE );
    hwc_overflows_CheckBox->setText( tr( "HWC overflows" ) );
    rightSideLayout->addWidget( hwc_overflows_CheckBox );
    }

    if (isInCurrentModifierList("hwc::counts")) {
       hwc_counts = TRUE;
    } else {
       hwc_counts = FALSE;
    }

    { // hwc_counts
    hwc_counts_CheckBox = new QCheckBox( GeneralGroupBox, "hwc_counts_CheckBox" );
    hwc_counts_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwc_counts_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwc_counts_CheckBox->setChecked( TRUE );
    hwc_counts_CheckBox->setText( tr( "HWC count values" ) );
    rightSideLayout->addWidget( hwc_counts_CheckBox );
    }

    if (isInCurrentModifierList("hwc::percent")) {
       hwc_percent = TRUE;
    } else {
       hwc_percent = FALSE;
    }

    { // hwc_percent
    hwc_percent_CheckBox = new QCheckBox( GeneralGroupBox, "hwc_percent_CheckBox" );
    hwc_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwc_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwc_percent_CheckBox->setChecked( TRUE );
    hwc_percent_CheckBox->setText( tr( "HWC percent" ) );
    rightSideLayout->addWidget( hwc_percent_CheckBox );
    }

    if (isInCurrentModifierList("hwc::ThreadAverage")) {
       hwc_ThreadAverage = TRUE;
    } else {
       hwc_ThreadAverage = FALSE;
    }

    { // hwc_ThreadAverage
    hwc_ThreadAverage_CheckBox = new QCheckBox( GeneralGroupBox, "hwc_ThreadAverage_CheckBox" );
    hwc_ThreadAverage_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwc_ThreadAverage_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwc_ThreadAverage_CheckBox->setChecked( TRUE );
    hwc_ThreadAverage_CheckBox->setText( tr( "HWC ThreadAverage" ) );
    rightSideLayout->addWidget( hwc_ThreadAverage_CheckBox );
    }

    if (isInCurrentModifierList("hwc::ThreadMin")) {
       hwc_ThreadMin = TRUE;
    } else {
       hwc_ThreadMin = FALSE;
    }

    { // hwc_ThreadMin
    hwc_ThreadMin_CheckBox = new QCheckBox( GeneralGroupBox, "hwc_ThreadMin_CheckBox" );
    hwc_ThreadMin_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwc_ThreadMin_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwc_ThreadMin_CheckBox->setChecked( TRUE );
    hwc_ThreadMin_CheckBox->setText( tr( "HWC ThreadMin" ) );
    rightSideLayout->addWidget( hwc_ThreadMin_CheckBox );
    }

    if (isInCurrentModifierList("hwc::ThreadMax")) {
       hwc_ThreadMax = TRUE;
    } else {
       hwc_ThreadMax = FALSE;
    }

    { // hwc_ThreadMax
    hwc_ThreadMax_CheckBox = new QCheckBox( GeneralGroupBox, "hwc_ThreadMax_CheckBox" );
    hwc_ThreadMax_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwc_ThreadMax_CheckBox->sizePolicy().hasHeightForWidth() ) );
    hwc_ThreadMax_CheckBox->setChecked( TRUE );
    hwc_ThreadMax_CheckBox->setText( tr( "HWC ThreadMax" ) );
    rightSideLayout->addWidget( hwc_ThreadMax_CheckBox );
    }


  } else if ( globalCollectorString.contains("iot") ) {


    if (isInCurrentModifierList("iot::exclusive_times")) {
       iot_exclusive_times = TRUE;
    } else {
       iot_exclusive_times = FALSE;
    }

    { // iot_exclusive_times
    iot_exclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "iot_exclusive_times_CheckBox" );
    iot_exclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_exclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_exclusive_times_CheckBox->setChecked( iot_exclusive_times );
    iot_exclusive_times_CheckBox->setText( tr( "IOT Exclusive Times" ) );
    rightSideLayout->addWidget( iot_exclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("iot::inclusive_times")) {
       iot_inclusive_times = TRUE;
    } else {
       iot_inclusive_times = FALSE;
    }

    { // iot_inclusive_times
    iot_inclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "iot_inclusive_times_CheckBox" );
    iot_inclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_inclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_inclusive_times_CheckBox->setChecked( iot_inclusive_times );
    iot_inclusive_times_CheckBox->setText( tr( "IOT Inclusive Times" ) );
    rightSideLayout->addWidget( iot_inclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("min")) {
       iot_min = TRUE;
    } else {
       iot_min = FALSE;
    }

    { // iot_min
    iot_min_CheckBox = new QCheckBox( GeneralGroupBox, "iot_min_CheckBox" );
    iot_min_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_min_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_min_CheckBox->setChecked( iot_min );
    iot_min_CheckBox->setText( tr( "IOT Min" ) );
    rightSideLayout->addWidget( iot_min_CheckBox );
    }

    if (isInCurrentModifierList("max")) {
       iot_max = TRUE;
    } else {
       iot_max = FALSE;
    }

    { // iot_max
    iot_max_CheckBox = new QCheckBox( GeneralGroupBox, "iot_max_CheckBox" );
    iot_max_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_max_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_max_CheckBox->setChecked( iot_max );
    iot_max_CheckBox->setText( tr( "IOT Max" ) );
    rightSideLayout->addWidget( iot_max_CheckBox );
    }

    if (isInCurrentModifierList("average")) {
       iot_average = TRUE;
    } else {
       iot_average = FALSE;
    }

    { // iot_average
    iot_average_CheckBox = new QCheckBox( GeneralGroupBox, "iot_average_CheckBox" );
    iot_average_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_average_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_average_CheckBox->setChecked( iot_average );
    iot_average_CheckBox->setText( tr( "IOT Average" ) );
    rightSideLayout->addWidget( iot_average_CheckBox );
    }

    if (isInCurrentModifierList("count")) {
       iot_count = TRUE;
    } else {
       iot_count = FALSE;
    }

    { // iot_count
    iot_count_CheckBox = new QCheckBox( GeneralGroupBox, "iot_count_CheckBox" );
    iot_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_count_CheckBox->setChecked( iot_count );
    iot_count_CheckBox->setText( tr( "IOT Count" ) );
    rightSideLayout->addWidget( iot_count_CheckBox );
    }

    if (isInCurrentModifierList("percent")) {
       iot_percent = TRUE;
    } else {
       iot_percent = FALSE;
    }

    { // iot_percent
    iot_percent_CheckBox = new QCheckBox( GeneralGroupBox, "iot_percent_CheckBox" );
    iot_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_percent_CheckBox->setChecked( iot_percent );
    iot_percent_CheckBox->setText( tr( "IOT Percent" ) );
    rightSideLayout->addWidget( iot_percent_CheckBox );
    }

    if (isInCurrentModifierList("stddev")) {
       iot_stddev = TRUE;
    } else {
       iot_stddev = FALSE;
    }

    { // iot_stddev
    iot_stddev_CheckBox = new QCheckBox( GeneralGroupBox, "iot_stddev_CheckBox" );
    iot_stddev_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_stddev_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_stddev_CheckBox->setChecked( iot_stddev );
    iot_stddev_CheckBox->setText( tr( "IOT Standard Deviation" ) );
    rightSideLayout->addWidget( iot_stddev_CheckBox );
    }

    if (isInCurrentModifierList("start_time")) {
       iot_start_time = TRUE;
    } else {
       iot_start_time = FALSE;
    }

    { // iot_start_time
    iot_start_time_CheckBox = new QCheckBox( VTraceGroupBox, "iot_start_time_CheckBox" );
    iot_start_time_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_start_time_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_start_time_CheckBox->setChecked( iot_start_time );
    iot_start_time_CheckBox->setText( tr( "IOT Start Time" ) );
    rightSideVTraceLayout->addWidget( iot_start_time_CheckBox );
    }

    if (isInCurrentModifierList("stop_time")) {
       iot_stop_time = TRUE;
    } else {
       iot_stop_time = FALSE;
    }

    { // iot_stop_time
    iot_stop_time_CheckBox = new QCheckBox( VTraceGroupBox, "iot_stop_time_CheckBox" );
    iot_stop_time_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_stop_time_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_stop_time_CheckBox->setChecked( iot_stop_time );
    iot_stop_time_CheckBox->setText( tr( "IOT Stop Time" ) );
    rightSideVTraceLayout->addWidget( iot_stop_time_CheckBox );
    }

    if (isInCurrentModifierList("syscallno")) {
       iot_syscallno = TRUE;
    } else {
       iot_syscallno = FALSE;
    }

    { // iot_syscallno
    iot_syscallno_CheckBox = new QCheckBox( VTraceGroupBox, "iot_syscallno_CheckBox" );
    iot_syscallno_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_syscallno_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_syscallno_CheckBox->setChecked( iot_syscallno );
    iot_syscallno_CheckBox->setText( tr( "IOT System Call Number" ) );
    rightSideVTraceLayout->addWidget( iot_syscallno_CheckBox );
    }

    if (isInCurrentModifierList("nsysargs")) {
       iot_nsysargs = TRUE;
    } else {
       iot_nsysargs = FALSE;
    }

    { // iot_nsysargs
    iot_nsysargs_CheckBox = new QCheckBox( VTraceGroupBox, "iot_nsysargs_CheckBox" );
    iot_nsysargs_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_nsysargs_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_nsysargs_CheckBox->setChecked( iot_nsysargs );
    iot_nsysargs_CheckBox->setText( tr( "IOT Number of Arguments to System Call" ) );
    rightSideVTraceLayout->addWidget( iot_nsysargs_CheckBox );
    }

    if (isInCurrentModifierList("retval")) {
       iot_retval = TRUE;
    } else {
       iot_retval = FALSE;
    }

    { // iot_retval
    iot_retval_CheckBox = new QCheckBox( VTraceGroupBox, "iot_retval_CheckBox" );
    iot_retval_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_retval_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_retval_CheckBox->setChecked( iot_retval );
    iot_retval_CheckBox->setText( tr( "IOT System Call Dependent Return Value (bytes read, bytes written etc." ) );
    rightSideVTraceLayout->addWidget( iot_retval_CheckBox );
    }

#if PATHNAME_READY
    if (isInCurrentModifierList("pathname")) {
       iot_pathname = TRUE;
    } else {
       iot_pathname = FALSE;
    }

    { // iot_pathname
    iot_pathname_CheckBox = new QCheckBox( VTraceGroupBox, "iot_pathname_CheckBox" );
    iot_pathname_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iot_pathname_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iot_pathname_CheckBox->setChecked( iot_pathname );
    iot_pathname_CheckBox->setText( tr( "IOT PathName" ) );
    rightSideVTraceLayout->addWidget( iot_pathname_CheckBox );
    }
#endif

  } else if ( globalCollectorString.contains("iop") ) {

    VTraceGroupBox->hide();

    if (isInCurrentModifierList("iop::exclusive_times")) {
       iop_exclusive_times = TRUE;
    } else {
       iop_exclusive_times = FALSE;
    }

    { // iop_exclusive_times
    iop_exclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "iop_exclusive_times_CheckBox" );
    iop_exclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iop_exclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iop_exclusive_times_CheckBox->setChecked( TRUE );
    iop_exclusive_times_CheckBox->setText( tr( "IOP exclusive_times" ) );
    rightSideLayout->addWidget( iop_exclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("iop::inclusive_times")) {
       iop_inclusive_times = TRUE;
    } else {
       iop_inclusive_times = FALSE;
    }

    { // iop_inclusive_times
    iop_inclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "iop_inclusive_times_CheckBox" );
    iop_inclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iop_inclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iop_inclusive_times_CheckBox->setChecked( TRUE );
    iop_inclusive_times_CheckBox->setText( tr( "IOP inclusive_times" ) );
    rightSideLayout->addWidget( iop_inclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("iop::percent")) {
       iop_percent = TRUE;
    } else {
       iop_percent = FALSE;
    }

    { // iop_percent
    iop_percent_CheckBox = new QCheckBox( GeneralGroupBox, "iop_percent_CheckBox" );
    iop_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iop_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iop_percent_CheckBox->setChecked( TRUE );
    iop_percent_CheckBox->setText( tr( "IOP percent" ) );
    rightSideLayout->addWidget( iop_percent_CheckBox );
    }

    if (isInCurrentModifierList("iop::count")) {
       iop_count = TRUE;
    } else {
       iop_count = FALSE;
    }

    { // iop_count
    iop_count_CheckBox = new QCheckBox( GeneralGroupBox, "iop_count_CheckBox" );
    iop_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iop_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iop_count_CheckBox->setChecked( TRUE );
    iop_count_CheckBox->setText( tr( "IOP count" ) );
    rightSideLayout->addWidget( iop_count_CheckBox );
    }

    if (isInCurrentModifierList("iop::ThreadAverage")) {
       iop_ThreadAverage = TRUE;
    } else {
       iop_ThreadAverage = FALSE;
    }

    { // iop_ThreadAverage
    iop_ThreadAverage_CheckBox = new QCheckBox( GeneralGroupBox, "iop_ThreadAverage_CheckBox" );
    iop_ThreadAverage_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iop_ThreadAverage_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iop_ThreadAverage_CheckBox->setChecked( TRUE );
    iop_ThreadAverage_CheckBox->setText( tr( "IOP ThreadAverage" ) );
    rightSideLayout->addWidget( iop_ThreadAverage_CheckBox );
    }

    if (isInCurrentModifierList("iop::ThreadMin")) {
       iop_ThreadMin = TRUE;
    } else {
       iop_ThreadMin = FALSE;
    }

    { // iop_ThreadMin
    iop_ThreadMin_CheckBox = new QCheckBox( GeneralGroupBox, "iop_ThreadMin_CheckBox" );
    iop_ThreadMin_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iop_ThreadMin_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iop_ThreadMin_CheckBox->setChecked( TRUE );
    iop_ThreadMin_CheckBox->setText( tr( "IOP ThreadMin" ) );
    rightSideLayout->addWidget( iop_ThreadMin_CheckBox );
    }

    if (isInCurrentModifierList("iop::ThreadMax")) {
       iop_ThreadMax = TRUE;
    } else {
       iop_ThreadMax = FALSE;
    }

    { // iop_ThreadMax
    iop_ThreadMax_CheckBox = new QCheckBox( GeneralGroupBox, "iop_ThreadMax_CheckBox" );
    iop_ThreadMax_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, iop_ThreadMax_CheckBox->sizePolicy().hasHeightForWidth() ) );
    iop_ThreadMax_CheckBox->setChecked( TRUE );
    iop_ThreadMax_CheckBox->setText( tr( "IOP ThreadMax" ) );
    rightSideLayout->addWidget( iop_ThreadMax_CheckBox );
    }

   } else if (globalCollectorString.contains("io") ) {
    VTraceGroupBox->hide();

    if (isInCurrentModifierList("io::exclusive_times")) {
       io_exclusive_times = TRUE;
    } else {
       io_exclusive_times = FALSE;
    }
    { // io_exclusive_times
    io_exclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "io_exclusive_times_CheckBox" );
    io_exclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, io_exclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    io_exclusive_times_CheckBox->setChecked( TRUE );
    io_exclusive_times_CheckBox->setText( tr( "IO Exclusive Times Value" ) );
    rightSideLayout->addWidget( io_exclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("min")) {
       io_min = TRUE;
    } else {
       io_min = FALSE;
    }
    { // io_min
    io_min_CheckBox = new QCheckBox( GeneralGroupBox, "io_min_CheckBox" );
    io_min_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, io_min_CheckBox->sizePolicy().hasHeightForWidth() ) );
    io_min_CheckBox->setChecked( TRUE );
    io_min_CheckBox->setText( tr( "IO Minimum Value" ) );
    rightSideLayout->addWidget( io_min_CheckBox );
    }

    if (isInCurrentModifierList("max")) {
       io_max = TRUE;
    } else {
       io_max = FALSE;
    }

    { // io_max
    io_max_CheckBox = new QCheckBox( GeneralGroupBox, "io_max_CheckBox" );
    io_max_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, io_max_CheckBox->sizePolicy().hasHeightForWidth() ) );
    io_max_CheckBox->setChecked( TRUE );
    io_max_CheckBox->setText( tr( "IO Maximum Value" ) );
    rightSideLayout->addWidget( io_max_CheckBox );
    }

    if (isInCurrentModifierList("average")) {
       io_average = TRUE;
    } else {
       io_average = FALSE;
    }

    { // io_average
    io_average_CheckBox = new QCheckBox( GeneralGroupBox, "io_average_CheckBox" );
    io_average_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, io_average_CheckBox->sizePolicy().hasHeightForWidth() ) );
    io_average_CheckBox->setChecked( TRUE );
    io_average_CheckBox->setText( tr( "IO Average Value" ) );
    rightSideLayout->addWidget( io_average_CheckBox );
    }

    if (isInCurrentModifierList("count")) {
       io_count = TRUE;
    } else {
       io_count = FALSE;
    }

    { // io_count
    io_count_CheckBox = new QCheckBox( GeneralGroupBox, "io_count_CheckBox" );
    io_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, io_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    io_count_CheckBox->setChecked( TRUE );
    io_count_CheckBox->setText( tr( "IO Count" ) );
    rightSideLayout->addWidget( io_count_CheckBox );
    }

    if (isInCurrentModifierList("percent")) {
       io_percent = TRUE;
    } else {
       io_percent = FALSE;
    }

    { // io_percent
    io_percent_CheckBox = new QCheckBox( GeneralGroupBox, "io_percent_CheckBox" );
    io_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, io_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    io_percent_CheckBox->setChecked( TRUE );
    io_percent_CheckBox->setText( tr( "IO Percent" ) );
    rightSideLayout->addWidget( io_percent_CheckBox );
    }

    if (isInCurrentModifierList("stddev")) {
       io_stddev = TRUE;
    } else {
       io_stddev = FALSE;
    }

    { // io_stddev
    io_stddev_CheckBox = new QCheckBox( GeneralGroupBox, "io_stddev_CheckBox" );
    io_stddev_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, io_stddev_CheckBox->sizePolicy().hasHeightForWidth() ) );
    io_stddev_CheckBox->setChecked( TRUE );
    io_stddev_CheckBox->setText( tr( "IO Standard Deviation" ) );
    rightSideLayout->addWidget( io_stddev_CheckBox );
    }

    if (isInCurrentModifierList("ThreadAverage")) {
       io_ThreadAverage = TRUE;
    } else {
       io_ThreadAverage = FALSE;
    }

    { // io_ThreadAverage
    io_ThreadAverage_CheckBox = new QCheckBox( GeneralGroupBox, "io_ThreadAverage_CheckBox" );
    io_ThreadAverage_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, io_ThreadAverage_CheckBox->sizePolicy().hasHeightForWidth() ) );
    io_ThreadAverage_CheckBox->setChecked( TRUE );
    io_ThreadAverage_CheckBox->setText( tr( "IO Thread Average Value" ) );
    rightSideLayout->addWidget( io_ThreadAverage_CheckBox );
    }

    if (isInCurrentModifierList("ThreadMin")) {
       io_ThreadMin = TRUE;
    } else {
       io_ThreadMin = FALSE;
    }

    { // io_ThreadMin
    io_ThreadMin_CheckBox = new QCheckBox( GeneralGroupBox, "io_ThreadMin_CheckBox" );
    io_ThreadMin_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, io_ThreadMin_CheckBox->sizePolicy().hasHeightForWidth() ) );
    io_ThreadMin_CheckBox->setChecked( TRUE );
    io_ThreadMin_CheckBox->setText( tr( "IO Thread Minimum Value" ) );
    rightSideLayout->addWidget( io_ThreadMin_CheckBox );
    }

    if (isInCurrentModifierList("ThreadMax")) {
       io_ThreadMax = TRUE;
    } else {
       io_ThreadMax = FALSE;
    }

    { // io_ThreadMax
    io_ThreadMax_CheckBox = new QCheckBox( GeneralGroupBox, "io_ThreadMax_CheckBox" );
    io_ThreadMax_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, io_ThreadMax_CheckBox->sizePolicy().hasHeightForWidth() ) );
    io_ThreadMax_CheckBox->setChecked( TRUE );
    io_ThreadMax_CheckBox->setText( tr( "IO Thread Maximum Value" ) );
    rightSideLayout->addWidget( io_ThreadMax_CheckBox );
    }

   } else if (globalCollectorString.contains("mem") ) {
    VTraceGroupBox->hide();

    if (isInCurrentModifierList("mem::exclusive_times")) {
       mem_exclusive_times = TRUE;
    } else {
       mem_exclusive_times = FALSE;
    }
    { // mem_exclusive_times
    mem_exclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "mem_exclusive_times_CheckBox" );
    mem_exclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mem_exclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mem_exclusive_times_CheckBox->setChecked( TRUE );
    mem_exclusive_times_CheckBox->setText( tr( "MEM Exclusive Times Value" ) );
    rightSideLayout->addWidget( mem_exclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("min")) {
       mem_min = TRUE;
    } else {
       mem_min = FALSE;
    }
    { // mem_min
    mem_min_CheckBox = new QCheckBox( GeneralGroupBox, "mem_min_CheckBox" );
    mem_min_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mem_min_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mem_min_CheckBox->setChecked( TRUE );
    mem_min_CheckBox->setText( tr( "MEM Minimum Value" ) );
    rightSideLayout->addWidget( mem_min_CheckBox );
    }

    if (isInCurrentModifierList("max")) {
       mem_max = TRUE;
    } else {
       mem_max = FALSE;
    }

    { // mem_max
    mem_max_CheckBox = new QCheckBox( GeneralGroupBox, "mem_max_CheckBox" );
    mem_max_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mem_max_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mem_max_CheckBox->setChecked( TRUE );
    mem_max_CheckBox->setText( tr( "MEM Maximum Value" ) );
    rightSideLayout->addWidget( mem_max_CheckBox );
    }

    if (isInCurrentModifierList("average")) {
       mem_average = TRUE;
    } else {
       mem_average = FALSE;
    }

    { // mem_average
    mem_average_CheckBox = new QCheckBox( GeneralGroupBox, "mem_average_CheckBox" );
    mem_average_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mem_average_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mem_average_CheckBox->setChecked( TRUE );
    mem_average_CheckBox->setText( tr( "MEM Average Value" ) );
    rightSideLayout->addWidget( mem_average_CheckBox );
    }

    if (isInCurrentModifierList("count")) {
       mem_count = TRUE;
    } else {
       mem_count = FALSE;
    }

    { // mem_count
    mem_count_CheckBox = new QCheckBox( GeneralGroupBox, "mem_count_CheckBox" );
    mem_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mem_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mem_count_CheckBox->setChecked( TRUE );
    mem_count_CheckBox->setText( tr( "MEM Count" ) );
    rightSideLayout->addWidget( mem_count_CheckBox );
    }

    if (isInCurrentModifierList("percent")) {
       mem_percent = TRUE;
    } else {
       mem_percent = FALSE;
    }

    { // mem_percent
    mem_percent_CheckBox = new QCheckBox( GeneralGroupBox, "mem_percent_CheckBox" );
    mem_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mem_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mem_percent_CheckBox->setChecked( TRUE );
    mem_percent_CheckBox->setText( tr( "MEM Percent" ) );
    rightSideLayout->addWidget( mem_percent_CheckBox );
    }

    if (isInCurrentModifierList("stddev")) {
       mem_stddev = TRUE;
    } else {
       mem_stddev = FALSE;
    }

    { // mem_stddev
    mem_stddev_CheckBox = new QCheckBox( GeneralGroupBox, "mem_stddev_CheckBox" );
    mem_stddev_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mem_stddev_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mem_stddev_CheckBox->setChecked( TRUE );
    mem_stddev_CheckBox->setText( tr( "MEM Standard Deviatmemn" ) );
    rightSideLayout->addWidget( mem_stddev_CheckBox );
    }

    if (isInCurrentModifierList("ThreadAverage")) {
       mem_ThreadAverage = TRUE;
    } else {
       mem_ThreadAverage = FALSE;
    }

    { // mem_ThreadAverage
    mem_ThreadAverage_CheckBox = new QCheckBox( GeneralGroupBox, "mem_ThreadAverage_CheckBox" );
    mem_ThreadAverage_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mem_ThreadAverage_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mem_ThreadAverage_CheckBox->setChecked( TRUE );
    mem_ThreadAverage_CheckBox->setText( tr( "MEM Thread Average Value" ) );
    rightSideLayout->addWidget( mem_ThreadAverage_CheckBox );
    }

    if (isInCurrentModifierList("ThreadMin")) {
       mem_ThreadMin = TRUE;
    } else {
       mem_ThreadMin = FALSE;
    }

    { // mem_ThreadMin
    mem_ThreadMin_CheckBox = new QCheckBox( GeneralGroupBox, "mem_ThreadMin_CheckBox" );
    mem_ThreadMin_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mem_ThreadMin_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mem_ThreadMin_CheckBox->setChecked( TRUE );
    mem_ThreadMin_CheckBox->setText( tr( "MEM Thread Minimum Value" ) );
    rightSideLayout->addWidget( mem_ThreadMin_CheckBox );
    }

    if (isInCurrentModifierList("ThreadMax")) {
       mem_ThreadMax = TRUE;
    } else {
       mem_ThreadMax = FALSE;
    }

    { // mem_ThreadMax
    mem_ThreadMax_CheckBox = new QCheckBox( GeneralGroupBox, "mem_ThreadMax_CheckBox" );
    mem_ThreadMax_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mem_ThreadMax_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mem_ThreadMax_CheckBox->setChecked( TRUE );
    mem_ThreadMax_CheckBox->setText( tr( "MEM Thread Maximum Value" ) );
    rightSideLayout->addWidget( mem_ThreadMax_CheckBox );
    }

  } else if ( globalCollectorString.contains("mpip") ) {

    VTraceGroupBox->hide();

    if (isInCurrentModifierList("mpip::exclusive_times")) {
       mpip_exclusive_times = TRUE;
    } else {
       mpip_exclusive_times = FALSE;
    }

    { // mpip_exclusive_times
    mpip_exclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "mpip_exclusive_times_CheckBox" );
    mpip_exclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpip_exclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpip_exclusive_times_CheckBox->setChecked( TRUE );
    mpip_exclusive_times_CheckBox->setText( tr( "MPIP exclusive_times" ) );
    rightSideLayout->addWidget( mpip_exclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("mpip::inclusive_times")) {
       mpip_inclusive_times = TRUE;
    } else {
       mpip_inclusive_times = FALSE;
    }

    { // mpip_inclusive_times
    mpip_inclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "mpip_inclusive_times_CheckBox" );
    mpip_inclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpip_inclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpip_inclusive_times_CheckBox->setChecked( TRUE );
    mpip_inclusive_times_CheckBox->setText( tr( "MPIP inclusive_times" ) );
    rightSideLayout->addWidget( mpip_inclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("mpip::percent")) {
       mpip_percent = TRUE;
    } else {
       mpip_percent = FALSE;
    }

    { // mpip_percent
    mpip_percent_CheckBox = new QCheckBox( GeneralGroupBox, "mpip_percent_CheckBox" );
    mpip_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpip_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpip_percent_CheckBox->setChecked( TRUE );
    mpip_percent_CheckBox->setText( tr( "MPIP percent" ) );
    rightSideLayout->addWidget( mpip_percent_CheckBox );
    }

    if (isInCurrentModifierList("mpip::count")) {
       mpip_count = TRUE;
    } else {
       mpip_count = FALSE;
    }

    { // mpip_count
    mpip_count_CheckBox = new QCheckBox( GeneralGroupBox, "mpip_count_CheckBox" );
    mpip_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpip_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpip_count_CheckBox->setChecked( TRUE );
    mpip_count_CheckBox->setText( tr( "MPIP count" ) );
    rightSideLayout->addWidget( mpip_count_CheckBox );
    }

    if (isInCurrentModifierList("mpip::ThreadAverage")) {
       mpip_ThreadAverage = TRUE;
    } else {
       mpip_ThreadAverage = FALSE;
    }

    { // mpip_ThreadAverage
    mpip_ThreadAverage_CheckBox = new QCheckBox( GeneralGroupBox, "mpip_ThreadAverage_CheckBox" );
    mpip_ThreadAverage_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpip_ThreadAverage_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpip_ThreadAverage_CheckBox->setChecked( TRUE );
    mpip_ThreadAverage_CheckBox->setText( tr( "MPIP ThreadAverage" ) );
    rightSideLayout->addWidget( mpip_ThreadAverage_CheckBox );
    }

    if (isInCurrentModifierList("mpip::ThreadMin")) {
       mpip_ThreadMin = TRUE;
    } else {
       mpip_ThreadMin = FALSE;
    }

    { // mpip_ThreadMin
    mpip_ThreadMin_CheckBox = new QCheckBox( GeneralGroupBox, "mpip_ThreadMin_CheckBox" );
    mpip_ThreadMin_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpip_ThreadMin_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpip_ThreadMin_CheckBox->setChecked( TRUE );
    mpip_ThreadMin_CheckBox->setText( tr( "MPIP ThreadMin" ) );
    rightSideLayout->addWidget( mpip_ThreadMin_CheckBox );
    }

    if (isInCurrentModifierList("mpip::ThreadMax")) {
       mpip_ThreadMax = TRUE;
    } else {
       mpip_ThreadMax = FALSE;
    }

    { // mpip_ThreadMax
    mpip_ThreadMax_CheckBox = new QCheckBox( GeneralGroupBox, "mpip_ThreadMax_CheckBox" );
    mpip_ThreadMax_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpip_ThreadMax_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpip_ThreadMax_CheckBox->setChecked( TRUE );
    mpip_ThreadMax_CheckBox->setText( tr( "MPIP ThreadMax" ) );
    rightSideLayout->addWidget( mpip_ThreadMax_CheckBox );
    }

   } else if ( globalCollectorString.contains("mpit") ) {

    if (isInCurrentModifierList("mpit::exclusive_times")) {
       mpit_exclusive_times = TRUE;
    } else {
       mpit_exclusive_times = FALSE;
    }

    { // mpit_exclusive_times
    mpit_exclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "mpit_exclusive_times_CheckBox" );
    mpit_exclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_exclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_exclusive_times_CheckBox->setChecked( TRUE );
    mpit_exclusive_times_CheckBox->setText( tr( "MPIT Exclusive Time Values." ) );
    rightSideLayout->addWidget( mpit_exclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("mpit::inclusive_times")) {
       mpit_inclusive_times = TRUE;
    } else {
       mpit_inclusive_times = FALSE;
    }

    { // mpit_inclusive_times
    mpit_inclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "mpit_inclusive_times_CheckBox" );
    mpit_inclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_inclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_inclusive_times_CheckBox->setChecked( TRUE );
    mpit_inclusive_times_CheckBox->setText( tr( "MPIT Inclusive Time Values." ) );
    rightSideLayout->addWidget( mpit_inclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("min")) {
       mpit_min = TRUE;
    } else {
       mpit_min = FALSE;
    }

    { // mpit_min
    mpit_min_CheckBox = new QCheckBox( GeneralGroupBox, "mpit_min_CheckBox" );
    mpit_min_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_min_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_min_CheckBox->setChecked( TRUE );
    mpit_min_CheckBox->setText( tr( "MPIT Minimum Time Values." ) );
    rightSideLayout->addWidget( mpit_min_CheckBox );
    }

    if (isInCurrentModifierList("max")) {
       mpit_max = TRUE;
    } else {
       mpit_max = FALSE;
    }

    { // mpit_max
    mpit_max_CheckBox = new QCheckBox( GeneralGroupBox, "mpit_max_CheckBox" );
    mpit_max_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_max_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_max_CheckBox->setChecked( TRUE );
    mpit_max_CheckBox->setText( tr( "MPIT Maximum Time Values." ) );
    rightSideLayout->addWidget( mpit_max_CheckBox );
    }

    if (isInCurrentModifierList("average")) {
       mpit_average = TRUE;
    } else {
       mpit_average = FALSE;
    }

    { // mpit_average
    mpit_average_CheckBox = new QCheckBox( GeneralGroupBox, "mpit_average_CheckBox" );
    mpit_average_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_average_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_average_CheckBox->setChecked( TRUE );
    mpit_average_CheckBox->setText( tr( "MPIT Average Time Values." ) );
    rightSideLayout->addWidget( mpit_average_CheckBox );
    }

    if (isInCurrentModifierList("count")) {
       mpit_count = TRUE;
    } else {
       mpit_count = FALSE;
    }

    { // mpit_count
    mpit_count_CheckBox = new QCheckBox( GeneralGroupBox, "mpit_count_CheckBox" );
    mpit_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_count_CheckBox->setChecked( TRUE );
    mpit_count_CheckBox->setText( tr( "MPIT Count (Calls To Function)." ) );
    rightSideLayout->addWidget( mpit_count_CheckBox );
    }

    if (isInCurrentModifierList("percent")) {
       mpit_percent = TRUE;
    } else {
       mpit_percent = FALSE;
    }

    { // mpit_percent
    mpit_percent_CheckBox = new QCheckBox( GeneralGroupBox, "mpit_percent_CheckBox" );
    mpit_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_percent_CheckBox->setChecked( TRUE );
    mpit_percent_CheckBox->setText( tr( "MPIT Exclusive Time Percentage Values." ) );
    rightSideLayout->addWidget( mpit_percent_CheckBox );
    }

    if (isInCurrentModifierList("stddev")) {
       mpit_stddev = TRUE;
    } else {
       mpit_stddev = FALSE;
    }

    { // mpit_stddev
    mpit_stddev_CheckBox = new QCheckBox( GeneralGroupBox, "mpit_stddev_CheckBox" );
    mpit_stddev_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_stddev_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_stddev_CheckBox->setChecked( TRUE );
    mpit_stddev_CheckBox->setText( tr( "MPIT Standard Deviation Values." ) );
    rightSideLayout->addWidget( mpit_stddev_CheckBox );
    }

    if (isInCurrentModifierList("start_time")) {
       mpit_start_time = TRUE;
    } else {
       mpit_start_time = FALSE;
    }

    { // mpit_start_time
    mpit_start_time_CheckBox = new QCheckBox( VTraceGroupBox, "mpit_start_time_CheckBox" );
    mpit_start_time_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_start_time_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_start_time_CheckBox->setChecked( TRUE );
    mpit_start_time_CheckBox->setText( tr( "MPIT Individual Event Start Times." ) );
    rightSideVTraceLayout->addWidget( mpit_start_time_CheckBox );
    }

    if (isInCurrentModifierList("stop_time")) {
       mpit_stop_time = TRUE;
    } else {
       mpit_stop_time = FALSE;
    }

    { // mpit_stop_time
    mpit_stop_time_CheckBox = new QCheckBox( VTraceGroupBox, "mpit_stop_time_CheckBox" );
    mpit_stop_time_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_stop_time_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_stop_time_CheckBox->setChecked( TRUE );
    mpit_stop_time_CheckBox->setText( tr( "MPIT Individual Event Stop Times." ) );
    rightSideVTraceLayout->addWidget( mpit_stop_time_CheckBox );
    }

    if (isInCurrentModifierList("source")) {
       mpit_source = TRUE;
    } else {
       mpit_source = FALSE;
    }

    { // mpit_source
    mpit_source_CheckBox = new QCheckBox( VTraceGroupBox, "mpit_source_CheckBox" );
    mpit_source_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_source_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_source_CheckBox->setChecked( TRUE );
    mpit_source_CheckBox->setText( tr( "MPIT Source Rank Numbers." ) );
    rightSideVTraceLayout->addWidget( mpit_source_CheckBox );
    }

    if (isInCurrentModifierList("dest")) {
       mpit_dest = TRUE;
    } else {
       mpit_dest = FALSE;
    }

    { // mpit_dest
    mpit_dest_CheckBox = new QCheckBox( VTraceGroupBox, "mpit_dest_CheckBox" );
    mpit_dest_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_dest_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_dest_CheckBox->setChecked( TRUE );
    mpit_dest_CheckBox->setText( tr( "MPIT Destination Rank Numbers." ) );
    rightSideVTraceLayout->addWidget( mpit_dest_CheckBox );
    }

    if (isInCurrentModifierList("tag")) {
       mpit_tag = TRUE;
    } else {
       mpit_tag = FALSE;
    }

    { // mpit_tag
    mpit_tag_CheckBox = new QCheckBox( VTraceGroupBox, "mpit_tag_CheckBox" );
    mpit_tag_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_tag_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_tag_CheckBox->setChecked( TRUE );
    mpit_tag_CheckBox->setText( tr( "MPIT Message Tag Values." ) );
    rightSideVTraceLayout->addWidget( mpit_tag_CheckBox );
    }

    if (isInCurrentModifierList("communicator")) {
       mpit_communicator = TRUE;
    } else {
       mpit_communicator = FALSE;
    }

    { // mpit_communicator
    mpit_communicator_CheckBox = new QCheckBox( VTraceGroupBox, "mpit_communicator" );
    mpit_communicator_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_communicator_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_communicator_CheckBox->setChecked( TRUE );
    mpit_communicator_CheckBox->setText( tr( "MPIT Communicator Used Values." ) );
    rightSideVTraceLayout->addWidget( mpit_communicator_CheckBox );
    }

    if (isInCurrentModifierList("datatype")) {
       mpit_datatype = TRUE;
    } else {
       mpit_datatype = FALSE;
    }

    { // mpit_datatype
    mpit_datatype_CheckBox = new QCheckBox( VTraceGroupBox, "mpit_datatype" );
    mpit_datatype_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_datatype_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_datatype_CheckBox->setChecked( TRUE );
    mpit_datatype_CheckBox->setText( tr( "MPIT Message Data Type Values." ) );
    rightSideVTraceLayout->addWidget( mpit_datatype_CheckBox );
    }

    if (isInCurrentModifierList("size")) {
       mpit_size = TRUE;
    } else {
       mpit_size = FALSE;
    }

    { // mpit_size
    mpit_size_CheckBox = new QCheckBox( VTraceGroupBox, "mpit_size" );
    mpit_size_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_size_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_size_CheckBox->setChecked( TRUE );
    mpit_size_CheckBox->setText( tr( "MPIT Message Size Values." ) );
    rightSideVTraceLayout->addWidget( mpit_size_CheckBox );
    }

    if (isInCurrentModifierList("retval")) {
       mpit_retval = TRUE;
    } else {
       mpit_retval = FALSE;
    }

    { // mpit_retval
    mpit_retval_CheckBox = new QCheckBox( VTraceGroupBox, "mpit_retval" );
    mpit_retval_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpit_retval_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpit_retval_CheckBox->setChecked( TRUE );
    mpit_retval_CheckBox->setText( tr( "MPIT Function Dependent Return Values." ) );
    rightSideVTraceLayout->addWidget( mpit_retval_CheckBox );
    }

   } else if ( globalCollectorString.contains("mpi") ) {
    VTraceGroupBox->hide();

    if (isInCurrentModifierList("mpit::exclusive_times")) {
       mpit_exclusive_times = TRUE;
    } else {
       mpit_exclusive_times = FALSE;
    }

    { // mpi_exclusive_times
    mpi_exclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "mpi_exclusive_times_CheckBox" );
    mpi_exclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpi_exclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpi_exclusive_times_CheckBox->setChecked( TRUE );
    mpi_exclusive_times_CheckBox->setText( tr( "MPI Exclusive Times Value" ) );
    rightSideLayout->addWidget( mpi_exclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("mpi::inclusive_times")) {
       mpi_inclusive_times = TRUE;
    } else {
       mpi_inclusive_times = FALSE;
    }

    { // mpi_inclusive_times
    mpi_inclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "mpi_inclusive_times_CheckBox" );
    mpi_inclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpi_inclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpi_inclusive_times_CheckBox->setChecked( TRUE );
    mpi_inclusive_times_CheckBox->setText( tr( "MPI Inclusive Times Value" ) );
    rightSideLayout->addWidget( mpi_inclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("min")) {
       mpi_min = TRUE;
    } else {
       mpi_min = FALSE;
    }

    { // mpi_min
    mpi_min_CheckBox = new QCheckBox( GeneralGroupBox, "mpi_min_CheckBox" );
    mpi_min_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpi_min_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpi_min_CheckBox->setChecked( TRUE );
    mpi_min_CheckBox->setText( tr( "MPI Minimum Value" ) );
    rightSideLayout->addWidget( mpi_min_CheckBox );
    }

    if (isInCurrentModifierList("max")) {
       mpi_max = TRUE;
    } else {
       mpi_max = FALSE;
    }

    { // mpi_max
    mpi_max_CheckBox = new QCheckBox( GeneralGroupBox, "mpi_max_CheckBox" );
    mpi_max_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpi_max_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpi_max_CheckBox->setChecked( TRUE );
    mpi_max_CheckBox->setText( tr( "MPI Maximum Value" ) );
    rightSideLayout->addWidget( mpi_max_CheckBox );
    }

    if (isInCurrentModifierList("average")) {
       mpi_average = TRUE;
    } else {
       mpi_average = FALSE;
    }

    { // mpi_average
    mpi_average_CheckBox = new QCheckBox( GeneralGroupBox, "mpi_average_CheckBox" );
    mpi_average_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpi_average_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpi_average_CheckBox->setChecked( TRUE );
    mpi_average_CheckBox->setText( tr( "MPI Average Value" ) );
    rightSideLayout->addWidget( mpi_average_CheckBox );
    }

    if (isInCurrentModifierList("count")) {
       mpi_count = TRUE;
    } else {
       mpi_count = FALSE;
    }

    { // mpi_count
    mpi_count_CheckBox = new QCheckBox( GeneralGroupBox, "mpi_count_CheckBox" );
    mpi_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpi_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpi_count_CheckBox->setChecked( TRUE );
    mpi_count_CheckBox->setText( tr( "MPI Count" ) );
    rightSideLayout->addWidget( mpi_count_CheckBox );
    }

    if (isInCurrentModifierList("percent")) {
       mpi_percent = TRUE;
    } else {
       mpi_percent = FALSE;
    }

    { // mpi_percent
    mpi_percent_CheckBox = new QCheckBox( GeneralGroupBox, "mpi_percent_CheckBox" );
    mpi_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpi_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpi_percent_CheckBox->setChecked( TRUE );
    mpi_percent_CheckBox->setText( tr( "MPI Percent" ) );
    rightSideLayout->addWidget( mpi_percent_CheckBox );
    }

    if (isInCurrentModifierList("stddev")) {
       mpi_stddev = TRUE;
    } else {
       mpi_stddev = FALSE;
    }

    { // mpi_stddev
    mpi_stddev_CheckBox = new QCheckBox( GeneralGroupBox, "mpi_stddev_CheckBox" );
    mpi_stddev_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mpi_stddev_CheckBox->sizePolicy().hasHeightForWidth() ) );
    mpi_stddev_CheckBox->setChecked( TRUE );
    mpi_stddev_CheckBox->setText( tr( "MPI Standard Deviation" ) );
    rightSideLayout->addWidget( mpi_stddev_CheckBox );
    }

   } else if (globalCollectorString.contains("pthreads") ) {
    VTraceGroupBox->hide();

    if (isInCurrentModifierList("pthreads::exclusive_times")) {
       pthreads_exclusive_times = TRUE;
    } else {
       pthreads_exclusive_times = FALSE;
    }
    { // pthreads_exclusive_times
    pthreads_exclusive_times_CheckBox = new QCheckBox( GeneralGroupBox, "pthreads_exclusive_times_CheckBox" );
    pthreads_exclusive_times_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pthreads_exclusive_times_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pthreads_exclusive_times_CheckBox->setChecked( TRUE );
    pthreads_exclusive_times_CheckBox->setText( tr( "PTHREADS Exclusive Times Value" ) );
    rightSideLayout->addWidget( pthreads_exclusive_times_CheckBox );
    }

    if (isInCurrentModifierList("min")) {
       pthreads_min = TRUE;
    } else {
       pthreads_min = FALSE;
    }
    { // pthreads_min
    pthreads_min_CheckBox = new QCheckBox( GeneralGroupBox, "pthreads_min_CheckBox" );
    pthreads_min_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pthreads_min_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pthreads_min_CheckBox->setChecked( TRUE );
    pthreads_min_CheckBox->setText( tr( "PTHREADS Minimum Value" ) );
    rightSideLayout->addWidget( pthreads_min_CheckBox );
    }

    if (isInCurrentModifierList("max")) {
       pthreads_max = TRUE;
    } else {
       pthreads_max = FALSE;
    }

    { // pthreads_max
    pthreads_max_CheckBox = new QCheckBox( GeneralGroupBox, "pthreads_max_CheckBox" );
    pthreads_max_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pthreads_max_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pthreads_max_CheckBox->setChecked( TRUE );
    pthreads_max_CheckBox->setText( tr( "PTHREADS Maximum Value" ) );
    rightSideLayout->addWidget( pthreads_max_CheckBox );
    }

    if (isInCurrentModifierList("average")) {
       pthreads_average = TRUE;
    } else {
       pthreads_average = FALSE;
    }

    { // pthreads_average
    pthreads_average_CheckBox = new QCheckBox( GeneralGroupBox, "pthreads_average_CheckBox" );
    pthreads_average_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pthreads_average_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pthreads_average_CheckBox->setChecked( TRUE );
    pthreads_average_CheckBox->setText( tr( "PTHREADS Average Value" ) );
    rightSideLayout->addWidget( pthreads_average_CheckBox );
    }

    if (isInCurrentModifierList("count")) {
       pthreads_count = TRUE;
    } else {
       pthreads_count = FALSE;
    }

    { // pthreads_count
    pthreads_count_CheckBox = new QCheckBox( GeneralGroupBox, "pthreads_count_CheckBox" );
    pthreads_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pthreads_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pthreads_count_CheckBox->setChecked( TRUE );
    pthreads_count_CheckBox->setText( tr( "PTHREADS Count" ) );
    rightSideLayout->addWidget( pthreads_count_CheckBox );
    }

    if (isInCurrentModifierList("percent")) {
       pthreads_percent = TRUE;
    } else {
       pthreads_percent = FALSE;
    }

    { // pthreads_percent
    pthreads_percent_CheckBox = new QCheckBox( GeneralGroupBox, "pthreads_percent_CheckBox" );
    pthreads_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pthreads_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pthreads_percent_CheckBox->setChecked( TRUE );
    pthreads_percent_CheckBox->setText( tr( "PTHREADS Percent" ) );
    rightSideLayout->addWidget( pthreads_percent_CheckBox );
    }

    if (isInCurrentModifierList("stddev")) {
       pthreads_stddev = TRUE;
    } else {
       pthreads_stddev = FALSE;
    }

    { // pthreads_stddev
    pthreads_stddev_CheckBox = new QCheckBox( GeneralGroupBox, "pthreads_stddev_CheckBox" );
    pthreads_stddev_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pthreads_stddev_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pthreads_stddev_CheckBox->setChecked( TRUE );
    pthreads_stddev_CheckBox->setText( tr( "PTHREADS Standard Deviatmemn" ) );
    rightSideLayout->addWidget( pthreads_stddev_CheckBox );
    }

    if (isInCurrentModifierList("ThreadAverage")) {
       pthreads_ThreadAverage = TRUE;
    } else {
       pthreads_ThreadAverage = FALSE;
    }

    { // pthreads_ThreadAverage
    pthreads_ThreadAverage_CheckBox = new QCheckBox( GeneralGroupBox, "pthreads_ThreadAverage_CheckBox" );
    pthreads_ThreadAverage_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pthreads_ThreadAverage_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pthreads_ThreadAverage_CheckBox->setChecked( TRUE );
    pthreads_ThreadAverage_CheckBox->setText( tr( "PTHREADS Thread Average Value" ) );
    rightSideLayout->addWidget( pthreads_ThreadAverage_CheckBox );
    }

    if (isInCurrentModifierList("ThreadMin")) {
       pthreads_ThreadMin = TRUE;
    } else {
       pthreads_ThreadMin = FALSE;
    }

    { // pthreads_ThreadMin
    pthreads_ThreadMin_CheckBox = new QCheckBox( GeneralGroupBox, "pthreads_ThreadMin_CheckBox" );
    pthreads_ThreadMin_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pthreads_ThreadMin_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pthreads_ThreadMin_CheckBox->setChecked( TRUE );
    pthreads_ThreadMin_CheckBox->setText( tr( "PTHREADS Thread Minimum Value" ) );
    rightSideLayout->addWidget( pthreads_ThreadMin_CheckBox );
    }

    if (isInCurrentModifierList("ThreadMax")) {
       pthreads_ThreadMax = TRUE;
    } else {
       pthreads_ThreadMax = FALSE;
    }

    { // pthreads_ThreadMax
    pthreads_ThreadMax_CheckBox = new QCheckBox( GeneralGroupBox, "pthreads_ThreadMax_CheckBox" );
    pthreads_ThreadMax_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, pthreads_ThreadMax_CheckBox->sizePolicy().hasHeightForWidth() ) );
    pthreads_ThreadMax_CheckBox->setChecked( TRUE );
    pthreads_ThreadMax_CheckBox->setText( tr( "PTHREADS Thread Maximum Value" ) );
    rightSideLayout->addWidget( pthreads_ThreadMax_CheckBox );
    }

   } else if ( globalCollectorString.contains("fpe") ) {
    VTraceGroupBox->hide();

#if 0
    if (isInCurrentModifierList("fpe::time")) {
       fpe_time = TRUE;
    } else {
       fpe_time = FALSE;
    }

    { // fpe_time
    fpe_time_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_time_CheckBox" );
    fpe_time_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_time_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_time_CheckBox->setChecked( TRUE );
    fpe_time_CheckBox->setText( tr( "FPE Experiment Time Value" ) );
    rightSideLayout->addWidget( fpe_time_CheckBox );
    }
#endif

    if (isInCurrentModifierList("fpe::counts")) {
       fpe_counts = TRUE;
    } else {
       fpe_counts = FALSE;
    }

    { // fpe_counts
    fpe_counts_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_counts_CheckBox" );
    fpe_counts_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_counts_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_counts_CheckBox->setChecked( TRUE );
    fpe_counts_CheckBox->setText( tr( "FPE Experiment Exclusive Counts Value" ) );
    rightSideLayout->addWidget( fpe_counts_CheckBox );
    }

    if (isInCurrentModifierList("fpe::inclusive_counts")) {
       fpe_inclusive_counts = TRUE;
    } else {
       fpe_inclusive_counts = FALSE;
    }

    { // fpe_inclusive_counts
    fpe_inclusive_counts_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_inclusive_counts_CheckBox" );
    fpe_inclusive_counts_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_inclusive_counts_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_inclusive_counts_CheckBox->setChecked( TRUE );
    fpe_inclusive_counts_CheckBox->setText( tr( "FPE Experiment Inclusive Counts Value" ) );
    rightSideLayout->addWidget( fpe_inclusive_counts_CheckBox );
    }


    if (isInCurrentModifierList("fpe::percent")) {
       fpe_percent = TRUE;
    } else {
       fpe_percent = FALSE;
    }

    { // fpe_percent
    fpe_percent_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_percent_CheckBox" );
    fpe_percent_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_percent_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_percent_CheckBox->setChecked( TRUE );
    fpe_percent_CheckBox->setText( tr( "FPE Experiment Percent Value" ) );
    rightSideLayout->addWidget( fpe_percent_CheckBox );
    }

    if (isInCurrentModifierList("fpe::ThreadAverage")) {
       fpe_ThreadAverage = TRUE;
    } else {
       fpe_ThreadAverage = FALSE;
    }

    { // fpe_ThreadAverage
    fpe_ThreadAverage_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_ThreadAverage_CheckBox" );
    fpe_ThreadAverage_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_ThreadAverage_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_ThreadAverage_CheckBox->setChecked( TRUE );
    fpe_ThreadAverage_CheckBox->setText( tr( "FPE Experiment ThreadAverage Value" ) );
    rightSideLayout->addWidget( fpe_ThreadAverage_CheckBox );
    }

    if (isInCurrentModifierList("fpe::ThreadMin")) {
       fpe_ThreadMin = TRUE;
    } else {
       fpe_ThreadMin = FALSE;
    }

    { // fpe_ThreadMin
    fpe_ThreadMin_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_ThreadMin_CheckBox" );
    fpe_ThreadMin_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_ThreadMin_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_ThreadMin_CheckBox->setChecked( TRUE );
    fpe_ThreadMin_CheckBox->setText( tr( "FPE Experiment Thread Minimum Value" ) );
    rightSideLayout->addWidget( fpe_ThreadMin_CheckBox );
    }

    if (isInCurrentModifierList("fpe::ThreadMax")) {
       fpe_ThreadMax = TRUE;
    } else {
       fpe_ThreadMax = FALSE;
    }

    { // fpe_ThreadMax
    fpe_ThreadMax_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_ThreadMax_CheckBox" );
    fpe_ThreadMax_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_ThreadMax_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_ThreadMax_CheckBox->setChecked( TRUE );
    fpe_ThreadMax_CheckBox->setText( tr( "FPE Experiment Thread Maximum Value" ) );
    rightSideLayout->addWidget( fpe_ThreadMax_CheckBox );
    }

    if (isInCurrentModifierList("fpe::inexact_result_count")) {
       fpe_inexact_result_count= TRUE;
    } else {
       fpe_inexact_result_count= FALSE;
    }

    { // fpe_inexact_result_count
    fpe_inexact_result_count_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_inexact_result_count_CheckBox" );
    fpe_inexact_result_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_inexact_result_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_inexact_result_count_CheckBox->setChecked( TRUE );
    fpe_inexact_result_count_CheckBox->setText( tr( "FPE Experiment Inexact Result Count Value" ) );
    rightSideLayout->addWidget( fpe_inexact_result_count_CheckBox );
    }

    if (isInCurrentModifierList("fpe::underflow_count")) {
       fpe_underflow_count= TRUE;
    } else {
       fpe_underflow_count= FALSE;
    }

    { // fpe_underflow_count
    fpe_underflow_count_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_underflow_count_CheckBox" );
    fpe_underflow_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_underflow_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_underflow_count_CheckBox->setChecked( TRUE );
    fpe_underflow_count_CheckBox->setText( tr( "FPE Experiment Underflow Count Value" ) );
    rightSideLayout->addWidget( fpe_underflow_count_CheckBox );
    }

    if (isInCurrentModifierList("fpe::overflow_count")) {
       fpe_overflow_count= TRUE;
    } else {
       fpe_overflow_count= FALSE;
    }

    { // fpe_overflow_count
    fpe_overflow_count_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_overflow_count_CheckBox" );
    fpe_overflow_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_overflow_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_overflow_count_CheckBox->setChecked( TRUE );
    fpe_overflow_count_CheckBox->setText( tr( "FPE Experiment Overflow Count Value" ) );
    rightSideLayout->addWidget( fpe_overflow_count_CheckBox );
    }

    if (isInCurrentModifierList("fpe::division_by_zero_count")) {
       fpe_division_by_zero_count= TRUE;
    } else {
       fpe_division_by_zero_count= FALSE;
    }

    { // fpe_division_by_zero_count
    fpe_division_by_zero_count_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_division_by_zero_count_CheckBox" );
    fpe_division_by_zero_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_division_by_zero_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_division_by_zero_count_CheckBox->setChecked( TRUE );
    fpe_division_by_zero_count_CheckBox->setText( tr( "FPE Experiment Overflow Count Value" ) );
    rightSideLayout->addWidget( fpe_division_by_zero_count_CheckBox );
    }

    if (isInCurrentModifierList("fpe::unnormal_count")) {
       fpe_unnormal_count= TRUE;
    } else {
       fpe_unnormal_count= FALSE;
    }

    { // fpe_unnormal_count
    fpe_unnormal_count_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_unnormal_count_CheckBox" );
    fpe_unnormal_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_unnormal_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_unnormal_count_CheckBox->setChecked( TRUE );
    fpe_unnormal_count_CheckBox->setText( tr( "FPE Experiment Unnormal Count Value" ) );
    rightSideLayout->addWidget( fpe_unnormal_count_CheckBox );
    }

    if (isInCurrentModifierList("fpe::invalid_count")) {
       fpe_invalid_count= TRUE;
    } else {
       fpe_invalid_count= FALSE;
    }

    { // fpe_invalid_count
    fpe_invalid_count_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_invalid_count_CheckBox" );
    fpe_invalid_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_invalid_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_invalid_count_CheckBox->setChecked( TRUE );
    fpe_invalid_count_CheckBox->setText( tr( "FPE Experiment Invalid Count Value" ) );
    rightSideLayout->addWidget( fpe_invalid_count_CheckBox );
    }

    if (isInCurrentModifierList("fpe::unknown_count")) {
       fpe_unknown_count= TRUE;
    } else {
       fpe_unknown_count= FALSE;
    }

    { // fpe_unknown_count
    fpe_unknown_count_CheckBox = new QCheckBox( GeneralGroupBox, "fpe_unknown_count_CheckBox" );
    fpe_unknown_count_CheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, fpe_unknown_count_CheckBox->sizePolicy().hasHeightForWidth() ) );
    fpe_unknown_count_CheckBox->setChecked( TRUE );
    fpe_unknown_count_CheckBox->setText( tr( "FPE Experiment Unknown Count Value" ) );
    rightSideLayout->addWidget( fpe_unknown_count_CheckBox );
    }

   } else {
    VTraceGroupBox->hide();
   }

//    GeneralGroupBox->insertChild( VTraceGroupBox );
    generalStackPageLayout->addWidget( GeneralGroupBox );
    generalStackPageLayout->addWidget( VTraceGroupBox );
    stack->addWidget( generalStackPage, 0 );
}


/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void OptionalViewsDialog::languageChange()
{
#if DEBUG_optional
   printf("OptionalViewsDialog::languageChange, entered\n");
#endif

   if( globalCollectorString.contains("pcsamp") ) {

    pcsamp_time_CheckBox->setChecked(pcsamp_time);
    QToolTip::add(pcsamp_time_CheckBox,
                tr("Display Program Counter Sampling experiment exclusive time.") );

    pcsamp_percent_CheckBox->setChecked(pcsamp_percent);
    QToolTip::add(pcsamp_percent_CheckBox,
                tr("Display Program Counter Sampling experiment percentage of exclusive time.") );

    pcsamp_ThreadAverage_CheckBox->setChecked(pcsamp_ThreadAverage);
    QToolTip::add(pcsamp_ThreadAverage_CheckBox,
                tr("Display Program Counter Sampling experiment average of exclusive time across ranks, processes, threads.") );

    pcsamp_ThreadMin_CheckBox->setChecked(pcsamp_ThreadMin);
    QToolTip::add(pcsamp_ThreadMin_CheckBox,
                tr("Display Program Counter Sampling experiment minimum value of exclusive time across ranks, processes, threads.") );

    pcsamp_ThreadMax_CheckBox->setChecked(pcsamp_ThreadMax);
    QToolTip::add(pcsamp_ThreadMax_CheckBox,
                tr("Display Program Counter Sampling experiment maximum value of exclusive time across ranks, processes, threads.") );

   } else if ( globalCollectorString.contains("omptp") ) {

    omptp_exclusive_times_CheckBox->setChecked(omptp_exclusive_times);
    QToolTip::add(omptp_exclusive_times_CheckBox,
                tr("Display OpenMP Profile (Call Stack) Sampling experiment exclusive time.") );

    omptp_inclusive_times_CheckBox->setChecked(omptp_inclusive_times);
    QToolTip::add(omptp_inclusive_times_CheckBox,
                tr("Display OpenMP Profile (Call Stack) Sampling experiment inclusive time.") );

    omptp_percent_CheckBox->setChecked(omptp_percent);
    QToolTip::add(omptp_percent_CheckBox,
                tr("Display OpenMP Profile (Call Stack)  Sampling experiment percentage of exclusive time.") );

    omptp_count_CheckBox->setChecked(omptp_count);
    QToolTip::add(omptp_count_CheckBox,
                tr("Display OpenMP Profile (Call Stack)  Sampling experiment counts.") );

    omptp_ThreadAverage_CheckBox->setChecked(omptp_ThreadAverage);
    QToolTip::add(omptp_ThreadAverage_CheckBox,
                tr("Display OpenMP Profile (Call Stack) Sampling experiment average of exclusive time across ranks, processes, threads.") );

    omptp_ThreadMin_CheckBox->setChecked(omptp_ThreadMin);
    QToolTip::add(omptp_ThreadMin_CheckBox,
                tr("Display OpenMP Profile (Call Stack) Sampling experiment minimum value of exclusive time across ranks, processes, threads.") );

    omptp_ThreadMax_CheckBox->setChecked(omptp_ThreadMax);
    QToolTip::add(omptp_ThreadMax_CheckBox,
                tr("Display OpenMP Profile (Call Stack) Sampling experiment maximum value of exclusive time across ranks, processes, threads.") );

   } else if ( globalCollectorString.contains("usertime") ) {

    usertime_exclusive_times_CheckBox->setChecked(usertime_exclusive_times);
    QToolTip::add(usertime_exclusive_times_CheckBox,
                tr("Display Usertime (Call Stack) Sampling experiment exclusive time.") );

    usertime_inclusive_times_CheckBox->setChecked(usertime_inclusive_times);
    QToolTip::add(usertime_inclusive_times_CheckBox,
                tr("Display Usertime (Call Stack) Sampling experiment inclusive time.") );

    usertime_percent_CheckBox->setChecked(usertime_percent);
    QToolTip::add(usertime_percent_CheckBox,
                tr("Display Usertime (Call Stack)  Sampling experiment percentage of exclusive time.") );

    usertime_count_CheckBox->setChecked(usertime_count);
    QToolTip::add(usertime_count_CheckBox,
                tr("Display Usertime (Call Stack)  Sampling experiment counts.") );

    usertime_ThreadAverage_CheckBox->setChecked(usertime_ThreadAverage);
    QToolTip::add(usertime_ThreadAverage_CheckBox,
                tr("Display Usertime (Call Stack) Sampling experiment average of exclusive time across ranks, processes, threads.") );

    usertime_ThreadMin_CheckBox->setChecked(usertime_ThreadMin);
    QToolTip::add(usertime_ThreadMin_CheckBox,
                tr("Display Usertime (Call Stack) Sampling experiment minimum value of exclusive time across ranks, processes, threads.") );

    usertime_ThreadMax_CheckBox->setChecked(usertime_ThreadMax);
    QToolTip::add(usertime_ThreadMax_CheckBox,
                tr("Display Usertime (Call Stack) Sampling experiment maximum value of exclusive time across ranks, processes, threads.") );

   } else if ( globalCollectorString.contains("hwcsamp") ) {
     hwcsamp_percent = hwcsamp_percent_CheckBox->isChecked();
     hwcsamp_ThreadAverage = hwcsamp_ThreadAverage_CheckBox->isChecked();
     hwcsamp_ThreadMin = hwcsamp_ThreadMin_CheckBox->isChecked();
     hwcsamp_ThreadMax = hwcsamp_ThreadMax_CheckBox->isChecked();

    hwcsamp_time_CheckBox->setChecked(hwcsamp_time);
    QToolTip::add(hwcsamp_time_CheckBox,
                tr("Display Hardware Counter Sampling experiment exclusive time.") );

    hwcsamp_allEvents_CheckBox->setChecked(hwcsamp_allEvents);
    QToolTip::add(hwcsamp_allEvents_CheckBox,
                tr("Display Hardware Counter Sampling experiment all hardware counter events.") );

    hwcsamp_percent_CheckBox->setChecked(hwcsamp_percent);
    QToolTip::add(hwcsamp_percent_CheckBox,
                tr("Display Hardware Counter Sampling experiment percent.") );

    hwcsamp_ThreadAverage_CheckBox->setChecked(hwcsamp_ThreadAverage);
    QToolTip::add(hwcsamp_ThreadAverage_CheckBox,
                tr("Display Hardware Counter Sampling experiment ThreadAverage.") );

    hwcsamp_ThreadMin_CheckBox->setChecked(hwcsamp_ThreadMin);
    QToolTip::add(hwcsamp_ThreadMin_CheckBox,
                tr("Display Hardware Counter Sampling experiment ThreadMin.") );

    hwcsamp_ThreadMax_CheckBox->setChecked(hwcsamp_ThreadMax);
    QToolTip::add(hwcsamp_ThreadMax_CheckBox,
                tr("Display Hardware Counter Sampling experiment ThreadMax.") );

   } else if ( globalCollectorString.contains("hwctime") ) {
     hwctime_percent = hwctime_percent_CheckBox->isChecked();
     hwctime_ThreadAverage = hwctime_ThreadAverage_CheckBox->isChecked();
     hwctime_ThreadMin = hwctime_ThreadMin_CheckBox->isChecked();
     hwctime_ThreadMax = hwctime_ThreadMax_CheckBox->isChecked();

    hwctime_exclusive_counts_CheckBox->setChecked(hwctime_exclusive_counts);
    QToolTip::add(hwctime_exclusive_counts_CheckBox,
                tr("Display Hardware Counter Time experiment exclusive counts.") );

    hwctime_exclusive_overflows_CheckBox->setChecked(hwctime_exclusive_overflows);
    QToolTip::add(hwctime_exclusive_overflows_CheckBox,
                tr("Display Hardware Counter Time experiment exclusive overflows.") );

    hwctime_inclusive_overflows_CheckBox->setChecked(hwctime_inclusive_overflows);
    QToolTip::add(hwctime_inclusive_overflows_CheckBox,
                tr("Display Hardware Counter Time experiment inclusive overflows.") );

    hwctime_inclusive_counts_CheckBox->setChecked(hwctime_inclusive_counts);
    QToolTip::add(hwctime_inclusive_counts_CheckBox,
                tr("Display Hardware Counter Time experiment inclusive counts.") );

    hwctime_percent_CheckBox->setChecked(hwctime_percent);
    QToolTip::add(hwctime_percent_CheckBox,
                tr("Display Hardware Counter Time experiment percent.") );

    hwctime_ThreadAverage_CheckBox->setChecked(hwctime_ThreadAverage);
    QToolTip::add(hwctime_ThreadAverage_CheckBox,
                tr("Display Hardware Counter Time experiment ThreadAverage.") );

    hwctime_ThreadMin_CheckBox->setChecked(hwctime_ThreadMin);
    QToolTip::add(hwctime_ThreadMin_CheckBox,
                tr("Display Hardware Counter Time experiment ThreadMin.") );

    hwctime_ThreadMax_CheckBox->setChecked(hwctime_ThreadMax);
    QToolTip::add(hwctime_ThreadMax_CheckBox,
                tr("Display Hardware Counter Time experiment ThreadMax.") );

   } else if ( globalCollectorString.contains("hwc") ) {

    hwc_overflows_CheckBox->setChecked(hwc_overflows);
    QToolTip::add(hwc_overflows_CheckBox,
                tr("Display Hardware Counter experiment overflows.") );

    hwc_counts_CheckBox->setChecked(hwc_counts);
    QToolTip::add(hwc_counts_CheckBox,
                tr("Display Hardware Counter experiment counts.") );

    hwc_percent_CheckBox->setChecked(hwc_percent);
    QToolTip::add(hwc_percent_CheckBox,
                tr("Display Hardware Counter experiment percent.") );

    hwc_ThreadAverage_CheckBox->setChecked(hwc_ThreadAverage);
    QToolTip::add(hwc_ThreadAverage_CheckBox,
                tr("Display Hardware Counter experiment ThreadAverage.") );

    hwc_ThreadMin_CheckBox->setChecked(hwc_ThreadMin);
    QToolTip::add(hwc_ThreadMin_CheckBox,
                tr("Display Hardware Counter experiment ThreadMin.") );

    hwc_ThreadMax_CheckBox->setChecked(hwc_ThreadMax);
    QToolTip::add(hwc_ThreadMax_CheckBox,
                tr("Display Hardware Counter experiment ThreadMax.") );

   } else if ( globalCollectorString.contains("iot") ) {

#if DEBUG_optional
    printf("OptionalViewsDialog::languageChange, setting iot_exclusive_times_CheckBox to iot_exclusive_times_CheckBox->isChecked()=%d\n",
           iot_exclusive_times_CheckBox->isChecked());
#endif
    iot_exclusive_times_CheckBox->setChecked(iot_exclusive_times);
    QToolTip::add(iot_exclusive_times_CheckBox,
                tr("Display I/O trace experiment exclusive time.") );

    iot_inclusive_times_CheckBox->setChecked(iot_inclusive_times);
    QToolTip::add(iot_inclusive_times_CheckBox,
                tr("Display I/O trace experiment inclusive time.") );

    iot_min_CheckBox->setChecked(iot_min);
    QToolTip::add(iot_min_CheckBox,
                tr("Display I/O trace experiment minimum value.") );

    iot_max_CheckBox->setChecked(iot_max);
    QToolTip::add(iot_max_CheckBox,
                tr("Display I/O trace experiment maximum value.") );

    iot_average_CheckBox->setChecked(iot_average);
    QToolTip::add(iot_average_CheckBox,
                tr("Display I/O trace experiment average value.") );

    iot_count_CheckBox->setChecked(iot_count);
    QToolTip::add(iot_count_CheckBox,
                tr("Display I/O trace experiment count value.") );

    iot_percent_CheckBox->setChecked(iot_percent);
    QToolTip::add(iot_percent_CheckBox,
                tr("Display I/O trace experiment percent value.") );

    iot_stddev_CheckBox->setChecked(iot_stddev);
    QToolTip::add(iot_stddev_CheckBox,
                tr("Display I/O trace experiment stddev value.") );

    iot_start_time_CheckBox->setChecked(iot_start_time);
    QToolTip::add(iot_start_time_CheckBox,
                tr("Display I/O trace experiment Start Time value.") );

    iot_stop_time_CheckBox->setChecked(iot_stop_time);
    QToolTip::add(iot_stop_time_CheckBox,
                tr("Display I/O trace experiment Stop Time value.") );

    iot_syscallno_CheckBox->setChecked(iot_syscallno);
    QToolTip::add(iot_syscallno_CheckBox,
                tr("Display I/O trace experiment Syscall Number value.") );

    iot_nsysargs_CheckBox->setChecked(iot_nsysargs);
    QToolTip::add(iot_nsysargs_CheckBox,
                tr("Display I/O trace experiment Number of Syscall Arguments value.") );

    iot_retval_CheckBox->setChecked(iot_retval);
    QToolTip::add(iot_retval_CheckBox,
                tr("Display I/O trace experiment Syscall Dependent Return Value value.") );

#if PATHNAME_READY
    iot_pathname_CheckBox->setChecked(iot_pathname);
    QToolTip::add(iot_pathname_CheckBox,
                tr("Display I/O trace experiment Pathname value.") );
#endif
   } else if ( globalCollectorString.contains("iop") ) {

    iop_exclusive_times_CheckBox->setChecked(iop_exclusive_times);
    QToolTip::add(iop_exclusive_times_CheckBox,
                tr("Display Light-weight I/O experiment exclusive time.") );

    iop_inclusive_times_CheckBox->setChecked(iop_inclusive_times);
    QToolTip::add(iop_inclusive_times_CheckBox,
                tr("Display Light-weight I/O experiment inclusive time.") );

    iop_percent_CheckBox->setChecked(iop_percent);
    QToolTip::add(iop_percent_CheckBox,
                tr("Display Light-weight I/O experiment percentage of exclusive time.") );

    iop_count_CheckBox->setChecked(iop_count);
    QToolTip::add(iop_count_CheckBox,
                tr("Display Light-weight I/O experiment counts.") );

    iop_ThreadAverage_CheckBox->setChecked(iop_ThreadAverage);
    QToolTip::add(iop_ThreadAverage_CheckBox,
                tr("Display Light-weight I/O experiment average of exclusive time across ranks, processes, threads.") );

    iop_ThreadMin_CheckBox->setChecked(iop_ThreadMin);
    QToolTip::add(iop_ThreadMin_CheckBox,
                tr("Display Light-weight I/O experiment minimum value of exclusive time across ranks, processes, threads.") );

    iop_ThreadMax_CheckBox->setChecked(iop_ThreadMax);
    QToolTip::add(iop_ThreadMax_CheckBox,
                tr("Display Light-weight I/O experiment maximum value of exclusive time across ranks, processes, threads.") );

   } else if ( globalCollectorString.contains("mpip") ) {

    mpip_exclusive_times_CheckBox->setChecked(mpip_exclusive_times);
    QToolTip::add(mpip_exclusive_times_CheckBox,
                tr("Display Light-weight MPI experiment exclusive time.") );

    mpip_inclusive_times_CheckBox->setChecked(mpip_inclusive_times);
    QToolTip::add(mpip_inclusive_times_CheckBox,
                tr("Display Light-weight MPI experiment inclusive time.") );

    mpip_percent_CheckBox->setChecked(mpip_percent);
    QToolTip::add(mpip_percent_CheckBox,
                tr("Display Light-weight MPI experiment percentage of exclusive time.") );

    mpip_count_CheckBox->setChecked(mpip_count);
    QToolTip::add(mpip_count_CheckBox,
                tr("Display Light-weight MPI experiment counts.") );

    mpip_ThreadAverage_CheckBox->setChecked(mpip_ThreadAverage);
    QToolTip::add(mpip_ThreadAverage_CheckBox,
                tr("Display Light-weight MPI experiment average of exclusive time across ranks, processes, threads.") );

    mpip_ThreadMin_CheckBox->setChecked(mpip_ThreadMin);
    QToolTip::add(mpip_ThreadMin_CheckBox,
                tr("Display Light-weight MPI experiment minimum value of exclusive time across ranks, processes, threads.") );

    mpip_ThreadMax_CheckBox->setChecked(mpip_ThreadMax);
    QToolTip::add(mpip_ThreadMax_CheckBox,
                tr("Display Light-weight MPI experiment maximum value of exclusive time across ranks, processes, threads.") );

   } else if (globalCollectorString.contains("io") ) {

    io_exclusive_times_CheckBox->setChecked(io_exclusive_times);
    QToolTip::add(io_exclusive_times_CheckBox,
                tr("Display I/O experiment exclusive times value.") );

    io_min_CheckBox->setChecked(io_min);
    QToolTip::add(io_min_CheckBox,
                tr("Display I/O experiment minimum value.") );

    io_max_CheckBox->setChecked(io_max);
    QToolTip::add(io_max_CheckBox,
                tr("Display I/O experiment maximum value.") );

    io_average_CheckBox->setChecked(io_average);
    QToolTip::add(io_average_CheckBox,
                tr("Display I/O experiment average value.") );

    io_count_CheckBox->setChecked(io_count);
    QToolTip::add(io_count_CheckBox,
                tr("Display I/O experiment count value.") );

    io_percent_CheckBox->setChecked(io_percent);
    QToolTip::add(io_percent_CheckBox,
                tr("Display I/O experiment percent value.") );

    io_stddev_CheckBox->setChecked(io_stddev);
    QToolTip::add(io_stddev_CheckBox,
                tr("Display I/O experiment stddev value.") );

    io_ThreadAverage_CheckBox->setChecked(io_ThreadAverage);
    QToolTip::add(io_ThreadAverage_CheckBox,
                tr("Display I/O experiment Thread Average value.") );

    io_ThreadMin_CheckBox->setChecked(io_ThreadMin);
    QToolTip::add(io_ThreadMin_CheckBox,
                tr("Display I/O experiment Thread Min value.") );

    io_ThreadMax_CheckBox->setChecked(io_ThreadMax);
    QToolTip::add(io_ThreadMax_CheckBox,
                tr("Display I/O experiment Thread Max value.") );

   } else if (globalCollectorString.contains("mem") ) {

    mem_exclusive_times_CheckBox->setChecked(mem_exclusive_times);
    QToolTip::add(mem_exclusive_times_CheckBox,
                tr("Display MEM experiment exclusive times value.") );

    mem_min_CheckBox->setChecked(mem_min);
    QToolTip::add(mem_min_CheckBox,
                tr("Display MEM experiment minimum value.") );

    mem_max_CheckBox->setChecked(mem_max);
    QToolTip::add(mem_max_CheckBox,
                tr("Display MEM experiment maximum value.") );

    mem_average_CheckBox->setChecked(mem_average);
    QToolTip::add(mem_average_CheckBox,
                tr("Display MEM experiment average value.") );

    mem_count_CheckBox->setChecked(mem_count);
    QToolTip::add(mem_count_CheckBox,
                tr("Display MEM experiment count value.") );

    mem_percent_CheckBox->setChecked(mem_percent);
    QToolTip::add(mem_percent_CheckBox,
                tr("Display MEM experiment percent value.") );

    mem_stddev_CheckBox->setChecked(mem_stddev);
    QToolTip::add(mem_stddev_CheckBox,
                tr("Display MEM experiment stddev value.") );

    mem_ThreadAverage_CheckBox->setChecked(mem_ThreadAverage);
    QToolTip::add(mem_ThreadAverage_CheckBox,
                tr("Display MEM experiment Thread Average value.") );

    mem_ThreadMin_CheckBox->setChecked(mem_ThreadMin);
    QToolTip::add(mem_ThreadMin_CheckBox,
                tr("Display MEM experiment Thread Min value.") );

    mem_ThreadMax_CheckBox->setChecked(mem_ThreadMax);
    QToolTip::add(mem_ThreadMax_CheckBox,
                tr("Display MEM experiment Thread Max value.") );

   } else if ( globalCollectorString.contains("mpit") ) {

    mpit_exclusive_times_CheckBox->setChecked(mpit_exclusive_times);
    QToolTip::add(mpit_exclusive_times_CheckBox,
                tr("Display MPI trace experiment exclusive time.") );

    mpit_inclusive_times_CheckBox->setChecked(mpit_inclusive_times);
    QToolTip::add(mpit_inclusive_times_CheckBox,
                tr("Display MPI trace experiment inclusive time.") );

    mpit_min_CheckBox->setChecked(mpit_min);
    QToolTip::add(mpit_min_CheckBox,
                tr("Display MPI trace experiment minimum values.") );

    mpit_max_CheckBox->setChecked(mpit_max);
    QToolTip::add(mpit_max_CheckBox,
                tr("Display MPI trace experiment maximum values.") );

    mpit_average_CheckBox->setChecked(mpit_average);
    QToolTip::add(mpit_average_CheckBox,
                tr("Display MPI trace experiment average values.") );

    mpit_count_CheckBox->setChecked(mpit_count);
    QToolTip::add(mpit_count_CheckBox,
                tr("Display MPI trace experiment count values.") );

    mpit_percent_CheckBox->setChecked(mpit_percent);
    QToolTip::add(mpit_percent_CheckBox,
                tr("Display MPI trace experiment percent values.") );

    mpit_stddev_CheckBox->setChecked(mpit_stddev);
    QToolTip::add(mpit_stddev_CheckBox,
                tr("Display MPI trace experiment standard deviation values.") );

    mpit_size_CheckBox->setChecked(mpit_size);
    QToolTip::add(mpit_size_CheckBox,
                tr("Display MPI trace experiment message size values.") );

    mpit_start_time_CheckBox->setChecked(mpit_start_time);
    QToolTip::add(mpit_start_time_CheckBox,
                tr("Display MPI trace experiment start time values.") );

    mpit_stop_time_CheckBox->setChecked(mpit_stop_time);
    QToolTip::add(mpit_stop_time_CheckBox,
                tr("Display MPI trace experiment stop time values.") );

    mpit_source_CheckBox->setChecked(mpit_source);
    QToolTip::add(mpit_source_CheckBox,
                tr("Display MPI trace experiment source rank values.") );

    mpit_dest_CheckBox->setChecked(mpit_dest);
    QToolTip::add(mpit_dest_CheckBox,
                tr("Display MPI trace experiment destination rank values.") );

    mpit_tag_CheckBox->setChecked(mpit_tag);
    QToolTip::add(mpit_tag_CheckBox,
                tr("Display MPI trace experiment message tag values.") );

    mpit_communicator_CheckBox->setChecked(mpit_communicator);
    QToolTip::add(mpit_communicator_CheckBox,
                tr("Display MPI trace experiment communicator used values.") );

    mpit_datatype_CheckBox->setChecked(mpit_datatype);
    QToolTip::add(mpit_datatype_CheckBox,
                tr("Display MPI trace experiment message data type values.") );

    mpit_retval_CheckBox->setChecked(mpit_retval);
    QToolTip::add(mpit_retval_CheckBox,
                tr("Display MPI trace experiment function dependent return values.") );

   } else if ( globalCollectorString.contains("mpi") ) {

    mpi_exclusive_times_CheckBox->setChecked(mpi_exclusive_times);
    QToolTip::add(mpi_exclusive_times_CheckBox,
                tr("Display MPI experiment exclusive time.") );

    mpi_inclusive_times_CheckBox->setChecked(mpi_inclusive_times);
    QToolTip::add(mpi_inclusive_times_CheckBox,
                tr("Display MPI experiment inclusive time.") );

    mpi_min_CheckBox->setChecked(mpi_min);
    QToolTip::add(mpi_min_CheckBox,
                tr("Display MPI experiment minimum values.") );

    mpi_max_CheckBox->setChecked(mpi_max);
    QToolTip::add(mpi_max_CheckBox,
                tr("Display MPI experiment maximum values.") );

    mpi_average_CheckBox->setChecked(mpi_average);
    QToolTip::add(mpi_average_CheckBox,
                tr("Display MPI experiment average values.") );

    mpi_count_CheckBox->setChecked(mpi_count);
    QToolTip::add(mpi_count_CheckBox,
                tr("Display MPI experiment count values.") );

    mpi_percent_CheckBox->setChecked(mpi_percent);
    QToolTip::add(mpi_percent_CheckBox,
                tr("Display MPI experiment percent values.") );

    mpi_stddev_CheckBox->setChecked(mpi_stddev);
    QToolTip::add(mpi_stddev_CheckBox,
                tr("Display MPI experiment standard deviation values.") );

   } else if (globalCollectorString.contains("pthreads") ) {

    pthreads_exclusive_times_CheckBox->setChecked(pthreads_exclusive_times);
    QToolTip::add(pthreads_exclusive_times_CheckBox,
                tr("Display PTHREADS experiment exclusive times value.") );

    pthreads_min_CheckBox->setChecked(pthreads_min);
    QToolTip::add(pthreads_min_CheckBox,
                tr("Display PTHREADS experiment minimum value.") );

    pthreads_max_CheckBox->setChecked(pthreads_max);
    QToolTip::add(pthreads_max_CheckBox,
                tr("Display PTHREADS experiment maximum value.") );

    pthreads_average_CheckBox->setChecked(pthreads_average);
    QToolTip::add(pthreads_average_CheckBox,
                tr("Display PTHREADS experiment average value.") );

    pthreads_count_CheckBox->setChecked(pthreads_count);
    QToolTip::add(pthreads_count_CheckBox,
                tr("Display PTHREADS experiment count value.") );

    pthreads_percent_CheckBox->setChecked(pthreads_percent);
    QToolTip::add(pthreads_percent_CheckBox,
                tr("Display PTHREADS experiment percent value.") );

    pthreads_stddev_CheckBox->setChecked(pthreads_stddev);
    QToolTip::add(pthreads_stddev_CheckBox,
                tr("Display PTHREADS experiment stddev value.") );

    pthreads_ThreadAverage_CheckBox->setChecked(pthreads_ThreadAverage);
    QToolTip::add(pthreads_ThreadAverage_CheckBox,
                tr("Display PTHREADS experiment Thread Average value.") );

    pthreads_ThreadMin_CheckBox->setChecked(pthreads_ThreadMin);
    QToolTip::add(pthreads_ThreadMin_CheckBox,
                tr("Display PTHREADS experiment Thread Min value.") );

    pthreads_ThreadMax_CheckBox->setChecked(pthreads_ThreadMax);
    QToolTip::add(pthreads_ThreadMax_CheckBox,
                tr("Display PTHREADS experiment Thread Max value.") );


   } else if ( globalCollectorString.contains("fpe") ) {

#if 0
    fpe_time_CheckBox->setChecked(fpe_time);
    QToolTip::add(fpe_time_CheckBox,
                tr("Display FPE experiment time values.") );
#endif

    fpe_counts_CheckBox->setChecked(fpe_counts);
    QToolTip::add(fpe_counts_CheckBox,
                tr("Display FPE experiment exclusive counts values.") );

    fpe_inclusive_counts_CheckBox->setChecked(fpe_inclusive_counts);
    QToolTip::add(fpe_inclusive_counts_CheckBox,
                tr("Display FPE experiment inclusive counts values.") );


    fpe_percent_CheckBox->setChecked(fpe_percent);
    QToolTip::add(fpe_percent_CheckBox,
                tr("Display FPE experiment percent values.") );

    fpe_ThreadAverage_CheckBox->setChecked(fpe_ThreadAverage);
    QToolTip::add(fpe_ThreadAverage_CheckBox,
                tr("Display FPE experiment ThreadAverage values.") );

    fpe_ThreadMin_CheckBox->setChecked(fpe_ThreadMin);
    QToolTip::add(fpe_ThreadMin_CheckBox,
                tr("Display FPE experiment Thread Min values.") );

    fpe_ThreadMax_CheckBox->setChecked(fpe_ThreadMax);
    QToolTip::add(fpe_ThreadMax_CheckBox,
                tr("Display FPE experiment Thread Max values.") );

    fpe_inexact_result_count_CheckBox->setChecked(fpe_inexact_result_count);
    QToolTip::add(fpe_inexact_result_count_CheckBox,
                tr("Display FPE experiment inexact result count values.") );

    fpe_underflow_count_CheckBox->setChecked(fpe_underflow_count);
    QToolTip::add(fpe_underflow_count_CheckBox,
                tr("Display FPE experiment underflow count values.") );

    fpe_overflow_count_CheckBox->setChecked(fpe_overflow_count);
    QToolTip::add(fpe_overflow_count_CheckBox,
                tr("Display FPE experiment overflow count values.") );

    fpe_division_by_zero_count_CheckBox->setChecked(fpe_division_by_zero_count);
    QToolTip::add(fpe_division_by_zero_count_CheckBox,
                tr("Display FPE experiment division by zero count values.") );

    fpe_unnormal_count_CheckBox->setChecked(fpe_unnormal_count);
    QToolTip::add(fpe_unnormal_count_CheckBox,
                tr("Display FPE experiment unnormal count values.") );

    fpe_invalid_count_CheckBox->setChecked(fpe_invalid_count);
    QToolTip::add(fpe_invalid_count_CheckBox,
                tr("Display FPE experiment invalid count values.") );

    fpe_unknown_count_CheckBox->setChecked(fpe_unknown_count);
    QToolTip::add(fpe_unknown_count_CheckBox,
                tr("Display FPE experiment unknown count values.") );
   } else {
   }

// printf("OptionalViewsDialog::languageChange() entered\n");

    setCaption( tr( "OptionalViews Dialog" ) );
    categoryListView->header()->setLabel( 0, tr( "Categories" ) );
    categoryListView->clear();

    if( globalCollectorString.contains("pcsamp") ) {
       GeneralGroupBox->setTitle( tr( "PCSAMP Experiment Custom Report Selection Dialog" ) );
    } else if ( globalCollectorString.contains("omptp") ) {
       GeneralGroupBox->setTitle( tr( "OMPTP Experiment Custom Report Selection Dialog" ) );
    } else if ( globalCollectorString.contains("usertime") ) {
       GeneralGroupBox->setTitle( tr( "USERTIME Experiment Custom Report Selection Dialog" ) );
    } else if ( globalCollectorString.contains("hwcsamp") ) {
       GeneralGroupBox->setTitle( tr( "HWCSAMP Experiment Custom Report Selection Dialog" ) );
    } else if ( globalCollectorString.contains("hwctime") ) {
       GeneralGroupBox->setTitle( tr( "HWCTIME Experiment Custom Report Selection Dialog" ) );
    } else if ( globalCollectorString.contains("hwc") ) {
       GeneralGroupBox->setTitle( tr( "HWC Experiment Custom Report Selection Dialog" ) );
    } else if ( globalCollectorString.contains("iot") ) {
       GeneralGroupBox->setTitle( tr( "IOT Experiment Custom Report Selection Dialog" ) );
       VTraceGroupBox->setTitle( tr( "IOT Experiment Event List (-v trace) ONLY" ) );
    } else if (globalCollectorString.contains("io") ) {
       GeneralGroupBox->setTitle( tr( "IO Experiment Custom Report Selection Dialog" ) );
    } else if ( globalCollectorString.contains("iop") ) {
       GeneralGroupBox->setTitle( tr( "IOP Experiment Custom Report Selection Dialog" ) );
    } else if ( globalCollectorString.contains("mpip") ) {
       GeneralGroupBox->setTitle( tr( "MPIP Experiment Custom Report Selection Dialog" ) );
    } else if ( globalCollectorString.contains("mpit") ) {
       GeneralGroupBox->setTitle( tr( "MPIT Experiment Custom Report Selection Dialog" ) );
       VTraceGroupBox->setTitle( tr( "MPIT Experiment Event List (-v trace) ONLY" ) );
    } else if ( globalCollectorString.contains("mpi") ) {
       GeneralGroupBox->setTitle( tr( "MPI Experiment Custom Report Selection Dialog" ) );
    } else if ( globalCollectorString.contains("fpe") ) {
       GeneralGroupBox->setTitle( tr( "FPE Experiment Custom Report Selection Dialog" ) );
    } else {
       GeneralGroupBox->setTitle( tr( "General" ) );
    }

    buttonHelp->setText( tr( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
    buttonDefaults->setText( tr( "&Defaults" ) );
    buttonDefaults->setAccel( QKeySequence( tr( "Alt+D" ) ) );
    buttonApply->setText( tr( "&Apply" ) );
    buttonApply->setAccel( QKeySequence( QString::null ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
}


void OptionalViewsDialog::readPreferencesOnEntry()
{
// Maybe pick up the existing view settings and make those reflected in the 
// values to show the user when they click the CR "custom report" button.
//
// printf("readPreferencesOnEntry() entered\n");

}

void OptionalViewsDialog::resetPreferenceDefaults()
{

   if( globalCollectorString.contains("pcsamp") ) {
     pcsamp_percent = TRUE;
     pcsamp_percent_CheckBox->setChecked(pcsamp_percent);
     pcsamp_time = TRUE;
     pcsamp_time_CheckBox->setChecked(pcsamp_time);
     pcsamp_ThreadAverage = FALSE;
     pcsamp_ThreadAverage_CheckBox->setChecked(pcsamp_ThreadAverage);
     pcsamp_ThreadMin = FALSE;
     pcsamp_ThreadMin_CheckBox->setChecked(pcsamp_ThreadMin);
     pcsamp_ThreadMax = FALSE;
     pcsamp_ThreadMax_CheckBox->setChecked(pcsamp_ThreadMax);
   } else if ( globalCollectorString.contains("usertime") ) {
     usertime_exclusive_times = TRUE;
     usertime_exclusive_times_CheckBox->setChecked(usertime_exclusive_times);
     usertime_inclusive_times = TRUE;
     usertime_inclusive_times_CheckBox->setChecked(usertime_inclusive_times);
     usertime_percent = TRUE;
     usertime_percent_CheckBox->setChecked(usertime_percent);
     usertime_count = FALSE;
     usertime_count_CheckBox->setChecked(usertime_count);
     usertime_ThreadAverage = FALSE;
     usertime_ThreadAverage_CheckBox->setChecked(usertime_ThreadAverage);
     usertime_ThreadMin = FALSE;
     usertime_ThreadMin_CheckBox->setChecked(usertime_ThreadMin);
     usertime_ThreadMax = FALSE;
     usertime_ThreadMax_CheckBox->setChecked(usertime_ThreadMax);
   } else if ( globalCollectorString.contains("hwcsamp") ) {
     hwcsamp_time = TRUE;
     hwcsamp_time_CheckBox->setChecked(hwcsamp_time);
     hwcsamp_allEvents = TRUE;
     hwcsamp_allEvents_CheckBox->setChecked(hwcsamp_allEvents);
     hwcsamp_percent = FALSE;
     hwcsamp_percent_CheckBox->setChecked(hwcsamp_percent);
     hwcsamp_ThreadAverage = FALSE;
     hwcsamp_ThreadAverage_CheckBox->setChecked(hwcsamp_ThreadAverage);
     hwcsamp_ThreadMin = FALSE;
     hwcsamp_ThreadMin_CheckBox->setChecked(hwcsamp_ThreadMin);
     hwcsamp_ThreadMax = FALSE;
     hwcsamp_ThreadMax_CheckBox->setChecked(hwcsamp_ThreadMax);
   } else if ( globalCollectorString.contains("hwctime") ) {
     hwctime_exclusive_counts = FALSE;
     hwctime_exclusive_counts_CheckBox->setChecked(hwctime_exclusive_counts);
     hwctime_exclusive_overflows = TRUE;
     hwctime_exclusive_overflows_CheckBox->setChecked(hwctime_exclusive_overflows);
     hwctime_inclusive_overflows = FALSE;
     hwctime_inclusive_overflows_CheckBox->setChecked(hwctime_inclusive_overflows);
     hwctime_inclusive_counts = FALSE;
     hwctime_inclusive_counts_CheckBox->setChecked(hwctime_inclusive_counts);
     hwctime_percent = TRUE;
     hwctime_percent_CheckBox->setChecked(hwctime_percent);
     hwctime_ThreadAverage = FALSE;
     hwctime_ThreadAverage_CheckBox->setChecked(hwctime_ThreadAverage);
     hwctime_ThreadMin = FALSE;
     hwctime_ThreadMin_CheckBox->setChecked(hwctime_ThreadMin);
     hwctime_ThreadMax = FALSE;
     hwctime_ThreadMax_CheckBox->setChecked(hwctime_ThreadMax);
   } else if ( globalCollectorString.contains("hwc") ) {
     hwc_overflows = TRUE;
     hwc_overflows_CheckBox->setChecked(hwc_overflows);
     hwc_counts = FALSE;
     hwc_counts_CheckBox->setChecked(hwc_counts);
     hwc_percent = TRUE;
     hwc_percent_CheckBox->setChecked(hwc_percent);
     hwc_ThreadAverage = FALSE;
     hwc_ThreadAverage_CheckBox->setChecked(hwc_ThreadAverage);
     hwc_ThreadMin = FALSE;
     hwc_ThreadMin_CheckBox->setChecked(hwc_ThreadMin);
     hwc_ThreadMax = FALSE;
     hwc_ThreadMax_CheckBox->setChecked(hwc_ThreadMax);
   } else if ( globalCollectorString.contains("iot") ) {
#ifdef DEBUG_optional
      printf("OptionalViewsDialog::resetPreferenceDefaults, setting iot_exclusive_times=(%d) to TRUE\n", iot_exclusive_times);
#endif
     iot_exclusive_times = TRUE;
     iot_exclusive_times_CheckBox->setChecked(iot_exclusive_times);
     iot_inclusive_times = FALSE;
     iot_inclusive_times_CheckBox->setChecked(iot_inclusive_times);
     iot_min = FALSE;
     iot_min_CheckBox->setChecked(iot_min);
     iot_max = FALSE;
     iot_max_CheckBox->setChecked(iot_max);
     iot_average = FALSE;
     iot_average_CheckBox->setChecked(iot_average);
     iot_count = TRUE;
     iot_count_CheckBox->setChecked(iot_count);
     iot_percent = TRUE;
     iot_percent_CheckBox->setChecked(iot_percent);
     iot_stddev = FALSE;
     iot_stddev_CheckBox->setChecked(iot_stddev);
     iot_start_time = TRUE;
     iot_start_time_CheckBox->setChecked(iot_start_time);
     iot_stop_time = FALSE;
     iot_stop_time_CheckBox->setChecked(iot_stop_time);
     iot_syscallno = FALSE;
     iot_syscallno_CheckBox->setChecked(iot_syscallno);
     iot_nsysargs = FALSE;
     iot_nsysargs_CheckBox->setChecked(iot_nsysargs);
     iot_retval = TRUE;
     iot_retval_CheckBox->setChecked(iot_retval);
#if PATHNAME_READY
     iot_pathname = TRUE;
     iot_pathname_CheckBox->setChecked(iot_pathname);
#endif
   } else if ( globalCollectorString.contains("iop") ) {
     iop_exclusive_times = TRUE;
     iop_exclusive_times_CheckBox->setChecked(iop_exclusive_times);
     iop_inclusive_times = TRUE;
     iop_inclusive_times_CheckBox->setChecked(iop_inclusive_times);
     iop_percent = TRUE;
     iop_percent_CheckBox->setChecked(iop_percent);
     iop_count = FALSE;
     iop_count_CheckBox->setChecked(iop_count);
     iop_ThreadAverage = FALSE;
     iop_ThreadAverage_CheckBox->setChecked(iop_ThreadAverage);
     iop_ThreadMin = FALSE;
     iop_ThreadMin_CheckBox->setChecked(iop_ThreadMin);
     iop_ThreadMax = FALSE;
     iop_ThreadMax_CheckBox->setChecked(iop_ThreadMax);
   } else if (globalCollectorString.contains("io") ) {
     io_exclusive_times = TRUE;
#ifdef DEBUG_optional
      printf("OptionalViewsDialog::resetPreferenceDefaults, setting io_exclusive_times=(%d) to TRUE\n", iot_exclusive_times);
#endif
     io_exclusive_times_CheckBox->setChecked(io_exclusive_times);
     io_min = FALSE;
     io_min_CheckBox->setChecked(io_min);
     io_max = FALSE;
     io_max_CheckBox->setChecked(io_max);
     io_average = FALSE;
     io_average_CheckBox->setChecked(io_average);
     io_count = TRUE;
     io_count_CheckBox->setChecked(io_count);
     io_percent = TRUE;
     io_percent_CheckBox->setChecked(io_percent);
     io_stddev = FALSE;
     io_stddev_CheckBox->setChecked(io_stddev);
     io_ThreadAverage = FALSE;
     io_ThreadAverage_CheckBox->setChecked(io_ThreadAverage);
     io_ThreadMin = FALSE;
     io_ThreadMin_CheckBox->setChecked(io_ThreadMin);
     io_ThreadMax = FALSE;
     io_ThreadMax_CheckBox->setChecked(io_ThreadMax);
   } else if (globalCollectorString.contains("mem") ) {
     mem_exclusive_times = TRUE;
#ifdef DEBUG_optional
      printf("OptionalViewsDialog::resetPreferenceDefaults, setting mem_exclusive_times=(%d) to TRUE\n", mem_exclusive_times);
#endif
     mem_exclusive_times_CheckBox->setChecked(mem_exclusive_times);
     mem_min = FALSE;
     mem_min_CheckBox->setChecked(mem_min);
     mem_max = FALSE;
     mem_max_CheckBox->setChecked(mem_max);
     mem_average = FALSE;
     mem_average_CheckBox->setChecked(mem_average);
     mem_count = TRUE;
     mem_count_CheckBox->setChecked(mem_count);
     mem_percent = TRUE;
     mem_percent_CheckBox->setChecked(mem_percent);
     mem_stddev = FALSE;
     mem_stddev_CheckBox->setChecked(mem_stddev);
     mem_ThreadAverage = FALSE;
     mem_ThreadAverage_CheckBox->setChecked(mem_ThreadAverage);
     mem_ThreadMin = FALSE;
     mem_ThreadMin_CheckBox->setChecked(mem_ThreadMin);
     mem_ThreadMax = FALSE;
     mem_ThreadMax_CheckBox->setChecked(mem_ThreadMax);
   } else if ( globalCollectorString.contains("mpip") ) {
     mpip_exclusive_times = TRUE;
     mpip_exclusive_times_CheckBox->setChecked(mpip_exclusive_times);
     mpip_inclusive_times = TRUE;
     mpip_inclusive_times_CheckBox->setChecked(mpip_inclusive_times);
     mpip_percent = TRUE;
     mpip_percent_CheckBox->setChecked(mpip_percent);
     mpip_count = FALSE;
     mpip_count_CheckBox->setChecked(mpip_count);
     mpip_ThreadAverage = FALSE;
     mpip_ThreadAverage_CheckBox->setChecked(mpip_ThreadAverage);
     mpip_ThreadMin = FALSE;
     mpip_ThreadMin_CheckBox->setChecked(mpip_ThreadMin);
     mpip_ThreadMax = FALSE;
     mpip_ThreadMax_CheckBox->setChecked(mpip_ThreadMax);
   } else if ( globalCollectorString.contains("mpit") ) {
     mpit_exclusive_times = FALSE;
     mpit_exclusive_times_CheckBox->setChecked(mpit_exclusive_times);
     mpit_inclusive_times = FALSE;
     mpit_inclusive_times_CheckBox->setChecked(mpit_inclusive_times);
     mpit_min = TRUE;
     mpit_min_CheckBox->setChecked(mpit_min);
     mpit_max = TRUE;
     mpit_max_CheckBox->setChecked(mpit_max);
     mpit_average = TRUE;
     mpit_average_CheckBox->setChecked(mpit_average);
     mpit_count = TRUE;
     mpit_count_CheckBox->setChecked(mpit_count);
     mpit_percent = FALSE;
     mpit_percent_CheckBox->setChecked(mpit_percent);
     mpit_stddev = FALSE;
     mpit_stddev_CheckBox->setChecked(mpit_stddev);
     mpit_size = TRUE;
     mpit_size_CheckBox->setChecked(mpit_size);
     mpit_start_time = TRUE;
     mpit_start_time_CheckBox->setChecked(mpit_start_time);
     mpit_stop_time = FALSE;
     mpit_stop_time_CheckBox->setChecked(mpit_stop_time);
     mpit_source = TRUE;
     mpit_source_CheckBox->setChecked(mpit_source);
     mpit_dest = TRUE;
     mpit_dest_CheckBox->setChecked(mpit_dest);
     mpit_tag = FALSE;
     mpit_tag_CheckBox->setChecked(mpit_tag);
     mpit_communicator = FALSE;
     mpit_communicator_CheckBox->setChecked(mpit_communicator);
     mpit_datatype = FALSE;
     mpit_datatype_CheckBox->setChecked(mpit_datatype);
     mpit_retval = TRUE;
     mpit_retval_CheckBox->setChecked(mpit_retval);
   } else if ( globalCollectorString.contains("mpi") ) {
     mpi_exclusive_times = FALSE;
     mpi_inclusive_times = FALSE;
     mpi_min = TRUE;
     mpi_max = TRUE;
     mpi_average = TRUE;
     mpi_count = TRUE;
     mpi_percent = FALSE;
     mpi_stddev = FALSE;
     mpi_exclusive_times_CheckBox->setChecked(mpi_exclusive_times);
     mpi_inclusive_times_CheckBox->setChecked(mpi_inclusive_times);
     mpi_min_CheckBox->setChecked(mpi_min);
     mpi_max_CheckBox->setChecked(mpi_max);
     mpi_average_CheckBox->setChecked(mpi_average);
     mpi_count_CheckBox->setChecked(mpi_count);
     mpi_percent_CheckBox->setChecked(mpi_percent);
     mpi_stddev_CheckBox->setChecked(mpi_stddev);
   } else if (globalCollectorString.contains("pthreads") ) {
     pthreads_exclusive_times = TRUE;
#ifdef DEBUG_optional
      printf("OptionalViewsDialog::resetPreferenceDefaults, setting pthreads_exclusive_times=(%d) to TRUE\n", pthreads_exclusive_times);
#endif
     pthreads_exclusive_times_CheckBox->setChecked(pthreads_exclusive_times);
     pthreads_min = FALSE;
     pthreads_min_CheckBox->setChecked(pthreads_min);
     pthreads_max = FALSE;
     pthreads_max_CheckBox->setChecked(pthreads_max);
     pthreads_average = FALSE;
     pthreads_average_CheckBox->setChecked(pthreads_average);
     pthreads_count = TRUE;
     pthreads_count_CheckBox->setChecked(pthreads_count);
     pthreads_percent = TRUE;
     pthreads_percent_CheckBox->setChecked(pthreads_percent);
     pthreads_stddev = FALSE;
     pthreads_stddev_CheckBox->setChecked(pthreads_stddev);
     pthreads_ThreadAverage = FALSE;
     pthreads_ThreadAverage_CheckBox->setChecked(pthreads_ThreadAverage);
     pthreads_ThreadMin = FALSE;
     pthreads_ThreadMin_CheckBox->setChecked(pthreads_ThreadMin);
     pthreads_ThreadMax = FALSE;
     pthreads_ThreadMax_CheckBox->setChecked(pthreads_ThreadMax);
   } else if ( globalCollectorString.contains("fpe") ) {
#if 0
     fpe_time = TRUE;
#endif
     fpe_counts = TRUE;
     fpe_inclusive_counts = TRUE;
     fpe_percent = TRUE;
     fpe_ThreadAverage = FALSE;
     fpe_ThreadMin = FALSE;
     fpe_ThreadMax = FALSE;
     fpe_inexact_result_count = FALSE;
     fpe_underflow_count = FALSE;
     fpe_overflow_count = FALSE;
     fpe_division_by_zero_count = FALSE;
     fpe_unnormal_count = FALSE;
     fpe_invalid_count = FALSE;
     fpe_unknown_count = FALSE;
#if 0
     fpe_time_CheckBox->setChecked(fpe_time);
#endif
     fpe_counts_CheckBox->setChecked(fpe_counts);
     fpe_inclusive_counts_CheckBox->setChecked(fpe_inclusive_counts);
     fpe_percent_CheckBox->setChecked(fpe_percent);
     fpe_ThreadAverage_CheckBox->setChecked(fpe_ThreadAverage);
     fpe_ThreadMin_CheckBox->setChecked(fpe_ThreadMin);
     fpe_ThreadMax_CheckBox->setChecked(fpe_ThreadMax);
     fpe_inexact_result_count_CheckBox->setChecked(fpe_inexact_result_count);
     fpe_underflow_count_CheckBox->setChecked(fpe_underflow_count);
     fpe_overflow_count_CheckBox->setChecked(fpe_overflow_count);
     fpe_division_by_zero_count_CheckBox->setChecked(fpe_division_by_zero_count);
     fpe_unnormal_count_CheckBox->setChecked(fpe_unnormal_count);
     fpe_invalid_count_CheckBox->setChecked(fpe_invalid_count);
     fpe_unknown_count_CheckBox->setChecked(fpe_unknown_count);
   } else {
   }

}

QWidget *
OptionalViewsDialog::matchPreferencesToStack(QString s)
{
#ifdef DEBUG_optional
    printf("OptionalViewsDialog::matchPreferencesToStack, &StatsPanel::current_list_of_iot_modifiers=(%x)\n", &StatsPanel::current_list_of_iot_modifiers);
#endif
  OpenSpeedshop *mw = panelContainer->getMasterPC()->getMainWindow();

  for( PreferencesStackPagesList::Iterator it = mw->preferencesStackPagesList.begin();
       it != mw->preferencesStackPagesList.end();
       it++ )
  {
    QWidget *w = (QWidget *)*it;
    if( s == w->name() )
    {
// printf("Found s->(%s)\n", s.ascii() );
      return(w);
    }
  }

// printf("(%s) not Found\n", s.ascii() );
  return( (QWidget *)NULL );
}

void OptionalViewsDialog::listItemSelected(QListViewItem*lvi)
{
#ifdef DEBUG_optional
    printf("OptionalViewsDialog::listItemSelected, &StatsPanel::current_list_of_iot_modifiers=(%x)\n", &StatsPanel::current_list_of_iot_modifiers);
#endif
  if( lvi == NULL )
  {
    return;
  }

  QString s = lvi->text(0);

  QWidget *w = matchPreferencesToStack(s);
  if( w )
  {
    preferenceDialogWidgetStack->raiseWidget(w);
  } else
  {
    preferenceDialogWidgetStack->raiseWidget(generalStackPage);
  }
}

void OptionalViewsDialog::applyPreferences()
{

   if( globalCollectorString.contains("pcsamp") ) {
     pcsamp_time = pcsamp_time_CheckBox->isChecked();
     pcsamp_percent = pcsamp_percent_CheckBox->isChecked();
     pcsamp_ThreadAverage = pcsamp_ThreadAverage_CheckBox->isChecked();
     pcsamp_ThreadMin = pcsamp_ThreadMin_CheckBox->isChecked();
     pcsamp_ThreadMax = pcsamp_ThreadMax_CheckBox->isChecked();
   } else if ( globalCollectorString.contains("usertime") ) {
     usertime_exclusive_times = usertime_exclusive_times_CheckBox->isChecked();
     usertime_inclusive_times = usertime_inclusive_times_CheckBox->isChecked();
     usertime_percent = usertime_percent_CheckBox->isChecked();
     usertime_count = usertime_count_CheckBox->isChecked();
     usertime_ThreadAverage = usertime_ThreadAverage_CheckBox->isChecked();
     usertime_ThreadMin = usertime_ThreadMin_CheckBox->isChecked();
     usertime_ThreadMax = usertime_ThreadMax_CheckBox->isChecked();
   } else if ( globalCollectorString.contains("hwcsamp") ) {
     hwcsamp_time = hwcsamp_time_CheckBox->isChecked();
     hwcsamp_allEvents = hwcsamp_allEvents_CheckBox->isChecked();
     hwcsamp_percent = hwcsamp_percent_CheckBox->isChecked();
     hwcsamp_ThreadAverage = hwcsamp_ThreadAverage_CheckBox->isChecked();
     hwcsamp_ThreadMin = hwcsamp_ThreadMin_CheckBox->isChecked();
     hwcsamp_ThreadMax = hwcsamp_ThreadMax_CheckBox->isChecked();
   } else if ( globalCollectorString.contains("hwctime") ) {
     hwctime_exclusive_counts = hwctime_exclusive_counts_CheckBox->isChecked();
     hwctime_exclusive_overflows = hwctime_exclusive_overflows_CheckBox->isChecked();
     hwctime_inclusive_overflows = hwctime_inclusive_overflows_CheckBox->isChecked();
     hwctime_inclusive_counts = hwctime_inclusive_counts_CheckBox->isChecked();
     hwctime_percent = hwctime_percent_CheckBox->isChecked();
     hwctime_ThreadAverage = hwctime_ThreadAverage_CheckBox->isChecked();
     hwctime_ThreadMin = hwctime_ThreadMin_CheckBox->isChecked();
     hwctime_ThreadMax = hwctime_ThreadMax_CheckBox->isChecked();

   } else if ( globalCollectorString.contains("hwc") ) {
     hwc_overflows = hwc_overflows_CheckBox->isChecked();
     hwc_counts = hwc_counts_CheckBox->isChecked();
     hwc_percent = hwc_percent_CheckBox->isChecked();
     hwc_ThreadAverage = hwc_ThreadAverage_CheckBox->isChecked();
     hwc_ThreadMin = hwc_ThreadMin_CheckBox->isChecked();
     hwc_ThreadMax = hwc_ThreadMax_CheckBox->isChecked();
   } else if ( globalCollectorString.contains("iot") ) {
     iot_exclusive_times = iot_exclusive_times_CheckBox->isChecked();
     iot_inclusive_times = iot_inclusive_times_CheckBox->isChecked();
     iot_min = iot_min_CheckBox->isChecked();
     iot_max = iot_max_CheckBox->isChecked();
     iot_average = iot_average_CheckBox->isChecked();
     iot_count = iot_count_CheckBox->isChecked();
     iot_percent = iot_percent_CheckBox->isChecked();
     iot_stddev = iot_stddev_CheckBox->isChecked();
     iot_start_time = iot_start_time_CheckBox->isChecked();
     iot_stop_time = iot_stop_time_CheckBox->isChecked();
     iot_syscallno = iot_syscallno_CheckBox->isChecked();
     iot_nsysargs = iot_nsysargs_CheckBox->isChecked();
     iot_retval = iot_retval_CheckBox->isChecked();
#if PATHNAME_READY
     iot_pathname = iot_pathname_CheckBox->isChecked();
#endif
#ifdef DEBUG_optional
     printf("OptionalViewsDialog::applyPreferences, iot_exclusive_times=(%d)\n", iot_exclusive_times);
     printf("OptionalViewsDialog::applyPreferences, iot_inclusive_times=(%d)\n", iot_inclusive_times);
     printf("OptionalViewsDialog::applyPreferences, iot_min=(%d)\n", iot_min);
#endif
   } else if ( globalCollectorString.contains("iop") ) {
     iop_exclusive_times = iop_exclusive_times_CheckBox->isChecked();
     iop_inclusive_times = iop_inclusive_times_CheckBox->isChecked();
     iop_percent = iop_percent_CheckBox->isChecked();
     iop_count = iop_count_CheckBox->isChecked();
     iop_ThreadAverage = iop_ThreadAverage_CheckBox->isChecked();
     iop_ThreadMin = iop_ThreadMin_CheckBox->isChecked();
     iop_ThreadMax = iop_ThreadMax_CheckBox->isChecked();
   } else if (globalCollectorString.contains("io") ) {
     io_exclusive_times = io_exclusive_times_CheckBox->isChecked();
     io_min = io_min_CheckBox->isChecked();
     io_max = io_max_CheckBox->isChecked();
     io_average = io_average_CheckBox->isChecked();
     io_count = io_count_CheckBox->isChecked();
     io_percent = io_percent_CheckBox->isChecked();
     io_stddev = io_stddev_CheckBox->isChecked();
     io_ThreadAverage = io_ThreadAverage_CheckBox->isChecked();
     io_ThreadMin = io_ThreadMin_CheckBox->isChecked();
     io_ThreadMax = io_ThreadMax_CheckBox->isChecked();
   } else if (globalCollectorString.contains("mem") ) {
     mem_exclusive_times = mem_exclusive_times_CheckBox->isChecked();
     mem_min = mem_min_CheckBox->isChecked();
     mem_max = mem_max_CheckBox->isChecked();
     mem_average = mem_average_CheckBox->isChecked();
     mem_count = mem_count_CheckBox->isChecked();
     mem_percent = mem_percent_CheckBox->isChecked();
     mem_stddev = mem_stddev_CheckBox->isChecked();
     mem_ThreadAverage = mem_ThreadAverage_CheckBox->isChecked();
     mem_ThreadMin = mem_ThreadMin_CheckBox->isChecked();
     mem_ThreadMax = mem_ThreadMax_CheckBox->isChecked();
   } else if ( globalCollectorString.contains("mpip") ) {
     mpip_exclusive_times = mpip_exclusive_times_CheckBox->isChecked();
     mpip_inclusive_times = mpip_inclusive_times_CheckBox->isChecked();
     mpip_percent = mpip_percent_CheckBox->isChecked();
     mpip_count = mpip_count_CheckBox->isChecked();
     mpip_ThreadAverage = mpip_ThreadAverage_CheckBox->isChecked();
     mpip_ThreadMin = mpip_ThreadMin_CheckBox->isChecked();
     mpip_ThreadMax = mpip_ThreadMax_CheckBox->isChecked();
   } else if ( globalCollectorString.contains("mpit") ) {
     mpit_exclusive_times = mpit_exclusive_times_CheckBox->isChecked();
     mpit_inclusive_times = mpit_inclusive_times_CheckBox->isChecked();
     mpit_min = mpit_min_CheckBox->isChecked();
     mpit_max = mpit_max_CheckBox->isChecked();
     mpit_average = mpit_average_CheckBox->isChecked();
     mpit_count = mpit_count_CheckBox->isChecked();
     mpit_percent = mpit_percent_CheckBox->isChecked();
     mpit_stddev = mpit_stddev_CheckBox->isChecked();
     mpit_size = mpit_size_CheckBox->isChecked();
     mpit_start_time = mpit_start_time_CheckBox->isChecked();
     mpit_stop_time = mpit_stop_time_CheckBox->isChecked();
     mpit_source = mpit_source_CheckBox->isChecked();
     mpit_dest = mpit_dest_CheckBox->isChecked();
     mpit_tag = mpit_tag_CheckBox->isChecked();
     mpit_communicator = mpit_communicator_CheckBox->isChecked();
     mpit_datatype = mpit_datatype_CheckBox->isChecked();
     mpit_retval = mpit_retval_CheckBox->isChecked();
   } else if ( globalCollectorString.contains("mpi") ) {
     mpi_exclusive_times_CheckBox->isChecked();
     mpi_inclusive_times_CheckBox->isChecked();
     mpi_min_CheckBox->isChecked();
     mpi_max_CheckBox->isChecked();
     mpi_average_CheckBox->isChecked();
     mpi_count_CheckBox->isChecked();
     mpi_percent_CheckBox->isChecked();
     mpi_stddev_CheckBox->isChecked();
   } else if (globalCollectorString.contains("pthreads") ) {
     pthreads_exclusive_times = pthreads_exclusive_times_CheckBox->isChecked();
     pthreads_min = pthreads_min_CheckBox->isChecked();
     pthreads_max = pthreads_max_CheckBox->isChecked();
     pthreads_average = pthreads_average_CheckBox->isChecked();
     pthreads_count = pthreads_count_CheckBox->isChecked();
     pthreads_percent = pthreads_percent_CheckBox->isChecked();
     pthreads_stddev = pthreads_stddev_CheckBox->isChecked();
     pthreads_ThreadAverage = pthreads_ThreadAverage_CheckBox->isChecked();
     pthreads_ThreadMin = pthreads_ThreadMin_CheckBox->isChecked();
     pthreads_ThreadMax = pthreads_ThreadMax_CheckBox->isChecked();
   } else if ( globalCollectorString.contains("fpe") ) {
#if 0
     fpe_time = fpe_time_CheckBox->isChecked();
#endif
     fpe_counts = fpe_counts_CheckBox->isChecked();
     fpe_inclusive_counts = fpe_inclusive_counts_CheckBox->isChecked();
     fpe_percent = fpe_percent_CheckBox->isChecked();
     fpe_ThreadAverage = fpe_ThreadAverage_CheckBox->isChecked();
     fpe_ThreadMin = fpe_ThreadMin_CheckBox->isChecked();
     fpe_ThreadMax = fpe_ThreadMax_CheckBox->isChecked();
     fpe_inexact_result_count = fpe_inexact_result_count_CheckBox->isChecked();
     fpe_underflow_count = fpe_underflow_count_CheckBox->isChecked();
     fpe_overflow_count = fpe_overflow_count_CheckBox->isChecked();
     fpe_division_by_zero_count = fpe_division_by_zero_count_CheckBox->isChecked();
     fpe_unnormal_count = fpe_unnormal_count_CheckBox->isChecked();
     fpe_invalid_count = fpe_invalid_count_CheckBox->isChecked();
     fpe_unknown_count = fpe_unknown_count_CheckBox->isChecked();
   } else {
   }

}

void OptionalViewsDialog::buttonApplySelected()
{
#ifdef DEBUG_optional
    printf("OptionalViewsDialog::buttonApplySelected, &StatsPanel::current_list_of_iot_modifiers=(%x)\n", &StatsPanel::current_list_of_iot_modifiers);
#endif
    applyPreferences();
}

void OptionalViewsDialog::buttonOkSelected()
{
#if DEBUG_optional
    printf("OptionalViewsDialog::buttonOKSelected, entered\n");
#endif

#ifdef DEBUG_optional
    printf("OptionalViewsDialog::buttonOkSelected, &StatsPanel::current_list_of_iot_modifiers=(%x)\n", &StatsPanel::current_list_of_iot_modifiers);
#endif

    applyPreferences();

//    savePreferences();

    hide();
    accept();
}

void OptionalViewsDialog::savePreferences()
{
#ifdef DEBUG_optional
    printf("OptionalViewsDialog::savePreferences, &StatsPanel::current_list_of_iot_modifiers=(%x)\n", &StatsPanel::current_list_of_iot_modifiers);
#endif
}
