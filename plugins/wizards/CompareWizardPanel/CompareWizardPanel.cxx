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


#include <stdio.h>
#include "CompareWizardPanel.hxx"
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove
#include "AttachProcessDialog.hxx"

#define MAXROWS 8
#define MAXCOLUMNS 8

#include <qapplication.h>
#include <qvariant.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qframe.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qfile.h>   // For the file dialog box.
#include <qfiledialog.h>  // For the file dialog box.
#include <qmessagebox.h>
#include <qscrollview.h>
#include <qvbox.h>
#include <qdir.h>
#include <qfileinfo.h>

#include <qbitmap.h>
#include "rightarrow.xpm"
#include "leftarrow.xpm"
#include "folder.xpm"

#include "LoadAttachObject.hxx"

#include "CompareDescription.hxx"

#include "SS_Input_Manager.hxx"
using namespace OpenSpeedShop::Framework;

CompareWizardPanel::CompareWizardPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
  nprintf(DEBUG_CONST_DESTRUCT) ("CompareWizardPanel::CompareWizardPanel() constructor called\n");
  if ( !getName() )
  {
	setName( "Compare" );
  }

  QPixmap *folder_pm = new QPixmap( folder_xpm );
  folder_pm->setMask(folder_pm->createHeuristicMask());

  fn = QString::null;
  leftSideBaseName = QString::null;
  rightSideBaseName = QString::null;
  char *cwd = get_current_dir_name();
  rightSideDirName = cwd;
  leftSideDirName = cwd;
  free(cwd);

  mpiFormLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  mainFrame = new QFrame( getBaseWidgetFrame(), "mainFrame" );
  mainFrame->setMinimumSize( QSize(10,10) );
  mainFrame->setFrameShape( QFrame::StyledPanel );
  mainFrame->setFrameShadow( QFrame::Raised );
  mainFrameLayout = new QVBoxLayout( mainFrame, 11, 6, "mainFrameLayout"); 

  mainWidgetStack = new QWidgetStack( mainFrame, "mainWidgetStack" );
  mainWidgetStack->setMinimumSize( QSize(10,10) );

// Begin: verbose description page
  vDescriptionPageWidget = new QWidget( mainWidgetStack, "vDescriptionPageWidget" );
  vDescriptionPageLayout = new QVBoxLayout( vDescriptionPageWidget, 11, 6, "vDescriptionPageLayout"); 

  vDescriptionPageTitleLabel = new QLabel( vDescriptionPageWidget, "vDescriptionPageTitleLabel" );
  vDescriptionPageTitleLabel->setMinimumSize( QSize(0,0) );
  vDescriptionPageTitleLabel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed, 0, 0, FALSE ) );
  vDescriptionPageTitleLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
  vDescriptionPageLayout->addWidget( vDescriptionPageTitleLabel );

  vDescriptionPageText = new QTextEdit( vDescriptionPageWidget, "vDescriptionPageText" );
  vDescriptionPageText->setReadOnly(TRUE);
  vDescriptionPageText->setMinimumSize( QSize(10,10) );
  vDescriptionPageText->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum, 0, 0, FALSE ) );
  vDescriptionPageText->setMinimumSize( QSize(10,10) );
  vDescriptionPageText->setWordWrap( QTextEdit::WidgetWidth );
  vDescriptionPageLayout->addWidget( vDescriptionPageText );

  vDescriptionPageButtonLayout = new QHBoxLayout( 0, 0, 6, "vDescriptionPageButtonLayout"); 

  vDescriptionPageButtonSpacer = new QSpacerItem( 1, 10, QSizePolicy::Expanding, QSizePolicy::Fixed );
  vDescriptionPageButtonLayout->addItem( vDescriptionPageButtonSpacer );

  vDescriptionPageIntroButton = new QPushButton( vDescriptionPageWidget, "vDescriptionPageIntroButton" );
  vDescriptionPageIntroButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vDescriptionPageIntroButton->sizePolicy().hasHeightForWidth() ) );
  vDescriptionPageButtonLayout->addWidget( vDescriptionPageIntroButton );

  vDescriptionPageNextButton = new QPushButton( vDescriptionPageWidget, "vDescriptionPageNextButton" );
  vDescriptionPageNextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vDescriptionPageNextButton->sizePolicy().hasHeightForWidth() ) );
  vDescriptionPageButtonLayout->addWidget( vDescriptionPageNextButton );

  vDescriptionPageFinishButton = new QPushButton( vDescriptionPageWidget, "vDescriptionPageFinishButton" );
  vDescriptionPageFinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vDescriptionPageFinishButton->sizePolicy().hasHeightForWidth() ) );
  vDescriptionPageButtonLayout->addWidget( vDescriptionPageFinishButton );
  vDescriptionPageFinishButton->setEnabled(FALSE);

  vDescriptionPageLayout->addLayout( vDescriptionPageButtonLayout );
  mainWidgetStack->addWidget( vDescriptionPageWidget, 0 );
// End: verbose description page



  vAttachOrLoadPageWidget = new QWidget( mainWidgetStack, "vAttachOrLoadPageWidget" );
  vAttachOrLoadPageWidget->setMinimumSize( QSize(10,10) );
  vAttachOrLoadPageWidget->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred, 0, 0, FALSE ) );


  vAttachOrLoadPageLayout = new QVBoxLayout( vAttachOrLoadPageWidget, 11, 6, "vAttachOrLoadPageLayout"); 

  vLoad2ExecutablesPageDescriptionLabel = new QTextEdit( vAttachOrLoadPageWidget, "vLoad2ExecutablesPageDescriptionLabel" );
  vLoad2ExecutablesPageDescriptionLabel->setReadOnly(TRUE);
  vLoad2ExecutablesPageDescriptionLabel->setMinimumSize( QSize(10,10) );
  vLoad2ExecutablesPageDescriptionLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred, 0, 0, FALSE ) );
  vAttachOrLoadPageLayout->addWidget( vLoad2ExecutablesPageDescriptionLabel );

// Begin: AttachOrLoad page
  sv = new QScrollView( vAttachOrLoadPageWidget, "scrollView" );
  sv->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum, 0, 0, FALSE ) );
  big_box_w = new QWidget(sv->viewport(), "big_box(viewport)" );
  big_box_w->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum, 0, 0, FALSE ) );
  const QColor color = mainFrame->paletteBackgroundColor();
  sv->viewport()->setBackgroundColor(color);
// sv->viewport()->setPaletteBackgroundColor(color);
  sv->addChild(big_box_w);
  vAttachOrLoadPageLayout->addWidget( sv );

// For debugging layout
// big_box_w->setBackgroundColor("Red");

  vAttachOrLoadPageAttachOrLoadLayout = new QHBoxLayout( big_box_w, 0, 6, "vAttachOrLoadPageAttachOrLoadLayout"); 

// Begin LS
{
  QDir *leftSideDir = new QDir( leftSideDirName, "*.openss" );
  QFileInfoList *leftSideFileList = (QFileInfoList*)(leftSideDir->entryInfoList());

  QVBoxLayout *leftSideLayout = new QVBoxLayout( vAttachOrLoadPageAttachOrLoadLayout, 1, "leftSideLayout");

  QHBoxLayout *leftSideLabelLayout = new QHBoxLayout( leftSideLayout, 1, "leftSideLabelLayout");
  QSpacerItem *spacer1 = new QSpacerItem(5,5, QSizePolicy::Fixed, QSizePolicy::Fixed);
  leftSideLabelLayout->addItem(spacer1);
  leftSideExperimentLabel = new QLabel( big_box_w, "leftSideExperimentLabel" );
  leftSideExperimentLabel->setText("Select first experiment file:");
  leftSideExperimentLabel->setMinimumSize( QSize(10,10) );
  leftSideExperimentLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  leftSideLabelLayout->addWidget( leftSideExperimentLabel );
  QSpacerItem *spacer3 = new QSpacerItem(20,20, QSizePolicy::Preferred, QSizePolicy::Fixed);
  leftSideLabelLayout->addItem( spacer3 );


  QHBoxLayout *leftSideExperimentComboBoxLayout = new QHBoxLayout( leftSideLayout, 1, "leftSideExperimentComboBoxLayout");

  QSpacerItem *spacer2 = new QSpacerItem(5,5, QSizePolicy::Fixed, QSizePolicy::Fixed);
  leftSideExperimentComboBoxLayout->addItem(spacer2);
  ls_cbl = new QLabel(big_box_w, "experimentComboBoxLabel");
  ls_cbl->setText( tr("Available Experiments:") );
  QToolTip::add(ls_cbl, tr("Select the first experiment that you want\nto use in the comparison.") );
  leftSideExperimentComboBoxLayout->addWidget(ls_cbl);

  leftSideExperimentComboBox = new QComboBox(FALSE, big_box_w, "leftSideExperimentComboBox");

  QToolTip::add(leftSideExperimentComboBox, tr("Select the first experiment that you want\nto use in the comparison.") );
  leftSideExperimentComboBoxLayout->addWidget(leftSideExperimentComboBox);

  if( leftSideExperimentComboBox )
  {
    if( leftSideFileList )
    {
      QFileInfo *fileInfo = leftSideFileList->first();
      while( fileInfo )
      {
        leftSideExperimentComboBox->insertItem( fileInfo->fileName().ascii() );
        fileInfo = leftSideFileList->next();
      }
    }
  }

  leftSideExperimentDirButton = new QPushButton(big_box_w, "leftSideExperimentDirButton");
  leftSideExperimentDirButton->setPixmap(folder_xpm);
  leftSideExperimentDirButton->setMinimumSize( QSize(22, 18) );
  leftSideExperimentDirButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  leftSideExperimentDirButton->resize(22,18);
  connect( leftSideExperimentDirButton, SIGNAL( clicked() ), this, SLOT( leftSideExperimentDirButtonSelected() ) );
  leftSideExperimentComboBoxLayout->addWidget(leftSideExperimentDirButton);

  QSpacerItem *spacer4 = new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding);
  leftSideLayout->addItem( spacer4 );

  QSpacerItem *spacer41 = new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding);
  leftSideLabelLayout->addItem( spacer41 );
}
// End LS


// Begin RS
{
  QDir *rightSideDir = new QDir( rightSideDirName, "*.openss" );
  QFileInfoList *rightSideFileList = (QFileInfoList*)(rightSideDir->entryInfoList());

  QVBoxLayout *rightSideLayout = new QVBoxLayout( vAttachOrLoadPageAttachOrLoadLayout, 1, "rightSideLayout");

  QHBoxLayout *rightSideLabelLayout = new QHBoxLayout( rightSideLayout, 1, "rightSideLabelLayout");
  QSpacerItem *spacer11 = new QSpacerItem(5,5, QSizePolicy::Fixed, QSizePolicy::Fixed);
  rightSideLabelLayout->addItem(spacer11);

  rightSideExperimentLabel = new QLabel( big_box_w, "rightSideExperimentLabel" );
  rightSideExperimentLabel->setText("Select second experiment file:");
  rightSideExperimentLabel->setMinimumSize( QSize(10,10) );
  rightSideExperimentLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  rightSideLabelLayout->addWidget( rightSideExperimentLabel );

  QSpacerItem *spacer12 = new QSpacerItem(5,5, QSizePolicy::Expanding, QSizePolicy::Fixed);
  rightSideLabelLayout->addItem(spacer12);


  {
  QHBoxLayout *rightSideExperimentComboBoxLayout = new QHBoxLayout( rightSideLayout, 1, "rightSideExperimentComboBoxLayout");

  rs_cbl = new QLabel(big_box_w, "rightSideExperimentComboBoxLabel");
  rs_cbl->setText( tr("Available Experiments:") );
  QToolTip::add(rs_cbl, tr("Select the first experiment that you want\nto use in the comparison.") );
  rightSideExperimentComboBoxLayout->addWidget(rs_cbl);

  rightSideExperimentComboBox = new QComboBox(FALSE, big_box_w, "rightSideExperimentComboBox");

  QToolTip::add(rightSideExperimentComboBox, tr("Select the first experiment that you want\nto use in the comparison.") );
  rightSideExperimentComboBoxLayout->addWidget(rightSideExperimentComboBox);

  if( rightSideExperimentComboBox )
  {
    if( rightSideFileList )
    {
      QFileInfo *fileInfo = rightSideFileList->first();
      while( fileInfo )
      {
        rightSideExperimentComboBox->insertItem( fileInfo->fileName().ascii() );
        fileInfo = rightSideFileList->next();
      }
    }
  }
  rightSideExperimentDirButton = new QPushButton(big_box_w, "rightSideExperimentDirButton");
  rightSideExperimentDirButton->setMinimumSize( QSize(22, 18) );
  rightSideExperimentDirButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  rightSideExperimentDirButton->resize(22,18);
  rightSideExperimentDirButton->setPixmap(folder_xpm);
  connect( rightSideExperimentDirButton, SIGNAL( clicked() ), this, SLOT( rightSideExperimentDirButtonSelected() ) );
  rightSideExperimentComboBoxLayout->addWidget(rightSideExperimentDirButton);

  QSpacerItem *spacer = new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Expanding);
  rightSideExperimentComboBoxLayout->addItem( spacer );

  }
  QSpacerItem *spacer5 = new QSpacerItem(1,5, QSizePolicy::Fixed, QSizePolicy::Expanding);
  rightSideLayout->addItem( spacer5 );

  QSpacerItem *spacer51 = new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding);
  rightSideLabelLayout->addItem( spacer51 );
}

// End RS

  vAttachOrLoadPageButtonLayout = new QHBoxLayout( 0, 0, 6, "vAttachOrLoadPageButtonLayout"); 

  vAttachOrLoadPageButtonSpacer = new QSpacerItem( 251, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );
  vAttachOrLoadPageButtonLayout->addItem( vAttachOrLoadPageButtonSpacer );
  vAttachOrLoadPageBackButton = new QPushButton( vAttachOrLoadPageWidget, "vAttachOrLoadPageBackButton" );
  vAttachOrLoadPageBackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vAttachOrLoadPageBackButton->sizePolicy().hasHeightForWidth() ) );
  vAttachOrLoadPageButtonLayout->addWidget( vAttachOrLoadPageBackButton );

  vAttachOrLoadPageClearButton = new QPushButton( vAttachOrLoadPageWidget, "vAttachOrLoadPageClearButton" );
  vAttachOrLoadPageClearButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vAttachOrLoadPageClearButton->sizePolicy().hasHeightForWidth() ) );
  vAttachOrLoadPageButtonLayout->addWidget( vAttachOrLoadPageClearButton );
vAttachOrLoadPageClearButton->hide();

  vAttachOrLoadPageNextButton = new QPushButton( vAttachOrLoadPageWidget, "vAttachOrLoadPageNextButton" );
  vAttachOrLoadPageNextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vAttachOrLoadPageNextButton->sizePolicy().hasHeightForWidth() ) );
  vAttachOrLoadPageButtonLayout->addWidget( vAttachOrLoadPageNextButton );

  vAttachOrLoadPageFinishButton = new QPushButton( vAttachOrLoadPageWidget, "vAttachOrLoadPageFinishButton" );
  vAttachOrLoadPageFinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vAttachOrLoadPageFinishButton->sizePolicy().hasHeightForWidth() ) );
  vAttachOrLoadPageButtonLayout->addWidget( vAttachOrLoadPageFinishButton );

  vAttachOrLoadPageLayout->addLayout( vAttachOrLoadPageButtonLayout );
  mainWidgetStack->addWidget( vAttachOrLoadPageWidget, 1 );
// End: AttachOrLoad page

// Begin: verbose summary page
  vSummaryPageWidget = new QWidget( mainWidgetStack, "vSummaryPageWidget" );
  vSummaryPageWidget->setMinimumSize( QSize(10,10) );
  vSummaryPageLayout = new QVBoxLayout( vSummaryPageWidget, 11, 6, "vSummaryPageLayout"); 

  vSummaryPageLabelLayout = new QVBoxLayout( 0, 0, 6, "vSummaryPageLabelLayout"); 
  vSummaryPageFinishLabel = new QTextEdit( vSummaryPageWidget, "vSummaryPageFinishLabel" );
  vSummaryPageFinishLabel->setReadOnly(TRUE);
  vSummaryPageFinishLabel->setMinimumSize( QSize(10,10) );
  vSummaryPageFinishLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred, 0, 0, FALSE ) );
  vSummaryPageLabelLayout->addWidget( vSummaryPageFinishLabel );
  vSummaryPageLayout->addLayout( vSummaryPageLabelLayout );

  vSummaryPageButtonLayout = new QHBoxLayout( 0, 0, 6, "vSummaryPageButtonLayout"); 

  vSummaryPageButtonSpacer = new QSpacerItem( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  vSummaryPageButtonLayout->addItem( vSummaryPageButtonSpacer );

  vSummaryPageBackButton = new QPushButton( vSummaryPageWidget, "vSummaryPageBackButton" );
  vSummaryPageBackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vSummaryPageBackButton->sizePolicy().hasHeightForWidth() ) );
  vSummaryPageButtonLayout->addWidget( vSummaryPageBackButton );

  vSummaryPageNextButton = new QPushButton( vSummaryPageWidget, "vSummaryPageNextButton" );
  vSummaryPageNextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vSummaryPageNextButton->sizePolicy().hasHeightForWidth() ) );
  vSummaryPageButtonLayout->addWidget( vSummaryPageNextButton );
  vSummaryPageNextButton->setEnabled(FALSE);

  vSummaryPageFinishButton = new QPushButton( vSummaryPageWidget, "vSummaryPageFinishButton" );
  vSummaryPageFinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vSummaryPageFinishButton->sizePolicy().hasHeightForWidth() ) );
  vSummaryPageButtonLayout->addWidget( vSummaryPageFinishButton );
  vSummaryPageLayout->addLayout( vSummaryPageButtonLayout );
  mainWidgetStack->addWidget( vSummaryPageWidget, 3 );
// End: verbose summary page

  mainFrameLayout->addWidget( mainWidgetStack );
  mpiFormLayout->addWidget( mainFrame );
// End: advance (expert) summary page


  languageChange();

  resize( QSize(631, 508).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

    // signals and slots connections

  connect( vDescriptionPageNextButton, SIGNAL( clicked() ), this,
           SLOT( vDescriptionPageNextButtonSelected() ) );
  connect( vDescriptionPageIntroButton, SIGNAL( clicked() ), this,
           SLOT( vDescriptionPageIntroButtonSelected() ) );

  connect( vAttachOrLoadPageBackButton, SIGNAL( clicked() ), this,
           SLOT( loadPageBackButtonSelected() ) );

  connect( vAttachOrLoadPageNextButton, SIGNAL( clicked() ), this,
           SLOT( loadPageNextButtonSelected() ) );

  connect( vSummaryPageBackButton, SIGNAL( clicked() ), this,
           SLOT( vSummaryPageBackButtonSelected() ) );
  connect( vSummaryPageFinishButton, SIGNAL( clicked() ), this,
           SLOT( vSummaryPageFinishButtonSelected() ) );

  connect( vAttachOrLoadPageFinishButton, SIGNAL( clicked() ), this,
           SLOT( finishButtonSelected() ) );

  if( ao && ao->int_data == 0 )
  {
    // This wizard panel was brought up explicitly.   Don't
    // enable the hook to go back to the IntroWizardPanel.
    vDescriptionPageIntroButton->hide();
  }
}


//! Destroys the object and frees any allocated resources
/*! The only thing that needs to be cleaned up is the baseWidgetFrame.
 */
CompareWizardPanel::~CompareWizardPanel()
{
  nprintf(DEBUG_CONST_DESTRUCT) ("  CompareWizardPanel::~CompareWizardPanel() destructor called\n");
}

//! Add user panel specific menu items if they have any.
bool
CompareWizardPanel::menu(QPopupMenu* contextMenu)
{
  nprintf(DEBUG_PANELS) ("CompareWizardPanel::menu() requested.\n");

  return( FALSE );
}

//! Save ascii version of this panel.
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.
 */
void 
CompareWizardPanel::save()
{
  nprintf(DEBUG_PANELS) ("CompareWizardPanel::save() requested.\n");
}

//! Save ascii version of this panel (to a file).
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
CompareWizardPanel::saveAs()
{
  nprintf(DEBUG_PANELS) ("CompareWizardPanel::saveAs() requested.\n");
}

//! This function listens for messages.
int 
CompareWizardPanel::listener(void *msg)
{
  nprintf(DEBUG_PANELS) ("CompareWizardPanel::listener() requested.\n");

  MessageObject *messageObject = (MessageObject *)msg;
  nprintf(DEBUG_PANELS) ("  messageObject->msgType = %s\n", messageObject->msgType.ascii() );
  if( messageObject->msgType == getName() )
  {
    vSummaryPageFinishButton->setEnabled(TRUE);
    vSummaryPageNextButton->setEnabled(FALSE);
    vSummaryPageBackButton->setEnabled(TRUE);
    qApp->flushX();
    return 1;
  }
  if( messageObject->msgType == "Wizard_Raise_First_Page" )
  {
    vSummaryPageFinishButton->setEnabled(TRUE);
    vSummaryPageNextButton->setEnabled(FALSE);
    vSummaryPageBackButton->setEnabled(TRUE);
//    qApp->flushX();
    nprintf(DEBUG_PANELS) ("vDescriptionPageWidget\n");
    mainWidgetStack->raiseWidget(vDescriptionPageWidget);
    return 1;
  }
  return 0;  // 0 means, did not want this message and did not act on anything.
}


//! This function broadcasts messages.
int 
CompareWizardPanel::broadcast(char *msg)
{
  nprintf(DEBUG_PANELS) ("CompareWizardPanel::broadcast() requested.\n");
  return 0;
}


void CompareWizardPanel::vDescriptionPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vDescriptionPageNextButtonSelected() \n");


  mainWidgetStack->raiseWidget(vAttachOrLoadPageWidget);
}

void CompareWizardPanel::vDescriptionPageIntroButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vDescriptionPageIntroButtonSelected() \n");

  getPanelContainer()->hidePanel((Panel *)this);

  Panel *p = getPanelContainer()->raiseNamedPanel("Intro Wizard");
  if( !p )
  {
    nprintf(DEBUG_PANELS) ("vDescriptionPageIntroButtonSelected() create a new one!\n");
    getPanelContainer()->getMasterPC()->dl_create_and_add_panel("Intro Wizard", getPanelContainer(), NULL);
  }
}

void CompareWizardPanel::loadPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("loadPageBackButtonSelected() \n");

  mainWidgetStack->raiseWidget(vDescriptionPageWidget);
}

void CompareWizardPanel::loadPageNextButtonSelected()
{
// printf("loadPageNextButtonSelected() \n");


// printf("leftSideExperimentComboBox->text()=(%s)\n", leftSideExperimentComboBox->currentText().ascii() );
// printf("rightSideExperimentComboBox->text()=(%s)\n", rightSideExperimentComboBox->currentText().ascii() );

  if( leftSideExperimentComboBox->currentText().isEmpty() )
  {
    warnOfnoSavedData();
    return;
  }

  
  fn = QString::null;

  vSummaryPageFinishLabel->setText( tr( QString("You are requesting to compare experiment <b>\"%1/%2\"</b> with experiment <b>\"%3/%4\"</b>.  <p><p>Pressing finish will bring up a Compare Experiments Panel with your requested information.\n").arg(leftSideDirName).arg(leftSideExperimentComboBox->currentText()).arg(rightSideDirName).arg(rightSideExperimentComboBox->currentText()) ) );

  mainWidgetStack->raiseWidget(vSummaryPageWidget);

  
}
// End verbose AttachOrLoad callbacks


void CompareWizardPanel::vSummaryPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vSummaryPageBackButtonSelected() \n");

  mainWidgetStack->raiseWidget(vAttachOrLoadPageWidget);
}

void CompareWizardPanel::finishButtonSelected()
{
  nprintf(DEBUG_PANELS) ("finishButtonSelected() \n");

  vSummaryPageFinishButtonSelected();
}


void CompareWizardPanel::vSummaryPageFinishButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vSummaryPageFinishButtonSelected() \n");
// printf("vSummaryPageFinishButtonSelected() \n");

  {
    if( fn.isEmpty() )
    {
// printf("leftSideExperimentComboBox->text()=(%s/%s)\n", leftSideDirName.ascii(), leftSideExperimentComboBox->currentText().ascii() );
// printf("rightSideExperimentComboBox->text()=(%s/%s)\n", rightSideDirName.ascii(), rightSideExperimentComboBox->currentText().ascii() );

//      leftSideDirName = leftSideExperimentComboBox->currentText();
//      rightSideDirName = rightSideExperimentComboBox->currentText();
  
      if( leftSideDirName+"/"+leftSideExperimentComboBox->currentText() == 
          rightSideDirName+"/"+rightSideExperimentComboBox->currentText() )
      {
        int answer = QMessageBox::question ( this, tr("Question:"), tr("You're requesting to compare the same 2 experiments.\n\nIs that what you intended?"),   QMessageBox::Yes,  QMessageBox::No,  QMessageBox::NoButton );
        if( answer ==  QMessageBox::No )
        {
          mainWidgetStack->raiseWidget(vAttachOrLoadPageWidget);
          return;
        }
      }

      vSummaryPageFinishLabel->setText( tr( QString("You are requesting to compare experiment <b>\"%1/%2\"</b> with experiment <b>\"%3/%4\"</b>.  Pressing finish will bring up a Compare Experiments Panel with your requested information.\n").arg(leftSideDirName).arg(leftSideExperimentComboBox->currentText()).arg(rightSideDirName).arg(rightSideExperimentComboBox->currentText()) ) );
      mainWidgetStack->raiseWidget(vSummaryPageWidget);
      if( getPanelContainer()->getMainWindow() )
      { 
        OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
        if( mw )
        {
          LoadAttachObject *lao = NULL;
          vSummaryPageFinishButton->setEnabled(FALSE);
          vSummaryPageBackButton->setEnabled(FALSE);
          qApp->flushX();

// printf("Create the CompareExperimentsPanel...\n");

          Panel *p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("Compare Experiments", getPanelContainer());

          if( p )
          {
// printf("First expRestore -f exp1; expRestore -f exp2;\n");

// printf("The call the CompareExperimentsPanel's listener to load the stats panel with the arguments expCompare -x 1 -x 2 sort of syntax....\n");
           lao = new LoadAttachObject(QString::null, QString::null, NULL, TRUE);
           lao->leftSideExperiment = leftSideDirName+"/"+leftSideExperimentComboBox->currentText();
           lao->rightSideExperiment = rightSideDirName+"/"+rightSideExperimentComboBox->currentText();
           p->listener((void *)lao);
        } else 
        {
          printf("Error creating \"Compare Experiments Panel\"\n");
        }
        }
      }
    } else
    {
// printf("fn was not empty\n");
      getPanelContainer()->getMainWindow()->fileOpenSavedExperiment(fn);
    }
  }
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void
CompareWizardPanel::languageChange()
{
  unsigned int traced_functions = 100;

  setCaption( tr( "Compare Wizard Panel" ) );
  vDescriptionPageTitleLabel->setText( tr( "<h1>Compare Wizard</h1>" ) );
  vDescriptionPageText->setText( tr( vCompareDescription ) );
  vDescriptionPageIntroButton->setText( tr( "< Back" ) );
  QToolTip::add( vDescriptionPageIntroButton, tr( "Takes you back to the second page of the Intro Wizard so you can make a different selection." ) );
  vDescriptionPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( vDescriptionPageNextButton, tr( "Advance to the next wizard page." ) );
  vDescriptionPageFinishButton->setText( tr( ">> Finish" ) );

  vLoad2ExecutablesPageDescriptionLabel->setText( tr( "This wizard helps load 2 experiments to so the results can be compared.\n\nBelow are 2 columns.   The left side experiment will be compared with the experiment from the right side.\n\nBy default, files ending with a \".openss\" suffix are shown in the pulldown menu.  If the experiment files you wish to load don't end in \".openss\" or they are in a different directory, use the dialog box (Click on the folder icon.) to change the directory/and or suffix.\n\nAfter selecting the 2 experiment files, click on the \"Next\" button to continue.") );

  vAttachOrLoadPageBackButton->setText( tr( "< Back" ) );
  QToolTip::add( vAttachOrLoadPageBackButton, tr( "Takes you back one page." ) );
  vAttachOrLoadPageClearButton->setText( tr( "Clear" ) );
  QToolTip::add( vAttachOrLoadPageClearButton, tr( "This clears all settings restoring them to system defaults." ) );
  vAttachOrLoadPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( vAttachOrLoadPageNextButton, tr( "Advance to the next wizard page." ) );
  vAttachOrLoadPageFinishButton->setText( tr( ">> Finish" ) );
  QToolTip::add( vAttachOrLoadPageFinishButton, tr( "Advance to the wizard finish page." ) );
  vSummaryPageFinishLabel->setText( tr( "No summary available.\n" ) );

  vSummaryPageBackButton->setText( tr( "< Back" ) );
  QToolTip::add( vSummaryPageBackButton, tr( "Takes you back one page." ) );
  vSummaryPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( vSummaryPageNextButton, tr( "Takes you forward one page." ) );
  vSummaryPageFinishButton->setText( tr( "Finish..." ) );
  QToolTip::add( vSummaryPageFinishButton, tr( "Finishes loading the wizard information and brings up a \"mpi\" panel" ) );
}

void
CompareWizardPanel::vUpdateAttachOrLoadPageWidget()
{
// printf("Pop up the dialog box to load an saved file.\n");
  requestExperimentFileName();
  if( !fn.isEmpty() )
  {
    mainWidgetStack->raiseWidget(vSummaryPageWidget);
  }

  return;
}

void
CompareWizardPanel::requestExperimentFileName()
{
  fn = QString::null;
  char *cwd = get_current_dir_name();
//  fn = QFileDialog::getOpenFileName( cwd, "Experiment Files (*.openss)", this, "open experiment dialog", "Choose an experiment file to open");
  fn = QFileDialog::getOpenFileName( cwd, "Experiment Files (*.openss);;Any Files (*.*)", this, "Choose experiment location:", "Choose a directory where experiment files are locate:");
  free(cwd);
  if( !fn.isEmpty() )
  {
    char buffer[2048];
    if( !fn.isEmpty() )
    {
      getPanelContainer()->getMainWindow()->executableName = QString::null;
// printf("fn = %s\n", fn.ascii() );
// printf("A: Determine which panel to bring up base on experiment file %s\n", fn.ascii() );
      vSummaryPageFinishLabel->setText( tr( QString("You are requesting to load saved experiment <b>\"%1\"</b>.  Pressing finish will bring up your requested information.\n").arg(fn) ) );
    }
  }
}

void
CompareWizardPanel::warnOfnoSavedData()
{
    QMessageBox::information(this, tr("No saved data files located."), tr("The ability to compare 2 experiments with one another requires 2 saved\nexperiment files.   To create a saved experiment file, first select an\nexperiment to run, load and run the executable, then using\nthe \"File->Save Experiment Data\", save the experiment to a file.  Likewise, save\nthe second data file from a second run.   Then reissue this wizard to compare the\nresults.\n\nThere are also other ways to access this same functionality (and more).  One\ncan first run and save an initial data file.  Then during the second run, bring\nup the Customize StatsPanel and customize the report you'd like to see."), "OK"); 
}

void
CompareWizardPanel::leftSideExperimentDirButtonSelected()
{
// printf("leftSideExperimentDirButtonSelected() entered\n");
  QString fn = QString::null;

  fn = QString::null;
  char *cwd = get_current_dir_name();
  fn = QFileDialog::getOpenFileName( cwd, "Experiment Files (*.openss);;Any Files (*.*)", this, "Choose experiment location:", "Choose a directory where experiment files are locate:");

// printf("A: fn=(%s)\n", fn.ascii() );
  QFileInfo fi(fn);

  int basename_index = fn.findRev("/");
  leftSideBaseName = fn;
  if( basename_index != -1 )
  {
    leftSideDirName = fn.left(basename_index);
    leftSideBaseName = fn.right((fn.length()-basename_index)-1);
  }
// printf("leftSideDirName=(%s) leftSideBaseName=(%s)\n", leftSideDirName.ascii(), leftSideBaseName.ascii() );


  free(cwd);
  if( !leftSideDirName.isEmpty() )
  {
    char buffer[2048];
    if( !fn.isEmpty() )
    {
      if( fi.isFile() )
      { // The user gave us a direct, possibly non .openss conforming filename.
        leftSideExperimentComboBox->insertItem( fn );
      } else
      {
        QDir *dir = new QDir( leftSideDirName, "*.openss" );
        QFileInfoList *fileList = (QFileInfoList*)(dir->entryInfoList());
        if( leftSideExperimentComboBox )
        {
          if( fileList )
          {
            leftSideExperimentComboBox->clear();
            QFileInfo *fileInfo = fileList->first();
            while( fileInfo )
            {
              leftSideExperimentComboBox->insertItem( fileInfo->fileName().ascii() );
              fileInfo = fileList->next();
            }
          }
        }
      }
    }
    leftSideExperimentComboBox->setCurrentText(leftSideBaseName);
  }

}


void
CompareWizardPanel::rightSideExperimentDirButtonSelected()
{
// printf("rightSideExperimentDirButtonSelected() entered\n");

  QString fn = QString::null;

  fn = QString::null;
  char *cwd = get_current_dir_name();
  fn = QFileDialog::getOpenFileName( cwd, "Experiment Files (*.openss);;Any Files (*.*)", this, "Choose experiment location:", "Choose a directory where experiment files are locate:");

// printf("fn=(%s)\n", fn.ascii() );
  QFileInfo fi(fn);

  int basename_index = fn.findRev("/");
  rightSideBaseName = fn;
  if( basename_index != -1 )
  {
    rightSideDirName = fn.left(basename_index);
    rightSideBaseName = fn.right((fn.length()-basename_index)-1);
  }
// printf("rightSideDirName=(%s) rightSideBaseName=(%s)\n", rightSideDirName.ascii(), rightSideBaseName.ascii() );


  free(cwd);
  if( !rightSideDirName.isEmpty() )
  {
    char buffer[2048];
    if( !fn.isEmpty() )
    {
      if( fi.isFile() )
      { // The user gave us a direct, possibly non .openss conforming filename.
        rightSideExperimentComboBox->insertItem( fn );
      } else
      {
        QDir *dir = new QDir( rightSideDirName, "*.openss" );
        QFileInfoList *fileList = (QFileInfoList*)(dir->entryInfoList());
        if( rightSideExperimentComboBox )
        {
          if( fileList )
          {
            rightSideExperimentComboBox->clear();
            QFileInfo *fileInfo = fileList->first();
            while( fileInfo )
            {
              rightSideExperimentComboBox->insertItem( fileInfo->fileName().ascii() );
              fileInfo = fileList->next();
            }
          }
        }
      }
    }
    rightSideExperimentComboBox->setCurrentText(rightSideBaseName);
  }
}

#ifdef PULL
void
CompareWizardPanel::handleSizeEvent(QResizeEvent *e)
{
  int calculated_height = 0;
  calculated_height += leftSideExperimentLabel->height();
  calculated_height += ls_cbl->height();
  calculated_height += leftSideExperimentComboBox->height();
  calculated_height += leftSideExperimentDirButton->height();

  int height = getPanelContainer()->parent->height();
  if( calculated_height > height )
  {
    height = calculated_height;
  }


  int calculated_width = 0;
  calculated_width += leftSideExperimentLabel->width();
  calculated_width += leftSideExperimentComboBox->width();
  calculated_width += leftSideExperimentDirButton->width();
  calculated_width += rightSideExperimentLabel->width();
  calculated_width += rightSideExperimentComboBox->width();
  calculated_width += rightSideExperimentDirButton->width();

  calculated_width += 50; // Margins

  big_box_w->resize(calculated_width,calculated_height);
}
#endif // PULL
