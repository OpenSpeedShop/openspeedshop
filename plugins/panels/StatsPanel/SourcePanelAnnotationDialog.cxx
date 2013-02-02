////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011-2013  Krell Institute  All Rights Reserved.
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
#include <qbuttongroup.h>
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
#include "SourcePanelAnnotationDialog.hxx"
#include "StatsPanel.hxx"

// enable the viewing of pathnames for iot experiments
#define PATHNAME_READY 1

//#define DEBUG_sourceAnno 1


/*
 *  Constructs a SourcePanelAnnotationDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SourcePanelAnnotationDialog::SourcePanelAnnotationDialog( QWidget* parent, 
                                          const char* name, 
                                          QString collectorString, 
                                          std::list<std::string> *current_modifiers, 
                                          bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
   panelContainer = (PanelContainer *)parent;
   globalCurrentModifiers = current_modifiers;
  
#ifdef DEBUG_sourceAnno
   std::cerr << "Enter SourcePanelAnnotationDialog::SourcePanelAnnotationDialog" << " name="  << name << " collectorString=" << collectorString << std::endl;
#endif

   if ( globalCollectorString.contains("hwcsamp") ) {
     for (int idx = 0; idx < hwcsamp_maxModIdx; idx++) {
          displayed_hwcsamp_CheckBox_status[idx] = FALSE;
          hwcsamp_CheckBox[idx]->setChecked( FALSE );
     }
   } else if ( globalCollectorString.contains("usertime") ) {
     for (int idx = 0; idx < usertime_maxModIdx; idx++) {
          displayed_usertime_CheckBox_status[idx] = FALSE;
          usertime_CheckBox[idx]->setChecked( FALSE );
     }
   } else {
   }

    if ( !name )
	setName( "SourcePanelAnnotationDialog" );

    globalCollectorString = collectorString;

    setSizeGripEnabled( TRUE );

    SourcePanelAnnotationDialogLayout = new QVBoxLayout( this, 11, 6, "SourcePanelAnnotationDialogLayout"); 

    SourcePanelAnnotationDialogWidgetStackLayout = new QHBoxLayout( 0, 0, 6, "SourcePanelAnnotationDialogWidgetStackLayout"); 

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

#ifdef DEBUG_sourceAnno
    printf("Calling SourcePanelAnnotationDialog::listCurrentModifierList\n");
#endif
    //listCurrentModifierList();

#ifdef DEBUG_sourceAnno
    printf("Calling SourcePanelAnnotationDialog::createExperimentDependentOptionalView\n");
#endif
    createExperimentDependentOptionalView(preferenceDialogWidgetStack, name );

    preferenceDialogRightFrameLayout->addWidget( preferenceDialogWidgetStack );
    preferenceDialogListLayout->addWidget( mainSplitter );

    SourcePanelAnnotationDialogWidgetStackLayout = new QHBoxLayout( 0, 0, 6, "SourcePanelAnnotationDialogWidgetStackLayout"); 
    

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setAutoDefault( TRUE );
    SourcePanelAnnotationDialogWidgetStackLayout->addWidget( buttonHelp );
    Horizontal_Spacing2 = new QSpacerItem( 120, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    SourcePanelAnnotationDialogWidgetStackLayout->addItem( Horizontal_Spacing2 );

    buttonDefaults = new QPushButton( this, "buttonDefaults" );
    SourcePanelAnnotationDialogWidgetStackLayout->addWidget( buttonDefaults );

    buttonApply = new QPushButton( this, "buttonApply" );
    buttonApply->setAutoDefault( TRUE );
    buttonApply->setDefault( TRUE );
    SourcePanelAnnotationDialogWidgetStackLayout->addWidget( buttonApply );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    SourcePanelAnnotationDialogWidgetStackLayout->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    SourcePanelAnnotationDialogWidgetStackLayout->addWidget( buttonCancel );
    preferenceDialogListLayout->addLayout( SourcePanelAnnotationDialogWidgetStackLayout );
    SourcePanelAnnotationDialogLayout->addLayout( preferenceDialogListLayout );

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
SourcePanelAnnotationDialog::~SourcePanelAnnotationDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

void SourcePanelAnnotationDialog::listCurrentModifierList() 
{

#ifdef DEBUG_sourceAnno
    printf("ENTER SourcePanelAnnotationDialog::listCurrentModifierList\n");
#endif
    for( std::list<std::string>::const_iterator it = globalCurrentModifiers->begin();
         it != globalCurrentModifiers->end(); it++ )
    {
      std::string cModifier = *it;
#ifdef DEBUG_sourceAnno
      printf("SourcePanelAnnotationDialog::listCurrentModifierList, cModifier.c_str()=%s\n", cModifier.c_str());
#endif
      ++it;
    }
#ifdef DEBUG_sourceAnno
    printf("EXIT SourcePanelAnnotationDialog::listCurrentModifierList\n");
#endif
    return;
}

bool SourcePanelAnnotationDialog::isInCurrentModifierList(std::string modifier) 
{

#ifdef DEBUG_sourceAnno
    printf("SourcePanelAnnotationDialog::isInCurrentModifierList, modifier to be checked is=%s\n", modifier.c_str());
#endif
    for( std::list<std::string>::const_iterator it = globalCurrentModifiers->begin();
         it != globalCurrentModifiers->end(); it++ )
    {
      std::string cModifier = *it;
      if (modifier == cModifier) {
#ifdef DEBUG_sourceAnno
        printf("SourcePanelAnnotationDialog::isInCurrentModifierList, result is TRUE\n");
#endif
        return TRUE;
      }
      it++;
    }
#ifdef DEBUG_sourceAnno
    printf("SourcePanelAnnotationDialog::isInCurrentModifierList, result is FALSE\n");
#endif
    return FALSE;
}


void
SourcePanelAnnotationDialog::createExperimentDependentOptionalView(QWidgetStack* stack, const char* name)
{
    generalStackPage = new QWidget( stack, name );
    generalStackPageLayout = new QVBoxLayout( generalStackPage, 11, 6, "generalStackPageLayout"); 

    GeneralGroupBox = new QButtonGroup( generalStackPage, "GeneralQButtonGroup" );
    GeneralGroupBox->setColumnLayout(0, Qt::Vertical );
    GeneralGroupBox->layout()->setSpacing( 6 );
    GeneralGroupBox->layout()->setMargin( 11 );
    GeneralGroupBox->setExclusive(TRUE);

    rightSideLayout = new QVBoxLayout( GeneralGroupBox->layout(), 11, "rightSideLayout"); 

    VTraceGroupBox = new QGroupBox( generalStackPage, "VTraceGroupBox" );
    VTraceGroupBox->setColumnLayout(0, Qt::Vertical );
    VTraceGroupBox->layout()->setSpacing( 6 );
    VTraceGroupBox->layout()->setMargin( 11 );

    rightSideVTraceLayout = new QVBoxLayout( VTraceGroupBox->layout(), 11, "rightSideVTraceLayout"); 

  if ( globalCollectorString.contains("hwcsamp") ) {

    VTraceGroupBox->hide();
    int mod_idx = 0;
    hwcsamp_maxModIdx = 0;
    for( std::list<std::string>::const_iterator it = globalCurrentModifiers->begin(); it != globalCurrentModifiers->end(); it++ )
    {
      std::string cModifier = *it;
#if DEBUG_sourceAnno
      printf("LISTING THE current modifiers for HWCSAMP, cModifier=%s\n", cModifier.c_str());
#endif


      { // hwcsamp_time
      std::string ModifierStringInfo = cModifier;
      std::string ModifierStringInfo1 = ModifierStringInfo.append("_CheckBox");
      hwcsamp_Modifiers[mod_idx] = cModifier;
      hwcsamp_CheckBox[mod_idx] = new QCheckBox( GeneralGroupBox, "modifier checkbox" );
      hwcsamp_CheckBox[mod_idx]->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, hwcsamp_CheckBox[mod_idx]->sizePolicy().hasHeightForWidth() ) );
      if ( hwcsamp_Modifiers[mod_idx] == "hwcsamp::time" ) {
          hwcsamp_CheckBox[mod_idx]->setChecked( TRUE );
      } else {
          hwcsamp_CheckBox[mod_idx]->setChecked( FALSE );
      } 
      std::string ModifierStringInfo2 = "HWCSAMP_";
      std::string ModifierStringInfo3 = ModifierStringInfo2.append(cModifier);
      hwcsamp_CheckBox[mod_idx]->setText( cModifier );
      rightSideLayout->addWidget( hwcsamp_CheckBox[mod_idx] );
    } // end hwcsamp
    mod_idx = mod_idx + 1;
    hwcsamp_maxModIdx = mod_idx;

    } // end for

  } else if ( globalCollectorString.contains("usertime") ) {

    VTraceGroupBox->hide();
    int mod_idx = 0;
    usertime_maxModIdx = 0;
    for( std::list<std::string>::const_iterator it = globalCurrentModifiers->begin(); it != globalCurrentModifiers->end(); it++ )
    {
      std::string cModifier = *it;
#if DEBUG_sourceAnno
      printf("LISTING THE current modifiers for USERTIME, cModifier=%s\n", cModifier.c_str());
#endif


      { // usertime_time
      std::string ModifierStringInfo = cModifier;
      std::string ModifierStringInfo1 = ModifierStringInfo.append("_CheckBox");
      usertime_Modifiers[mod_idx] = cModifier;
      usertime_CheckBox[mod_idx] = new QCheckBox( GeneralGroupBox, "modifier checkbox" );
      usertime_CheckBox[mod_idx]->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, usertime_CheckBox[mod_idx]->sizePolicy().hasHeightForWidth() ) );
      if ( usertime_Modifiers[mod_idx] == "usertime::exclusive_times" ) {
          usertime_CheckBox[mod_idx]->setChecked( TRUE );
      } else {
          usertime_CheckBox[mod_idx]->setChecked( FALSE );
      } 
      std::string ModifierStringInfo2 = "USERTIME_";
      std::string ModifierStringInfo3 = ModifierStringInfo2.append(cModifier);
      usertime_CheckBox[mod_idx]->setText( cModifier );
      rightSideLayout->addWidget( usertime_CheckBox[mod_idx] );
    } // end usertime
    mod_idx = mod_idx + 1;
    usertime_maxModIdx = mod_idx;

    } // end for

   } else {
    VTraceGroupBox->hide();
   }

    generalStackPageLayout->addWidget( GeneralGroupBox );
    generalStackPageLayout->addWidget( VTraceGroupBox );
    stack->addWidget( generalStackPage, 0 );
}


/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SourcePanelAnnotationDialog::languageChange()
{

   if ( globalCollectorString.contains("hwcsamp") ) {

#if DEBUG_sourceAnno
     printf("SourcePanelAnnotationDialog::languageChange, entered, hwcsamp_maxModIdx=%d\n", hwcsamp_maxModIdx);
#endif

     for (int idx = 0; idx < hwcsamp_maxModIdx; idx++) {
        displayed_hwcsamp_CheckBox_status[idx] = hwcsamp_CheckBox[idx]->isChecked();
     }
   } else if ( globalCollectorString.contains("usertime") ) {

#if DEBUG_sourceAnno
     printf("SourcePanelAnnotationDialog::languageChange, entered, usertime_maxModIdx=%d\n", usertime_maxModIdx);
#endif

     for (int idx = 0; idx < usertime_maxModIdx; idx++) {
        displayed_usertime_CheckBox_status[idx] = usertime_CheckBox[idx]->isChecked();
     }
   }


// printf("SourcePanelAnnotationDialog::languageChange() entered\n");

    setCaption( tr( "Source Panel Annotation Dialog" ) );
    categoryListView->header()->setLabel( 0, tr( "Categories" ) );
    categoryListView->clear();

    if ( globalCollectorString.contains("hwcsamp") ) {
       GeneralGroupBox->setTitle( tr( "HWCSAMP Experiment Source Panel Metric Annotation Selection Dialog" ) );
    } else if ( globalCollectorString.contains("usertime") ) {
       GeneralGroupBox->setTitle( tr( "USERTIME Experiment Source Panel Metric Annotation Selection Dialog" ) );
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


void SourcePanelAnnotationDialog::readPreferencesOnEntry()
{
// Maybe pick up the existing view settings and make those reflected in the 
// values to show the user when they click the CR "custom report" button.
//
// printf("readPreferencesOnEntry() entered\n");

}

void SourcePanelAnnotationDialog::resetPreferenceDefaults()
{

   if ( globalCollectorString.contains("hwcsamp") ) {
     for (int idx = 0; idx < hwcsamp_maxModIdx; idx++) {
        if ( hwcsamp_Modifiers[idx] == "hwcsamp::time" ) {
          displayed_hwcsamp_CheckBox_status[idx] = TRUE;
          hwcsamp_CheckBox[idx]->setChecked( TRUE );
        } else {
          displayed_hwcsamp_CheckBox_status[idx] = FALSE;
          hwcsamp_CheckBox[idx]->setChecked( FALSE );
        } 
     }

   } else if ( globalCollectorString.contains("usertime") ) {
     for (int idx = 0; idx < usertime_maxModIdx; idx++) {
        if ( usertime_Modifiers[idx] == "usertime::exclusive_times" ) {
          displayed_usertime_CheckBox_status[idx] = TRUE;
          usertime_CheckBox[idx]->setChecked( TRUE );
        } else {
          displayed_usertime_CheckBox_status[idx] = FALSE;
          usertime_CheckBox[idx]->setChecked( FALSE );
        } 
     }

   }

}

QWidget *
SourcePanelAnnotationDialog::matchPreferencesToStack(QString s)
{
#ifdef DEBUG_sourceAnno
    printf("SourcePanelAnnotationDialog::matchPreferencesToStack, &SourcePanelAnnotationDialog::globalCurrentModifiers=(%x)\n", &SourcePanelAnnotationDialog::globalCurrentModifiers);
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

void SourcePanelAnnotationDialog::listItemSelected(QListViewItem*lvi)
{

#ifdef DEBUG_sourceAnno
    printf("SourcePanelAnnotationDialog::listItemSelected, &SourcePanelAnnotationDialog::globalCurrentModifiers=(%x)\n", &SourcePanelAnnotationDialog::globalCurrentModifiers);
#endif

  if( lvi == NULL ) {
    return;
  }

  QString s = lvi->text(0);

  QWidget *w = matchPreferencesToStack(s);
  if( w ) {
    preferenceDialogWidgetStack->raiseWidget(w);
  } else {
    preferenceDialogWidgetStack->raiseWidget(generalStackPage);
  }
}

void SourcePanelAnnotationDialog::applyPreferences()
{

   if ( globalCollectorString.contains("hwcsamp") ) {

     int itemsSetCount = 0;
     for (int idx = 0; idx < hwcsamp_maxModIdx; idx++) {
        displayed_hwcsamp_CheckBox_status[idx] = hwcsamp_CheckBox[idx]->isChecked();
        if (hwcsamp_CheckBox[idx]->isChecked() == TRUE) {
          itemsSetCount = itemsSetCount + 1;
        }
     }
     if (itemsSetCount > 1) {
           // error condition
        fprintf(stderr, "Only one Source Annotation can be done at a time, please only choose one metric\n"); 
     }


   } else if ( globalCollectorString.contains("usertime") ) {

     int itemsSetCount = 0;
     for (int idx = 0; idx < usertime_maxModIdx; idx++) {
        displayed_usertime_CheckBox_status[idx] = usertime_CheckBox[idx]->isChecked();
        if (usertime_CheckBox[idx]->isChecked() == TRUE) {
          itemsSetCount = itemsSetCount + 1;
        }
     }
     if (itemsSetCount > 1) {
           // error condition
        fprintf(stderr, "Only one Source Annotation can be done at a time, please only choose one metric\n"); 
     }


   } else {
   }

}

void SourcePanelAnnotationDialog::buttonApplySelected()
{
#ifdef DEBUG_sourceAnno
    printf("SourcePanelAnnotationDialog::buttonApplySelected, &SourcePanelAnnotationDialog::globalCurrentModifiers=(%x)\n", &SourcePanelAnnotationDialog::globalCurrentModifiers);
#endif
    applyPreferences();
}

void SourcePanelAnnotationDialog::buttonOkSelected()
{
#if DEBUG_sourceAnno
    printf("SourcePanelAnnotationDialog::buttonOKSelected, entered\n");
#endif

#ifdef DEBUG_sourceAnno
    printf("SourcePanelAnnotationDialog::buttonOkSelected, &SourcePanelAnnotationDialog::globalCurrentModifiers=(%x)\n", &SourcePanelAnnotationDialog::globalCurrentModifiers);
#endif

    applyPreferences();

//    savePreferences();

    hide();
    accept();
}

void SourcePanelAnnotationDialog::savePreferences()
{
#ifdef DEBUG_sourceAnno
    printf("SourcePanelAnnotationDialog::savePreferences, &SourcePanelAnnotationDialog::globalCurrentModifiers=(%x)\n", &SourcePanelAnnotationDialog::globalCurrentModifiers);
#endif
}
