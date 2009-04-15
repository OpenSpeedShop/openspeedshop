////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 Krell Institute. All Rights Reserved.
// Copyright (c) 2006, 2007, 2008 Krell Institute All Rights Reserved.
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


//
//
//#define DEBUG_loadPanel 1

#include "loadPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove
#include <qapplication.h>
#include <qvariant.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qcheckbox.h>
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
#include <qbitmap.h>
#include <qlistview.h>  // For QListView
#include <qvaluelist.h>  // For QTextEdit in example below...
#include <qsplitter.h>  // For QSplitter in example below...
#include <chartform.hxx>  // For chart in example below...

loadPanel::loadPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{

#ifdef DEBUG_loadPanel
  printf("loadPanel::loadPanel(), entered\n");
#endif

  // Initialize the panel handle of the panel calling the loadPanel
  targetPanel = NULL;

  // Initialize experiment id 
  int expID = -1;

  // Initialize that we are not under control of a MPIWizard
  setMPIWizardCalledMe(FALSE);

  // Initialize that we have loaded an executable before, so we can ask if 
  // we want to load a different one now
  setHaveWeLoadedAnExecutableBefore(FALSE);
  setHaveWeAttachedToPidBefore(FALSE);

  // Indicate that we were not doing parallel loads or stores when
  // we left the loadPanel last.
  setWasDoingParallel(FALSE);

  // Is an argument object present?
  if( ao ) {

#ifdef DEBUG_loadPanel
    printf("loadPanel::loadPanel(), have non-null argument object\n");
#endif

    targetPanel = ao->panel_data;

#ifdef DEBUG_loadPanel
    printf("loadPanel::loadPanel(), (targetPanel is initially set non-null) panel name is=%s\n", targetPanel->getName());
#endif

    bool temp_instrumentorIsOffline = ao->isInstrumentorOffline;
    setInstrumentorIsOffline(temp_instrumentorIsOffline);

#ifdef DEBUG_loadPanel
    printf("loadPanel::loadPanel(), ao->isInstrumentorOffline=%d\n", ao->isInstrumentorOffline);
#endif

    if (targetPanel->getName()) {

      QString panelName = targetPanel->getName();

      // Use whether we are under control of a MPI wizard
      // to set the initial checkboxes for the user
      int mpi_index = panelName.find("MPI");
      if (mpi_index > -1) {
        setMPIWizardCalledMe(TRUE);
      }

      // Try to glean the experiment Id off of the panel name
      int start_index = panelName.find("[");
      int end_index;
      if (start_index > -1) {
        end_index = panelName.find("]");
      
        QString expIDStr = panelName.mid(start_index+1, end_index-start_index-1);
        expID = expIDStr.toInt();

#ifdef DEBUG_loadPanel
        printf("loadPanel::loadPanel(), expIDStr.ascii()=%s, expID=%d\n", expIDStr.ascii(), expID);
#endif

      } else {

#ifdef DEBUG_loadPanel
        printf("loadPanel::loadPanel(), NO expID in targetPanel name start_index=%d, expID=%d\n", start_index, expID);
#endif

      }
    }
  }

  setCaption("loadPanel");

  char name_buffer[100];
  if ( !getName() ) {

    // If the panel doesn't have a name, set it
    if (expID == -1) {
      setName("loadPanel");
    } else {
      sprintf(name_buffer, "loadPanel [%d]", expID);
      setName(name_buffer);
    }

  } else {
    
    // If the panel does have a name, set it with the experiment id
    if (expID == -1) {
      sprintf(name_buffer, "%s", getName());
    } else {
      sprintf(name_buffer, "%s [%d]", getName(), expID);
    }
    setName(name_buffer);

  } // end else clause

  vMPLoadPageLoadMPHostName = QString::null;
  vMPLoadPageLoadMPPidName = QString::null;
  vMPLoadPageLoadMPProgName = QString::null;

  loadPanelFormLayout = new QVBoxLayout( getBaseWidgetFrame(), 0, 0, "Load Panel" );

  mainFrame = new QFrame( getBaseWidgetFrame(), "mainFrame" );
  mainFrame->setMinimumSize( QSize(10,10) );
  mainFrame->setFrameShape( QFrame::StyledPanel );
  mainFrame->setFrameShadow( QFrame::Raised );
  mainFrame->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum, 0, 0, FALSE ) );
  mainFrameLayout = new QVBoxLayout( mainFrame, 11, 6, "mainFrameLayout");

  mainWidgetStack = new QWidgetStack( mainFrame, "mainWidgetStack" );
  mainWidgetStack->setMinimumSize( QSize(10,10) );
  mainWidgetStack->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum, 0, 0, FALSE ) );


// Begin: AttachOrLoad page
  vALStackPage0 = new QWidget( mainWidgetStack, "vALStackPage0" );
  vALStackPage0->setMinimumSize( QSize(10,10) );
  vALStackPage0->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred, 0, 0, FALSE ) );
//  vALStackPage0Layout = new QVBoxLayout( vALStackPage0, 11, 6, "vALStackPage0Layout"); 
  vALStackPage0Layout = new QVBoxLayout( vALStackPage0, 0, 0, "vALStackPage0Layout"); 

  vpage0sv = new QScrollView( vALStackPage0, "vpage0sv" );
  vpage0sv->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum, 0, 0, FALSE ) );

  vpage0big_box = new QVBox(vpage0sv->viewport(), "vpage0big_box" );
  vpage0big_box->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum, 0, 0, FALSE ) );
//  vpage0big_box->setPaletteBackgroundColor("Tan");

  vpage0sv->addChild(vpage0big_box);

  vAttachOrLoadPageDescriptionLabel = new QTextEdit( vALStackPage0, "vAttachOrLoadPageDescriptionLabel" );
  vAttachOrLoadPageDescriptionLabel->setReadOnly(TRUE);
  vAttachOrLoadPageDescriptionLabel->setMinimumSize( QSize(10,10) );
  vAttachOrLoadPageDescriptionLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum, 0, 0, vAttachOrLoadPageDescriptionLabel->sizePolicy().hasHeightForWidth()  ) );
  vAttachOrLoadPageDescriptionLabel->setWordWrap( QTextEdit::WidgetWidth );

  vALStackPage0Layout->addWidget( vAttachOrLoadPageDescriptionLabel);


//  vpage0sv->viewport()->setBackgroundColor("Yellow");

  //
  // Separate the Intro Text Label from the ScrollView with the load/attach options
  //
  vLAPage0Line1 = new QFrame( vALStackPage0, "vLAPage0Line1" );
  vLAPage0Line1->setMinimumSize( QSize(10,10) );
  vLAPage0Line1->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred, 0, 0, FALSE ) );
  vLAPage0Line1->setFrameShape( QFrame::HLine );
  vLAPage0Line1->setFrameShadow( QFrame::Sunken );
  vALStackPage0Layout->addWidget( vLAPage0Line1 );

  // Add single process label here
  vLAPageTitleLabel1 = new QLabel( vpage0big_box, "vLAPageTitleLabel1" );
  const QColor vpage0acolor = vLAPageTitleLabel1->paletteBackgroundColor();

  vpage0sv->viewport()->setBackgroundColor(vpage0acolor);
  vpage0sv->viewport()->setPaletteBackgroundColor(vpage0acolor);

  vAttachOrLoadPageLoadExecutableCheckBox = new QCheckBox( vpage0big_box, "vAttachOrLoadPageLoadExecutableCheckBox" );
  vAttachOrLoadPageLoadExecutableCheckBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, 0, 0, FALSE ) );

  vAttachOrLoadPageExecutableLabel = new QLabel( vpage0big_box, "vAttachOrLoadPageExecutableLabel" );

  // Add single process load different label here
  vAttachOrLoadPageLoadDifferentExecutableCheckBox = new QCheckBox( vpage0big_box, "vAttachOrLoadPageLoadDifferentExecutableCheckBox" );
  vAttachOrLoadPageLoadDifferentExecutableCheckBox->setChecked(FALSE);
  vAttachOrLoadPageLoadDifferentExecutableCheckBox->hide();
  vAttachOrLoadPageLoadDifferentExecutableCheckBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, 0, 0, FALSE ) );

  // Add single process attach label here
  if (!getInstrumentorIsOffline() ) {
    vAttachOrLoadPageAttachToProcessCheckBox = new QCheckBox( vpage0big_box, "vAttachOrLoadPageAttachToProcessCheckBox" );
    vAttachOrLoadPageAttachToProcessCheckBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, 0, 0, FALSE ) );
   }


//  QVBoxLayout  *_vALStackPage0Layout = new QVBoxLayout( vpage0big_box, 11, 6, "_vALStackPage0Layout"); 
//  QSpacerItem *AlSpacerItem = new QSpacerItem( 1800, 30, QSizePolicy::Maximum, QSizePolicy::Maximum );
//  _vALStackPage0Layout->addItem(AlSpacerItem);

  vAttachOrLoadPageProcessListLabel = new QLabel( vpage0big_box, "vAttachOrLoadPageProcessListLabel" );

  // Add multi-process label here

  vLAPage0SeqMPSeparatorLine = new QFrame( vpage0big_box, "vLAPage0SeqMPSeparatorLine" );
  vLAPage0SeqMPSeparatorLine->setFrameShape( QFrame::HLine );
  vLAPage0SeqMPSeparatorLine->setFrameShadow( QFrame::Sunken );

  vLAPageTitleLabel2 = new QLabel( vpage0big_box, "vLAPageTitleLabel2" );

  // MultiProcess Load Executable Code
  vAttachOrLoadPageLoadMultiProcessExecutableCheckBox = new QCheckBox( vpage0big_box, "vAttachOrLoadPageLoadMultiProcessExecutableCheckBox" );
  vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, 0, 0, FALSE ) );

  vAttachOrLoadPageMultiProcessExecutableLabel = new QLabel( vpage0big_box, "vAttachOrLoadPageMultiProcessExecutableLabel" );

  // MultiProcess Load Different
  vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox = new QCheckBox( vpage0big_box, "vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox" );
  vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->setChecked(FALSE);
  vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->hide();
  vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, 0, 0, FALSE ) );

  // MultiProcess Attach
  if (!getInstrumentorIsOffline() ) {
     vAttachOrLoadPageAttachToMultiProcessCheckBox = new QCheckBox( vpage0big_box, "vAttachOrLoadPageAttachToMultiProcessCheckBox" );
     vAttachOrLoadPageAttachToMultiProcessCheckBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, 0, 0, FALSE ) );
  }

  vAttachOrLoadPageMultiProcessListLabel = new QLabel( vpage0big_box, "vAttachOrLoadPageMultiProcessListLabel" );
  vAttachOrLoadPageMultiProcessListLabel->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, 0, 0, FALSE ) );

  QLabel *dummySpacer = new QLabel( vpage0big_box, "dummySpacer" );

  vALStackPage0Layout->addWidget( vpage0sv);

  vLAPage0Line2 = new QFrame( vALStackPage0, "vLAPage0Line2" );
  vLAPage0Line2->setMinimumSize( QSize(10,10) );
  vLAPage0Line2->setFrameShape( QFrame::HLine );
  vLAPage0Line2->setFrameShadow( QFrame::Sunken );
  vLAPage0Line2->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, 0, 0, FALSE ) );
  vALStackPage0Layout->addWidget( vLAPage0Line2 );

  vAttachOrLoadPageSpacer = new QSpacerItem( 20, 10, QSizePolicy::Expanding, QSizePolicy::Expanding );
  vALStackPage0Layout->addItem( vAttachOrLoadPageSpacer );


  //
  // BUTTON LAYOUT
  //

  vAttachOrLoadPageButtonLayout = new QHBoxLayout( 0, 0, 6, "vAttachOrLoadPageButtonLayout"); 

  vAttachOrLoadPageButtonSpacer = new QSpacerItem( 1, 10, QSizePolicy::Expanding, QSizePolicy::Fixed );
  vAttachOrLoadPageButtonLayout->addItem( vAttachOrLoadPageButtonSpacer );

  vAttachOrLoadPageClearButton = new QPushButton( vALStackPage0, "vAttachOrLoadPageClearButton" );
  vAttachOrLoadPageClearButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vAttachOrLoadPageClearButton->sizePolicy().hasHeightForWidth() ) );
  vAttachOrLoadPageButtonLayout->addWidget( vAttachOrLoadPageClearButton );

  vAttachOrLoadPageBackButton = new QPushButton( vALStackPage0, "vAttachOrLoadPageBackButton" );
  vAttachOrLoadPageBackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vAttachOrLoadPageBackButton->sizePolicy().hasHeightForWidth() ) );
  vAttachOrLoadPageBackButton->setEnabled(TRUE);
  vAttachOrLoadPageButtonLayout->addWidget( vAttachOrLoadPageBackButton );

  vAttachOrLoadPageNextButton = new QPushButton( vALStackPage0, "vAttachOrLoadPageNextButton" );
  vAttachOrLoadPageNextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vAttachOrLoadPageNextButton->sizePolicy().hasHeightForWidth() ) );
  vAttachOrLoadPageButtonLayout->addWidget( vAttachOrLoadPageNextButton );

  vAttachOrLoadPageFinishButton = new QPushButton( vALStackPage0, "vAttachOrLoadPageFinishButton" );
  vAttachOrLoadPageFinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vAttachOrLoadPageFinishButton->sizePolicy().hasHeightForWidth() ) );
  vAttachOrLoadPageButtonLayout->addWidget( vAttachOrLoadPageFinishButton );
  vAttachOrLoadPageFinishButton->setEnabled(FALSE);

  vALStackPage0Layout->addLayout( vAttachOrLoadPageButtonLayout );
  mainWidgetStack->addWidget( vALStackPage0, 1 );

  vpage0big_box->adjustSize();


#ifdef DEBUG_loadPanel
  printf("vpage0big_box->width()=%d\n", vpage0big_box->width());
  printf("vpage0sv->height()=%d\n", vpage0sv->height());
  printf("vpage0sv->width()=%d\n", vpage0sv->width());
  printf("vpage0sv->viewport()->width()=%d\n", vpage0sv->viewport()->width());
#endif

  vpage0big_box->resize(vpage0sv->width(), vpage0sv->height());


#ifdef DEBUG_loadPanel
  printf("after resize, vpage0big_box->width()=%d\n", vpage0big_box->width());
#endif

// End: AttachOrLoad page

// ----------------------------------------------------------
// Begin: Load Page ///  Page 2 of Load/Attach Wizard -------
// ----------------------------------------------------------


  vMPStackPage1 = new QWidget( mainWidgetStack, "vMPStackPage1" );
//  vMPStackPage1->setMinimumSize( QSize(100,100) );

  vMPStackPage1Layout = new QVBoxLayout( vMPStackPage1, 7, 4, "vMPStackPage1Layout"); 
//  vMPStackPage1Layout = new QVBoxLayout( vMPStackPage1, 11, 6, "vMPStackPage1Layout"); 

  vpage1sv = new QScrollView( vMPStackPage1, "vpage1sv" );

  vpage1big_box = new QVBox( vpage1sv->viewport(), "vpage1big_box" );
  vpage1big_box->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum, 0, 0, FALSE ) );
//  vpage1big_box->setPaletteForegroundColor("Dark Green");
  vpage1sv->addChild(vpage1big_box);

//  vpage1big_box->setPaletteBackgroundColor("Tan");
//  vpage1sv->viewport()->setBackgroundColor("Yellow");

  const QColor vpageColor = vLAPageTitleLabel2->paletteBackgroundColor();
  vpage1sv->viewport()->setBackgroundColor(vpageColor);
  vpage1sv->viewport()->setPaletteBackgroundColor(vpageColor);
//  vpage1sv->viewport()->setPaletteBackgroundColor("Light Blue");

  // Line above big_box
  vMPLoadLine = new QFrame( vMPStackPage1, "vMPLoadLine" );
  vMPLoadLine->setMinimumSize( QSize(10,10) );
  vMPLoadLine->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred, 0, 0, FALSE ) );
  vMPLoadLine->setFrameShape( QFrame::HLine );
  vMPLoadLine->setFrameShadow( QFrame::Sunken );
  vMPStackPage1Layout->addWidget( vMPLoadLine );

  // Create a MPI job command ... header label
  vMPLoadTitleLabel1 = new QLabel( vMPStackPage1, "vMPLoadTitleLabel1" );
  vMPLoadTitleLabel1->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, 0, 0, FALSE ) );
  vMPStackPage1Layout->addWidget( vMPLoadTitleLabel1 );

  vMPLoadTitleLabel2 = new QLabel( vMPStackPage1, "vMPLoadTitleLabel2" );
  vMPLoadTitleLabel2->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, 0, 0, FALSE ) );
  vMPStackPage1Layout->addWidget( vMPLoadTitleLabel2 );


  // Create the parallel execution extra widget entities - parallel execution prefix label and text entry form  
  // Step 1: Enter MPI .... text for this label

  vMPLoadParallelPrefixLabel = new QLabel( vpage1big_box, "vMPLoadParallelPrefixLabel" );
  vMPLoadParallelPrefixLabel->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, 0, 0, FALSE ) );
  vMPLoadParallelPrefixLineedit = new QLineEdit( vpage1big_box );

  // Add the parallel execution text entry and label to the dialog form.

  vMPLoadPageLoadLabel = new QLabel( vpage1big_box, "vMPLoadPageLoadLabel");

  vMPLoadPageLoadButton = new QPushButton( vpage1big_box, "vMPLoadPageLoadButton");
  vMPLoadPageLoadButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, FALSE ) );

  vMPLoadMPILoadLineedit = new QLineEdit( vpage1big_box );
  vMPLoadMPILoadLineedit->setMinimumSize( QSize(631,10) );
//  vMPLoadMPILoadLineedit->setReadOnly(TRUE);

  // Add the parallel execution text entry and label to the dialog form.

  // Create the first extra widget entity - command line agrument label and text entry form
  vMPLoadCommandArgumentsLabel = new QLabel(vpage1big_box, "vMPLoadCommandArgumentsLabel" );
  vMPLoadCommandArgumentsLabel->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, 0, 0, FALSE ) );

  vMPLoadPageArgBrowseButton = new QPushButton( vpage1big_box, "vMPLoadPageArgBrowseButton");
  vMPLoadPageArgBrowseButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, FALSE ) );

  vMPLoadCommandArgumentsLineedit = new QLineEdit( vpage1big_box );

  // Add the entities to the dialog form


#ifdef SHOWCOMMAND_ENABLED
  //  Show the command 
  vMPLoadMPIShowCommandLabel = new QLabel( vpage1big_box, "vMPLoadMPIShowCommandLabel" );
  vMPLoadMPIShowCommandLabel->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, 0, 0, FALSE ) );

  vMPLoadPageShowButton = new QPushButton( vpage1big_box, "vMPLoadPageShowButton");
  vMPLoadPageShowButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, FALSE ) );

  const QColor vpage0color = vMPStackPage1->paletteBackgroundColor();

  vMPLoadMPICommandLineedit = new QLineEdit( vpage1big_box );
  vMPLoadMPICommandLineedit->setReadOnly(TRUE);

#endif

  vMPLoadMPIAcceptCommandLabel = new QLabel( vpage1big_box, "vMPLoadMPIAcceptCommandLabel" );
  vMPLoadMPIAcceptCommandLabel->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, 0, 0, FALSE ) );

  vMPStackPage1Layout->addWidget( vpage1sv);

  vMPLoadLine1 = new QFrame( vMPStackPage1, "vMPLoadLine1" );
  vMPLoadLine1->setMinimumSize( QSize(10,10) );
  vMPLoadLine1->setFrameShape( QFrame::HLine );
  vMPLoadLine1->setFrameShadow( QFrame::Sunken );
  vMPStackPage1Layout->addWidget( vMPLoadLine1 );

  vMPLoadSpacer = new QSpacerItem( 10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding );
  vMPStackPage1Layout->addItem( vMPLoadSpacer );

  vMPLoadButtonLayout = new QHBoxLayout( 0, 0, 6, "vMPLoadButtonLayout"); 

  vMPLoadButtonSpacer = new QSpacerItem( 251, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );
  vMPLoadButtonLayout->addItem( vMPLoadButtonSpacer );

  vMPLoadPageClearButton = new QPushButton( vMPStackPage1, "vMPLoadPageClearButton" );
  vMPLoadPageClearButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vMPLoadPageClearButton->sizePolicy().hasHeightForWidth() ) );
  vMPLoadButtonLayout->addWidget( vMPLoadPageClearButton );

  vMPLoadPageBackButton = new QPushButton( vMPStackPage1, "vMPLoadPageBackButton" );
  vMPLoadPageBackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vMPLoadPageBackButton->sizePolicy().hasHeightForWidth() ) );
  vMPLoadButtonLayout->addWidget( vMPLoadPageBackButton );

  vMPLoadPageNextButton = new QPushButton( vMPStackPage1, "vMPLoadPageNextButton" );
  vMPLoadPageNextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vMPLoadPageNextButton->sizePolicy().hasHeightForWidth() ) );
  // Not going to be used but for consistency keep it's position (usability study result)
  vMPLoadPageNextButton->setEnabled(FALSE);
  vMPLoadButtonLayout->addWidget( vMPLoadPageNextButton );

  vMPLoadPageFinishButton = new QPushButton( vMPStackPage1, "vMPLoadPageFinishButton" );
  vMPLoadPageFinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vMPLoadPageFinishButton->sizePolicy().hasHeightForWidth() ) );
  vMPLoadButtonLayout->addWidget( vMPLoadPageFinishButton );

  vMPStackPage1Layout->addLayout( vMPLoadButtonLayout );
  mainWidgetStack->addWidget( vMPStackPage1, 1 );

  mainFrameLayout->addWidget( mainWidgetStack );
  loadPanelFormLayout->addWidget( mainFrame );
//  vpage1big_box->adjustSize();

#ifdef DEBUG_loadPanel
  printf("vpage1big_box->width()=%d\n", vpage1big_box->width());
  printf("vpage1sv->height()=%d\n", vpage1sv->height());
  printf("vpage1sv->width()=%d\n", vpage1sv->width());
  printf("vpage1sv->viewport()->width()=%d\n", vpage1sv->viewport()->width());
#endif

//  vpage1big_box->layout()->setResizeMode(QLayout::Minimum);
//  vpage1big_box->resize(vpage1sv->width(), vpage1sv->height());


#ifdef DEBUG_loadPanel
  printf("after resize, vpage1big_box->width()=%d\n", vpage1big_box->width());
#endif

// End: AttachOrLoad page
// -- --- End New attach load page

  languageChange();
  resize( QSize(631, 508).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  connect( vAttachOrLoadPageBackButton, SIGNAL( clicked() ), this,
           SLOT( vAttachOrLoadPageBackButtonSelected() ) );

  connect( vAttachOrLoadPageClearButton, SIGNAL( clicked() ), this,
           SLOT( vAttachOrLoadPageClearButtonSelected() ) );

  if (!getInstrumentorIsOffline() ) {
    connect( vAttachOrLoadPageAttachToMultiProcessCheckBox, SIGNAL( clicked() ), this,
             SLOT( vAttachOrLoadPageAttachToMultiProcessCheckBoxSelected() ) );
  }

  connect( vAttachOrLoadPageLoadMultiProcessExecutableCheckBox, SIGNAL( clicked() ), this,
           SLOT( vAttachOrLoadPageLoadMultiProcessExecutableCheckBoxSelected() ) );

  connect( vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox,
           SIGNAL( clicked() ), this,
           SLOT( vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBoxSelected() ) );

  if (!getInstrumentorIsOffline() ) {
     connect( vAttachOrLoadPageAttachToProcessCheckBox, SIGNAL( clicked() ), this,
              SLOT( vAttachOrLoadPageAttachToProcessCheckBoxSelected() ) );
  }

  connect( vAttachOrLoadPageLoadExecutableCheckBox, SIGNAL( clicked() ), this,
           SLOT( vAttachOrLoadPageLoadExecutableCheckBoxSelected() ) );

  connect( vAttachOrLoadPageLoadDifferentExecutableCheckBox,
           SIGNAL( clicked() ), this,
           SLOT( vAttachOrLoadPageLoadDifferentExecutableCheckBoxSelected() ) );

  connect( vAttachOrLoadPageNextButton, SIGNAL( clicked() ), this,
           SLOT( vAttachOrLoadPageNextButtonSelected() ) );

  connect( vAttachOrLoadPageFinishButton, SIGNAL( clicked() ), this,
           SLOT( finishButtonSelected() ) );

  connect( vMPLoadPageBackButton, SIGNAL( clicked() ), this,
           SLOT( vMPLoadPageBackButtonSelected() ) );

  connect( vMPLoadPageClearButton, SIGNAL( clicked() ), this,
           SLOT( vMPLoadPageClearButtonSelected() ) );

  connect( vMPLoadPageFinishButton, SIGNAL( clicked() ), this,
           SLOT( vMPLoadPageFinishButtonSelected() ) );

  connect( vMPLoadPageLoadButton, SIGNAL( clicked() ), this,
           SLOT( vMPLoadPageLoadButtonSelected() ) );

#ifdef SHOWCOMMAND_ENABLED
  connect( vMPLoadPageShowButton, SIGNAL( clicked() ), this,
           SLOT( vMPLoadPageShowButtonSelected() ) );
#endif

  connect( vMPLoadPageArgBrowseButton, SIGNAL( clicked() ), this,
           SLOT( vMPLoadPageArgBrowseSelected() ) );

#if 0
  // jeg-09-27-07 RETHINK THIS - coming in from Experiment menu
  // now and as a separate entity wizard.  But we want this button
  // enabled for the wizard cases and this check is causing it to
  // be disabled in the wizard cases -- unfortunately.  Commenting
  // out and then will fix when all the cases are flushed out.
  if( ao && ao->int_data == 0 )
  {
    // This wizard panel was brought up explicitly.   Don't
    // enable the hook to go back to the IntroWizardPanel.
    vAttachOrLoadPageBackButton->setEnabled(FALSE);
  }
#endif

  // Set up the first "wizard" page for the load
  // Initializes everything
  vUpdateAttachOrLoadPageWidget();

  vLAPage0SeqMPSeparatorLine->setMinimumSize( QSize(1000,10) );
//  vLAPage0SeqMPSeparatorLine->setMinimumSize( QSize(1500,10) );
  mainWidgetStack->raiseWidget(vALStackPage0);
  getBaseWidgetFrame()->setCaption("loadPanelBaseWidget");

#ifdef DEBUG_loadPanel
  printf("loadPanel::loadPanel(), constructor exited\n");
#endif


}


loadPanel::~loadPanel()
{
#ifdef DEBUG_loadPanel
  printf("loadPanel::~loadPanel(), entered\n");
#endif
  // Delete anything you new'd from the constructor.
}

void
loadPanel::languageChange()
{

#ifdef DEBUG_loadPanel
  printf("loadPanel::languageChange(), entered\n");
#endif

  setCaption( tr( "Load Program Panel" ) );


  if (!getInstrumentorIsOffline() ) {
    vAttachOrLoadPageDescriptionLabel->setText( tr( "Open|SpeedShop can attach to an existing running process (or running processes) or load an executable from disk.  It can also start a multiprocess job from disk or automatically attach to all the running processes/ranks of a MPI job.  Please select the desired action.  Note: A dialog will be raised, prompting for the information needed to load an executable or attach to a process.") );
    vAttachOrLoadPageAttachToProcessCheckBox->setText( tr( "Attach to one or more already running processes/ranks/threads." ) );
    vAttachOrLoadPageAttachToMultiProcessCheckBox->setText( tr( "Attach to all processes/ranks/threads in an already running multiprocess job (MPI)." ) );
  } else {
    vAttachOrLoadPageDescriptionLabel->setText( tr( "Open|SpeedShop can load an executable from disk.  It can also start a multiprocess job from disk.  Please select the desired action.  Note: A dialog will be raised, prompting for the information needed to load an executable.") );
  }
  vAttachOrLoadPageLoadExecutableCheckBox->setText( tr( "Load an executable from disk." ) );
  vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setText( tr( "Start/Run a multi-process executable from disk (MPI)." ) );
  vAttachOrLoadPageLoadDifferentExecutableCheckBox->setText( tr( "Load a different executable from disk." ) );
  vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->setText( tr( "Start/Run a different multi-process executable from disk (MPI)." ) );

  vAttachOrLoadPageBackButton->setText( tr( "< Back" ) );
  QToolTip::add( vAttachOrLoadPageBackButton, tr( "Takes you back one page." ) );
  vAttachOrLoadPageClearButton->setText( tr( "Reset" ) );
  QToolTip::add( vAttachOrLoadPageClearButton, tr( "This resets all settings restoring them to system defaults." ) );
  vAttachOrLoadPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( vAttachOrLoadPageNextButton, tr( "Advance to the next wizard page." ) );
  vAttachOrLoadPageFinishButton->setText( tr( ">> Load/Attach" ) );
  QToolTip::add( vAttachOrLoadPageFinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );


  // Set language specific information here.
  vMPLoadPageBackButton->setText( tr( "< Back" ) );
  QToolTip::add( vMPLoadPageBackButton, tr( "Takes you back one page." ) );
  vMPLoadPageClearButton->setText( tr( "Reset" ) );
  QToolTip::add( vMPLoadPageClearButton, tr( "This resets all settings restoring them to system defaults." ) );
  vMPLoadPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( vMPLoadPageNextButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  vMPLoadPageFinishButton->setText( tr( ">> Load" ) );
  QToolTip::add( vMPLoadPageFinishButton, tr( "Load the multiprocess executable." ) );
  vMPLoadParallelPrefixLabel->setText( tr("<b>Step 1:</b> Enter MPI Execution Prefix: (For example: mpirun -np 64 or srun -N 128 -n 128)") );
  vMPLoadCommandArgumentsLabel->setText( tr("<b>Step 3:</b> Enter MPI program command line arguments: ") );

#ifdef SHOWCOMMAND_ENABLED
  vMPLoadMPIShowCommandLabel->setText(tr("<b>Optional Step 4:</b> Review/Show the MPI Command Open|SpeedShop is building:") );
  vMPLoadMPIAcceptCommandLabel->setText(tr("<b>Step 5:</b> When you are ready to accept the MPI Command Open|SpeedShop has built, click on the Finish button to load the multiprocess program.") );
  vMPLoadPageShowButton->setText( tr("Review") );
  QToolTip::add( vMPLoadPageShowButton, tr( "Review the current MPI command that Open|SpeedShop is building." ) );
#else
  vMPLoadMPIAcceptCommandLabel->setText(tr("<b>Step 4:</b> When you are ready to accept the MPI Command Open|SpeedShop has built, click on the Finish button to load the multiprocess program.") );
#endif
  vMPLoadPageLoadLabel->setText( tr("<b>Step 2:</b> Select the MPI executable you want to analyze:") );
  vMPLoadPageLoadButton->setText( tr("Browse") );
  vMPLoadPageArgBrowseButton->setText( tr("Browse") );
  QToolTip::add( vMPLoadPageArgBrowseButton, tr( "Raise a dialog box to choose an argument file." ) );
  QToolTip::add( vMPLoadPageLoadButton, tr( "Raise a dialog box to choose the MPI executable to run and analyze." ) );

  vLAPageTitleLabel1->setText( tr( "<h4>Use O|SS on Sequential/Single Process Code</h4>" ) );
  vLAPageTitleLabel2->setText( tr( "<h4>Use O|SS on MPI Code</h4>" ) );

  vMPLoadTitleLabel1->setText( tr( "<h4>Create a MPI job command including your application selection, by following these steps:</h4>" ) );
  vMPLoadTitleLabel2->setText( tr( "\n" ) );


#ifdef DEBUG_loadPanel
  printf("loadPanel::languageChanged() , exit\n");
#endif
}

void loadPanel::vAttachOrLoadPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vAttachOrLoadPageBackButtonSelected() \n");
#ifdef DEBUG_loadPanel
  printf("loadPanel::vAttachOrLoadPageBackButtonSelected() entered \n");
#endif

  Panel *p = targetPanel;
#ifdef DEBUG_loadPanel
  printf("loadPanel::loadPanel(), this=0x%x, (targetPanel is read for Wizard_Raise_LoadPanel_Back_Page) panel name is=%s\n", this, targetPanel->getName());
#endif
  MessageObject *msg = new MessageObject("Wizard_Raise_LoadPanel_Back_Page");
#ifdef DEBUG_loadPanel
  printf("loadPanel::vAttachOrLoadPageBackButtonSelected() processing Wizard_Raise_LoadPanel_Back_Page msg, this=0x%x, p=0x%x\n", this, p);
#endif
  if (p) {
    p->getPanelContainer()->raisePanel(p);
    p->listener((void *)msg);
#ifdef DEBUG_loadPanel
    printf("loadPanel::vAttachOrLoadPageBackButtonSelected(), panel to raise for BACK found , p=0x%x\n", p);
#endif
  } else {
#ifdef DEBUG_loadPanel
  printf("loadPanel::vAttachOrLoadPageBackButtonSelected(), panel to raise for BACK button not found \n");
#endif
  }
  delete msg;

#ifdef DEBUG_loadPanel
  printf("loadPanel::vAttachOrLoadPageBackButtonSelected() exitted \n");
#endif
}


void loadPanel::vAttachOrLoadPageClearButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vAttachOrLoadPageClearButtonSelected() \n");
#ifdef DEBUG_loadPanel
  printf("loadPanel::vAttachOrLoadPageClearButtonSelected() \n");
#endif

  vAttachOrLoadPageLoadDifferentExecutableCheckBox->setChecked(FALSE);
  vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->setChecked(FALSE);
  if (getMPIWizardCalledMe()) {
    vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setChecked(TRUE);
    vAttachOrLoadPageLoadExecutableCheckBox->setChecked(FALSE);
  } else {
    vAttachOrLoadPageLoadExecutableCheckBox->setChecked(TRUE);
    vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setChecked(FALSE);
  }

  if (!getInstrumentorIsOffline() ) {
    vAttachOrLoadPageAttachToProcessCheckBox->setChecked(FALSE);
    vAttachOrLoadPageAttachToMultiProcessCheckBox->setChecked(FALSE);
  }

  if( getPanelContainer()->getMainWindow() )
  {
    OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
    if( mw )
    {
      mw->executableName = QString::null;
      mw->pidStr = QString::null;
    }
  }
  vUpdateAttachOrLoadPageWidget();
}

void loadPanel::vAttachOrLoadPageAttachToMultiProcessCheckBoxSelected()
{
#ifdef DEBUG_loadPanel
  printf("loadPanel::vAttachOrLoadPageAttachToMultiProcessCheckBoxSelected() \n");
#endif
  if( vAttachOrLoadPageAttachToMultiProcessCheckBox->isChecked() ) {
    vAttachOrLoadPageLoadExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadDifferentExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageAttachToProcessCheckBox->setChecked(FALSE);
  }
}

void loadPanel::vAttachOrLoadPageAttachToProcessCheckBoxSelected()
{
#ifdef DEBUG_loadPanel
  printf("loadPanel::vAttachOrLoadPageAttachToProcessCheckBoxSelected() \n");
#endif
  if( vAttachOrLoadPageAttachToProcessCheckBox->isChecked() ) {
    vAttachOrLoadPageLoadExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadDifferentExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageAttachToMultiProcessCheckBox->setChecked(FALSE);
  }
}

void loadPanel::vAttachOrLoadPageLoadExecutableCheckBoxSelected()
{
#ifdef DEBUG_loadPanel
  printf("loadPanel::vAttachOrLoadPageLoadExecutableCheckBoxSelected() \n");
#endif
  if( vAttachOrLoadPageLoadExecutableCheckBox->isChecked() )
  {
    vAttachOrLoadPageLoadDifferentExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setChecked(FALSE);
    if (!getInstrumentorIsOffline() ) {
       vAttachOrLoadPageAttachToMultiProcessCheckBox->setChecked(FALSE);
       vAttachOrLoadPageAttachToProcessCheckBox->setChecked(FALSE);
    }
  }
}

void loadPanel::vAttachOrLoadPageLoadMultiProcessExecutableCheckBoxSelected()
{
#ifdef DEBUG_loadPanel
  printf("loadPanel::vAttachOrLoadPageLoadMultiProcessExecutableCheckBoxSelected() \n");
#endif
  if( vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->isChecked() ) {
    vAttachOrLoadPageLoadDifferentExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadExecutableCheckBox->setChecked(FALSE);
    if (!getInstrumentorIsOffline() ) {
       vAttachOrLoadPageAttachToMultiProcessCheckBox->setChecked(FALSE);
       vAttachOrLoadPageAttachToProcessCheckBox->setChecked(FALSE);
    }
  }
}

void loadPanel::vAttachOrLoadPageLoadDifferentExecutableCheckBoxSelected()
{
#ifdef DEBUG_loadPanel
  printf("loadPanel::vAttachOrLoadPageLoadDifferentExecutableCheckBoxSelected() \n");
#endif
  if( vAttachOrLoadPageLoadDifferentExecutableCheckBox->isChecked() ) {
    vAttachOrLoadPageLoadExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setChecked(FALSE);
    if (!getInstrumentorIsOffline() ) {
       vAttachOrLoadPageAttachToMultiProcessCheckBox->setChecked(FALSE);
       vAttachOrLoadPageAttachToProcessCheckBox->setChecked(FALSE);
    }
  }
}

void loadPanel::vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBoxSelected()
{
#ifdef DEBUG_loadPanel
  printf("loadPanel::vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBoxSelected() \n");
#endif
  if( vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->isChecked() )
  {
    vAttachOrLoadPageLoadExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadDifferentExecutableCheckBox->setChecked(FALSE);
    vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setChecked(FALSE);
    if (!getInstrumentorIsOffline() ) {
       vAttachOrLoadPageAttachToMultiProcessCheckBox->setChecked(FALSE);
       vAttachOrLoadPageAttachToProcessCheckBox->setChecked(FALSE);
    }
  }
}

static void clearPreviousSettings(OpenSpeedshop *mw)
{
  if( mw ) {
    mw->parallelPrefixCommandStr = QString::null;
    mw->executableName = QString::null;
    mw->pidStr = QString::null;
    mw->argsStr = QString::null;
  }
}

void loadPanel::vAttachOrLoadPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vAttachOrLoadPageNextButtonSelected() \n");
#ifdef DEBUG_loadPanel
  printf("loadPanel::vAttachOrLoadPageNextButtonSelected() \n");
  printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), vAttachOrLoadPageLoadExecutableCheckBox->isChecked()=%d \n",
         vAttachOrLoadPageLoadExecutableCheckBox->isChecked() );
  printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->isChecked()=%d \n",
         vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->isChecked() );
  printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->isChecked()=%d \n",
         vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->isChecked() );
  if (!getInstrumentorIsOffline() ) {
    printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), vAttachOrLoadPageAttachToMultiProcessCheckBox->isChecked()=%d \n",
           vAttachOrLoadPageAttachToMultiProcessCheckBox->isChecked() );
    printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), vAttachOrLoadPageAttachToProcessCheckBox->isChecked()=%d \n",
           vAttachOrLoadPageAttachToProcessCheckBox->isChecked() );
  }
  printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), vAttachOrLoadPageLoadDifferentExecutableCheckBox->isChecked()=%d \n",
         vAttachOrLoadPageLoadDifferentExecutableCheckBox->isChecked() );
#endif

  char buffer[2048];

  if (!getInstrumentorIsOffline()) {
     if( !vAttachOrLoadPageAttachToProcessCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadExecutableCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->isChecked() &&
         !vAttachOrLoadPageAttachToMultiProcessCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadDifferentExecutableCheckBox->isChecked() )
     {
       QString msg = QString("You must either select the option to attach to an \nexisting process or load an executable.  Please select one.\n");
       QMessageBox::information( (QWidget *)this, "Process or executable needed...",
                                  msg, QMessageBox::Ok );
    
       return;
     }
  } else {
     if( !vAttachOrLoadPageLoadExecutableCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadDifferentExecutableCheckBox->isChecked() )
     {
       QString msg = QString("You must either select the option to load an executable.  Please select one.\n");
       QMessageBox::information( (QWidget *)this, "Process or executable needed...",
                                  msg, QMessageBox::Ok );
    
       return;
     }

  } 

  if (!getInstrumentorIsOffline()) {
     if( vAttachOrLoadPageAttachToProcessCheckBox->isChecked() &&
         vAttachOrLoadPageLoadExecutableCheckBox->isChecked() )
     {
       QString msg = QString("From this wizard you can only select to either attach or load.  Please select only one.\n");
       QMessageBox::information( (QWidget *)this, "Process or executable needed...",
                                  msg, QMessageBox::Ok );
    
       return;
     }
   } 

  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();

  if( !mw ) {
#ifdef DEBUG_loadPanel
    printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), vAttachOrLoadPageNextButtonSelected, mw=%d return\n", mw);
#endif
    return;
  } 

  // New MultiProcess duplicate
  if( !getInstrumentorIsOffline() && vAttachOrLoadPageAttachToMultiProcessCheckBox->isChecked() ) { 

    if( mw->pidStr.isEmpty() ) {
#ifdef DEBUG_loadPanel
      printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), vAttachOrLoadPageNextButtonSelected, calling attachNewMultiProcess\n");
#endif
      clearPreviousSettings(mw);
      mw->attachNewMultiProcess();
    }

#ifdef DEBUG_loadPanel
    printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), after attachNewMultiProcess, calling vMPLoadPageProcessAccept() \n");
#endif
    vMPLoadPageProcessAccept();

  }

  // Sequential attach

  if( !getInstrumentorIsOffline() && vAttachOrLoadPageAttachToProcessCheckBox->isChecked() ) {

    if( mw->pidStr.isEmpty() ) {

#ifdef DEBUG_loadPanel
      printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), calling attachNewProcess\n");
#endif
      clearPreviousSettings(mw);
      mw->attachNewProcess();
    }
#ifdef DEBUG_loadPanel
    printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), after attachNewProcess, calling vMPLoadPageProcessAccept() \n");
#endif
    vMPLoadPageProcessAccept();

  }

  // New MultiProcess load duplicate of existing
  if( vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->isChecked() ||
      vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->isChecked() ) {

#ifdef DEBUG_loadPanel
    printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), vAttachOrLoadPageNextButtonSelected, want to load multiprocess executable raiseWidget(vMPStackPage1)\n");
#endif
    mainWidgetStack->raiseWidget(vMPStackPage1);

  }

  // Existing sequential load executable code
  if( vAttachOrLoadPageLoadExecutableCheckBox->isChecked() ||
      vAttachOrLoadPageLoadDifferentExecutableCheckBox->isChecked() ) {

    if( mw->executableName.isEmpty() ||
        vAttachOrLoadPageLoadDifferentExecutableCheckBox->isChecked() ) {

#ifdef DEBUG_loadPanel
      printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), Load the QFile \n");
#endif
      clearPreviousSettings(mw);
      mw->loadNewProgram();

    }

#ifdef DEBUG_loadPanel
    printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), after loadNewProgram, calling vMPLoadPageProcessAccept() \n");
#endif

    if( !mw->executableName.isEmpty() ) {
       vMPLoadPageProcessAccept();
    }

  }

#ifdef DEBUG_loadPanel
  printf("loadPanel::vAttachOrLoadPageNextButtonSelected(), exitted \n");
#endif


}


// End verbose AttachOrLoad callbacks

void loadPanel::vMPLoadPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vMPLoadPageBackButtonSelected() \n");

#ifdef DEBUG_loadPanel
  printf("vMPLoadPageBackButtonSelected(), raise vALStackPage0 \n");
#endif

  mainWidgetStack->raiseWidget(vALStackPage0);
}

void loadPanel::vMPLoadPageAcceptButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vMPLoadPageAcceptButtonSelected() \n");

#ifdef DEBUG_loadPanel
  printf("vMPLoadPageAcceptButtonSelected() called TBD\n");
#endif

  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
  if( !mw ) {
    return;
  }
  if(!mw->executableName.isEmpty() ) {
    mw->parallelPrefixCommandStr = vMPLoadParallelPrefixLineedit->text().ascii();
  }
  vAttachOrLoadPageNextButtonSelected();

}

#ifdef SHOWCOMMAND_ENABLED
void loadPanel::vMPLoadPageShowButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vMPLoadPageShowButtonSelected() \n");

#ifdef DEBUG_loadPanel
  printf("vMPLoadPageShowButtonSelected() called\n");
#endif

  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
  if( !mw ) {
    return;
  }
  if(!mw->executableName.isEmpty() ) {

#ifdef DEBUG_loadPanel
    printf("vMPLoadPageShowButtonSelected(), mw->executableName.ascii()=%s\n",
            mw->executableName.ascii() );
#endif

  }

  QString allOfIt = QString::null;

  if ( !vMPLoadParallelPrefixLineedit->text().isEmpty() ) {
    allOfIt.append(vMPLoadParallelPrefixLineedit->text().ascii()) ;
    allOfIt.append(" ");
  }

  if ( !vMPLoadPageLoadMPProgName.isEmpty() ) {
    allOfIt.append(vMPLoadPageLoadMPProgName.ascii() );
    allOfIt.append(" ");
  }

  if (!vMPLoadCommandArgumentsLineedit->text().isEmpty()) {
    allOfIt.append(vMPLoadCommandArgumentsLineedit->text().ascii());
    allOfIt.append(" ");
  }

  vMPLoadMPICommandLineedit->setText( allOfIt.ascii() );

//  vMPLoadMPICommandLineedit->setText(vMPLoadParallelPrefixLineedit->text());

#ifdef DEBUG_loadPanel
  if ( !vMPLoadParallelPrefixLineedit->text().isEmpty() ) {
    printf("vMPLoadPageShowButtonSelected(), vMPLoadParallelPrefixLineedit->text()=%s\n",
          vMPLoadParallelPrefixLineedit->text().ascii() );
  }
#endif

//  vMPLoadMPICommandLineedit->setText(vMPLoadPageLoadMPProgName.ascii());

#ifdef DEBUG_loadPanel
  if ( !vMPLoadPageLoadMPProgName.isEmpty() ) {
    printf("vMPLoadPageShowButtonSelected(), vMPLoadPageLoadMPProgName.ascii()=%s\n",
          vMPLoadPageLoadMPProgName.ascii() );
  }
#endif

//  vMPLoadMPICommandLineedit->setText(vMPLoadCommandArgumentsLineedit->text());

#ifdef DEBUG_loadPanel
  if ( !vMPLoadCommandArgumentsLineedit->text().isEmpty() ) {
     printf("vMPLoadPageShowButtonSelected(), vMPLoadCommandArgumentsLineedit->text()=%s\n",
          vMPLoadCommandArgumentsLineedit->text().ascii() );
  }
#endif


}
#endif

void loadPanel::vMPLoadPageLoadButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vMPLoadPageLoadButtonSelected() \n");

#ifdef DEBUG_loadPanel
  printf("vMPLoadPageLoadButtonSelected() called TBD\n");
#endif

  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
  if( !mw ) {
    return;
  }
  mw->loadNewProgram();

  if( mw->executableName.isEmpty() ) {
     // Fix me
     return;
  } else {

#ifdef DEBUG_loadPanel
   if ( !mw->executableName.isEmpty() ) {
      printf("vMPLoadPageLoadButtonSelected(), mw->executableName.ascii()=%s\n", mw->executableName.ascii() );
   }
#endif

   vMPLoadPageLoadMPProgName = mw->executableName.ascii();
   vMPLoadMPILoadLineedit->setText(mw->executableName.ascii());
#ifdef DEBUG_loadPanel
   printf("loadPanel::vMPLoadPageLoadButtonSelected(), mw->argsStr.isEmpty()=%d\n",
          mw->argsStr.isEmpty() );
   if (!mw->argsStr.isEmpty() ) {
      if ( !vMPLoadCommandArgumentsLineedit->text().isEmpty() ) {
       printf("loadPanel::vMPLoadPageLoadButtonSelected(), vMPLoadCommandArgumentsLineedit->text()=%s\n",
              vMPLoadCommandArgumentsLineedit->text().ascii() );
      }
   }
#endif
   if( !mw->argsStr.isEmpty() ) {
      vMPLoadCommandArgumentsLineedit->setText( mw->argsStr.ascii() );;

#ifdef DEBUG_loadPanel
      if ( !vMPLoadCommandArgumentsLineedit->text().isEmpty() ) {
         printf("loadPanel::vMPLoadPageLoadButtonSelected(), vMPLoadCommandArgumentsLineedit->text()=%s\n",
                vMPLoadCommandArgumentsLineedit->text().ascii() );
      }
#endif
   }

  } // end else clause

}

void loadPanel::vMPLoadPageArgBrowseSelected()
{
  nprintf(DEBUG_PANELS) ("vMPLoadPageArgBrowseSelected() \n");
#ifdef DEBUG_loadPanel
  printf("vMPLoadPageArgBrowseSelected() called TBD\n");
#endif

  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
  if( !mw ) {
    return;
  }
  mw->loadArgumentFile();

  if( mw->ArgFileName.isEmpty() ) {
     // Fix me
     return;
  } else {
    vMPLoadCommandArgumentsLineedit->setText( mw->ArgFileName.ascii() );;
  }
}


void loadPanel::vMPLoadPageClearButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vMPLoadPageClearButtonSelected(), setting values in window to NULL \n");

#ifdef DEBUG_loadPanel
  printf("vMPLoadPageClearButtonSelected() called setting values in window to NULL\n");

  printf("loadPanel::vMPLoadPageClearButtonSelected(), vMPLoadParallelPrefixLineedit->text()=%s\n",
          vMPLoadParallelPrefixLineedit->text().ascii() );
  printf("loadPanel::vMPLoadPageClearButtonSelected(), vMPLoadPageLoadMPProgName.ascii()=%s\n",
          vMPLoadPageLoadMPProgName.ascii() );
  printf("loadPanel::vMPLoadPageClearButtonSelected(), vMPLoadCommandArgumentsLineedit->text()=%s\n",
          vMPLoadCommandArgumentsLineedit->text().ascii() );
#endif

  vMPLoadPageLoadMPProgName = QString::null;
  vMPLoadMPILoadLineedit->setText("");
  vMPLoadCommandArgumentsLineedit->setText("");;
  vMPLoadParallelPrefixLineedit->setText("");
  vMPLoadMPICommandLineedit->setText("");;

}

void
loadPanel::vUpdateAttachOrLoadPageWidget()
{
#ifdef DEBUG_loadPanel
  printf("loadPanel::vUpdateAttachOrLoadPageWidget() entered\n");
#endif

  vAttachOrLoadPageProcessListLabel->hide();
  vAttachOrLoadPageMultiProcessListLabel->hide();
  vAttachOrLoadPageExecutableLabel->hide();
  vAttachOrLoadPageMultiProcessExecutableLabel->hide();
  if( getPanelContainer()->getMainWindow() )
  {
    OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
    if( mw )
    {
#ifdef DEBUG_loadPanel
      printf("loadPanel::vUpdateAttachOrLoadPageWidget(), mw->executableName.isEmpty()=%d\n", mw->executableName.isEmpty() );
      printf("loadPanel::vUpdateAttachOrLoadPageWidget(), mw->pidStr.isEmpty()=%d\n", mw->pidStr.isEmpty() );
#endif
      if( !mw->executableName.isEmpty() )
      {
        vAttachOrLoadPageAttachToProcessCheckBox->setChecked(FALSE);
        if (getMPIWizardCalledMe()) {
          vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setChecked(TRUE);
          vAttachOrLoadPageLoadExecutableCheckBox->setChecked(FALSE);
        } else {
          vAttachOrLoadPageLoadExecutableCheckBox->setChecked(TRUE);
          vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setChecked(FALSE);
        }
        vAttachOrLoadPageExecutableLabel->setText( mw->executableName );
        vAttachOrLoadPageMultiProcessExecutableLabel->setText( mw->executableName );
        vAttachOrLoadPageLoadExecutableCheckBox->setText( tr( "Load the following executable from disk." ) );
        vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setText( tr( "Start/Run the following multi-process executable from disk (MPI)." ) );
        vAttachOrLoadPageExecutableLabel->show();
        vAttachOrLoadPageMultiProcessExecutableLabel->show();
        vAttachOrLoadPageLoadDifferentExecutableCheckBox->show();
        vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->show();
      } else if( !mw->pidStr.isEmpty() ) {
        if (getMPIWizardCalledMe()) {
          if (!getInstrumentorIsOffline()) {
            vAttachOrLoadPageAttachToMultiProcessCheckBox->setChecked(TRUE);
            vAttachOrLoadPageAttachToProcessCheckBox->setChecked(FALSE);
          }
        } else {
          if (!getInstrumentorIsOffline()) {
            vAttachOrLoadPageAttachToMultiProcessCheckBox->setChecked(FALSE);
            vAttachOrLoadPageAttachToProcessCheckBox->setChecked(TRUE);
          }
        } 
        
        vAttachOrLoadPageLoadExecutableCheckBox->setChecked(FALSE);
        vAttachOrLoadPageProcessListLabel->setText( mw->pidStr );
        vAttachOrLoadPageLoadExecutableCheckBox->setText( tr( "Load an executable from disk." ) );
        vAttachOrLoadPageProcessListLabel->show();
        vAttachOrLoadPageLoadDifferentExecutableCheckBox->hide();
      }
    }
    if( mw->executableName.isEmpty() )
    {
      vAttachOrLoadPageExecutableLabel->setText( "" );
      vAttachOrLoadPageLoadDifferentExecutableCheckBox->hide();
      vAttachOrLoadPageMultiProcessExecutableLabel->setText( "" );
      vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->hide();
    }
    if( mw->pidStr.isEmpty() )
    {
      vAttachOrLoadPageProcessListLabel->setText( mw->pidStr );
      vAttachOrLoadPageMultiProcessListLabel->setText( mw->pidStr );
    }
    // If both possible choices are empty then we should select the
    // load executable check box setting as the default.
    if (mw->executableName.isEmpty() && mw->pidStr.isEmpty() ) {
      if (getMPIWizardCalledMe()) {
          vAttachOrLoadPageLoadExecutableCheckBox->setChecked(FALSE);
          vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setChecked(TRUE);
      } else {
          vAttachOrLoadPageLoadExecutableCheckBox->setChecked(TRUE);
          vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->setChecked(FALSE);
      }
    }
  }
}

void loadPanel::vMPLoadPageProcessAccept()

{
  nprintf(DEBUG_PANELS) ("vMPLoadPageProcessAccept() \n");
#ifdef DEBUG_loadPanel
  printf("loadPanel::vMPLoadPageProcessAccept() \n");
#endif

  char buffer[2048];

  if (!getInstrumentorIsOffline()) {
     if( !vAttachOrLoadPageAttachToProcessCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadExecutableCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->isChecked() &&
         !vAttachOrLoadPageAttachToMultiProcessCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadDifferentExecutableCheckBox->isChecked() )
     {
       QString msg = QString("You must either select the option to attach to an \nexisting process or load an executable.  Please select one.\n");
       QMessageBox::information( (QWidget *)this, "Process or executable needed...",
                                  msg, QMessageBox::Ok );
    
       return;
     }
  } else {
     if( !vAttachOrLoadPageLoadExecutableCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->isChecked() &&
         !vAttachOrLoadPageLoadDifferentExecutableCheckBox->isChecked() )
     {
       QString msg = QString("You must either select one of the options to load an executable.  Please select one.\n");
       QMessageBox::information( (QWidget *)this, "Process or executable needed...",
                                  msg, QMessageBox::Ok );
    
       return;
     }
  }

#ifdef DEBUG_loadPanel
  printf("IN loadPanel::vMPLoadPageProcessAccept() --------------------------- \n");
  printf("vAttachOrLoadPageLoadExecutableCheckBox->isChecked()=%d \n",
         vAttachOrLoadPageLoadExecutableCheckBox->isChecked() );
  printf("vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->isChecked()=%d \n",
         vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->isChecked() );
  printf("vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->isChecked()=%d \n",
         vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->isChecked() );
  if (!getInstrumentorIsOffline()) {
    printf("vAttachOrLoadPageAttachToMultiProcessCheckBox->isChecked()=%d \n",
           vAttachOrLoadPageAttachToMultiProcessCheckBox->isChecked() );
    printf("vAttachOrLoadPageAttachToProcessCheckBox->isChecked()=%d \n",
           vAttachOrLoadPageAttachToProcessCheckBox->isChecked() );
  }
  printf("vAttachOrLoadPageLoadDifferentExecutableCheckBox->isChecked()=%d \n",
         vAttachOrLoadPageLoadDifferentExecutableCheckBox->isChecked() );
  printf("IN loadPanel::vMPLoadPageProcessAccept() --------------------------- \n");
#endif

  if (!getInstrumentorIsOffline() && 
      vAttachOrLoadPageAttachToProcessCheckBox->isChecked() && 
      vAttachOrLoadPageLoadExecutableCheckBox->isChecked() ) {

    QString msg = QString("From this wizard you can only select to either attach or load.  Please select only one.\n");
    QMessageBox::information( (QWidget *)this, "Process or executable needed...",
                               msg, QMessageBox::Ok );
    
    return;
  }

  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
  if( !mw ) {
    return;
  } 


  // New MultiProcess duplicate
  if( !getInstrumentorIsOffline() && vAttachOrLoadPageAttachToMultiProcessCheckBox->isChecked() ) { 

    if( mw->pidStr.isEmpty() ) {
// jeg this is double cancel issue
#if 0
      mw->attachNewMultiProcess();
#endif
    }

    if( mw->pidStr.isEmpty() ) {
      return;
    } else {
      // Parallel Call
#ifdef DEBUG_loadPanel
      printf("loadPanel::xxx1, WasDoingParallel==TRUE\n");
#endif
      // Even though we are doing parallel attach here, we
      // want to go back to the first load wizard page for the attach parallel
      // option.  We call with a FALSE argument to accomplish that.
      vSummaryPageFinishButtonSelected(FALSE /* WasDoingParallel */ );
    }

  }

  // Existing sequential attach
  if( !getInstrumentorIsOffline() && vAttachOrLoadPageAttachToProcessCheckBox->isChecked() ) { 

    if( mw->pidStr.isEmpty() ) {
// jeg this is double cancel issue
#if 0
      mw->attachNewProcess();
#endif
    }

    if( mw->pidStr.isEmpty() ) {
      return;
    } else {
      // Sequential Call
#ifdef DEBUG_loadPanel
      printf("loadPanel::xxx1, WasDoingParallel==FALSE\n");
#endif
      vSummaryPageFinishButtonSelected(FALSE /* WasDoingSequentialLA */);
    }

#if 0

    QString host_name = mw->pidStr.section(' ', 0, 0, QString::SectionSkipEmpty);
    QString pid_name = mw->pidStr.section(' ', 1, 1, QString::SectionSkipEmpty);
    QString prog_name = mw->pidStr.section(' ', 2, 2, QString::SectionSkipEmpty);

    sprintf(buffer, "<p align=\"left\">You've selected a pc Sample experiment for process \"%s\" running on host \"%s\".  Furthermore, you've chosen a sampling rate of \"%s\".<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"loadPanel\" panel will be raised to allow you to futher control the experiment, or click the \"RUN\" button to run the experiment as created by the wizard process.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->pidStr.ascii(), mw->hostStr.ascii(), /* vParameterPageSampleRateText->text().ascii() */ "dummyRate");
#endif

  }

  // New MultiProcess load duplicate of existing
  if( vAttachOrLoadPageLoadMultiProcessExecutableCheckBox->isChecked() ||
      vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->isChecked() ) {

    if( mw->executableName.isEmpty() ||
        vAttachOrLoadPageLoadDifferentMultiProcessExecutableCheckBox->isChecked() ) {

      nprintf(DEBUG_PANELS) ("Load the QFile \n");
      mainWidgetStack->raiseWidget(vMPStackPage1);
//      mw->loadNewMultiProcessProgram();

    }
    // Parallel Call
#ifdef DEBUG_loadPanel
    printf("loadPanel::xxx1, WasDoingParallel==TRUE\n");
#endif
    vSummaryPageFinishButtonSelected(TRUE /* WasDoingParallelLA */);

// Won't get here anymore because this handled by vMPStackPage1 logic
#if 0
    if( mw->executableName.isEmpty() ) {
      return;
    }

    QString host_name = mw->pidStr.section(' ', 0, 0, QString::SectionSkipEmpty);
    QString pid_name = mw->pidStr.section(' ', 1, 1, QString::SectionSkipEmpty);
    QString prog_name = mw->pidStr.section(' ', 2, 2, QString::SectionSkipEmpty);

    if( mw->parallelPrefixCommandStr.isEmpty() || mw->parallelPrefixCommandStr.isNull() ) {
        sprintf(buffer, "<p align=\"left\">You've selected a pc Sample experiment for command/executable \"%s\" to be run on host \"%s\".  Furthermore, you've chosen a sampling rate of \"%s\".<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"loadPanel\" panel will be raised to allow you to futher control the experiment, or click the \"RUN\" button to run the experiment as created by the wizard process.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->executableName.ascii(), mw->hostStr.ascii(), /* vParameterPageSampleRateText->text().ascii() */ "dummyRate" );
    } else {
        sprintf(buffer, "<p align=\"left\">You've selected a pc Sample experiment for command/executable <br>\"%s %s\" to be run on host<br>\"%s\".  Furthermore, you've chosen a sampling rate of \"%s\".<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"loadPanel\" panel will be raised to allow you to futher control the experiment, or click the \"RUN\" button to run the experiment as created by the wizard process.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->parallelPrefixCommandStr.ascii(), mw->executableName.ascii(), mw->hostStr.ascii(), /* vParameterPageSampleRateText->text().ascii()  */ "dummyRate");
    }
#endif

  }

  // Existing sequential load executable code
  if( vAttachOrLoadPageLoadExecutableCheckBox->isChecked() ||
      vAttachOrLoadPageLoadDifferentExecutableCheckBox->isChecked() ) {

    if( mw->executableName.isEmpty() ||
        vAttachOrLoadPageLoadDifferentExecutableCheckBox->isChecked() ) {
      nprintf(DEBUG_PANELS) ("Load the QFile \n");
#ifdef DEBUG_loadPanel
      printf("loadPanel::vMPLoadPageProcessAccept() calling loadNewProgram at Load the QFile area\n");
#endif
      mw->loadNewProgram();
    }

    if( mw->executableName.isEmpty() ) {
      return;
    } else {
      // Sequential Call
#ifdef DEBUG_loadPanel
     printf("loadPanel::xxx4, WasDoingParallel==FALSE\n");
#endif
      vSummaryPageFinishButtonSelected(FALSE /* WasDoingSequentialLA */);
    }
  }

#ifdef DEBUG_loadPanel
  printf("loadPanel::vMPLoadPageProcessAccept() EXITED \n");
#endif

}

void loadPanel::vMPLoadPageFinishButtonSelected()
{
#ifdef DEBUG_loadPanel
  printf("loadPanel::vMPLoadPageFinishButtonSelected() \n");
#endif
  // Check to see if any data was entered by the user.  If not or if
  // incorrect go back to the vMPLoadPage and ask for the input
  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
  if(mw && 
     vMPLoadParallelPrefixLineedit->text().isEmpty() ) {

#ifdef DEBUG_loadPanel
     printf("loadPanel::vMPLoadPageFinishButtonSelected(), need input for the parallel prefix and program to be loaded\n");
     printf("loadPanel::vMPLoadPageFinishButtonSelected(), reraise the load parallel program page\n");
#endif 

    QString msg = QString("You must enter a parallel prefix that Open|SpeedShop will use to launch your MPI job.\nExamples are: \"orterun -np 2\" or \"srun -N 4 -n 8\".\n");
    QMessageBox::information( (QWidget *)this, "Parallel command prefix needed...",
                               msg, QMessageBox::Ok );
    mainWidgetStack->raiseWidget(vMPStackPage1);
    
  }  
  if(mw && 
     vMPLoadMPILoadLineedit->text().isEmpty() ) {

    QString msg = QString("You must select a parallel executable that Open|SpeedShop will launch as part of your MPI job.\nUse the \"Browse\" button found in \"Step 2\" to select your MPI executable.\n");
    QMessageBox::information( (QWidget *)this, "Parallel executable needed...",
                               msg, QMessageBox::Ok );
  }  

  if(mw && !vMPLoadParallelPrefixLineedit->text().isEmpty() ) {
     mw->parallelPrefixCommandStr = vMPLoadParallelPrefixLineedit->text().ascii();
  }

  if(mw && !vMPLoadMPILoadLineedit->text().isEmpty() ) {
     mw->executableName = vMPLoadMPILoadLineedit->text().ascii();
  }

  if(mw && !vMPLoadCommandArgumentsLineedit->text().isEmpty() ) {
     mw->argsStr =  vMPLoadCommandArgumentsLineedit->text().ascii();
  }

#ifdef DEBUG_loadPanel
    if (! vMPLoadCommandArgumentsLineedit->text().isEmpty() ) {
      printf("loadPanel::vMPLoadPageFinishButtonSelected(), vMPLoadCommandArgumentsLineedit->text()=%s\n",
            vMPLoadCommandArgumentsLineedit->text().ascii() );
    }
    printf("loadPanel::vMPLoadPageFinishButtonSelected(),else clause call vSummaryPageFinishButtonSelected()\n" );
#endif
    // Parallel Call
    vSummaryPageFinishButtonSelected(TRUE /* WasDoingParallelLA */);


}

void loadPanel::finishButtonSelected()
{
  nprintf(DEBUG_PANELS) ("loadPanel::finishButtonSelected() \n");
#ifdef DEBUG_loadPanel
  printf("loadPanel::finishButtonSelected() \n");
#endif

#if 0
  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
  if( mw->executableName.isEmpty() && mw->pidStr.isEmpty() )
  {
      mainWidgetStack->raiseWidget(vALStackPage0);
#ifdef DEBUG_loadPanel
      printf("loadPanel::finishButtonSelected(),after raising mainWidgetStack->raiseWidget(vALStackPage0);\n" );
#endif
      vUpdateAttachOrLoadPageWidget();
#ifdef DEBUG_loadPanel
      printf("loadPanel::finishButtonSelected(),after vUpdateAttachOrLoadPageWidget()\n" );
#endif
      vAttachOrLoadPageNextButtonSelected();
#ifdef DEBUG_loadPanel
      printf("loadPanel::finishButtonSelected(),after vAttachOrLoadPageNextButtonSelected(), calling vSummaryPageFinishButtonSelected\n" );
#endif
      vSummaryPageFinishButtonSelected();
#ifdef DEBUG_loadPanel
      printf("loadPanel::finishButtonSelected(),after vSummaryPageFinishButtonSelected()\n" );
#endif
  } else {

    if(mw && !vMPLoadParallelPrefixLineedit->text().isEmpty() ) {
      mw->parallelPrefixCommandStr = vMPLoadParallelPrefixLineedit->text().ascii();
    }

    if(mw && !vMPLoadMPILoadLineedit->text().isEmpty() ) {
      mw->executableName = vMPLoadMPILoadLineedit->text().ascii();
    }
    if(mw && !vMPLoadCommandArgumentsLineedit->text().isEmpty() ) {
      mw->argsStr =  vMPLoadCommandArgumentsLineedit->text().ascii();
    }
#ifdef DEBUG_loadPanel
    if ( !vMPLoadCommandArgumentsLineedit->text().isEmpty() ) {
      printf("loadPanel::finishButtonSelected(),else clause,vMPLoadCommandArgumentsLineedit->text()=%s\n",
          vMPLoadCommandArgumentsLineedit->text().ascii() );
    }
    printf("loadPanel::finishButtonSelected(),else clause call vSummaryPageFinishButtonSelected()\n" );
#endif
    // Parallel Call
    vSummaryPageFinishButtonSelected(TRUE /* WasDoingParallelLA */);
  }
#endif
}

void loadPanel::vSummaryPageFinishButtonSelected(bool wasIdoingParallel)
{
  nprintf(DEBUG_PANELS) ("vSummaryPageFinishButtonSelected() \n");
#ifdef DEBUG_loadPanel
  printf("loadPanel::vSummaryPageFinishButtonSelected(), wasIdoingParallel=%d\n", wasIdoingParallel);
#endif

  // Indicate to the loadPanel that the last load or attach
  // was parallel or sequential.  This is used to raise the
  // correct page of the loadPanel when the Raise_Second_Load_Panel
  // message is received in the listener

  setWasDoingParallel(wasIdoingParallel);
  Panel *p = targetPanel;

#ifdef DEBUG_loadPanel
  printf("loadPanel::vSummaryPageFinishButtonSelected(), targetPanel is read for sending summary page raise msg: p->getName()=%s\n", p->getName() );
#endif

  if( getPanelContainer()->getMainWindow() ) {

    OpenSpeedshop *mw = getPanelContainer()->getMainWindow();

    if( mw ) {

      LoadAttachObject *lao = NULL;
      ParamList *paramList = new ParamList();

      if( !mw->executableName.isEmpty() ) {

          // We should ask the user if he wants to change the
          // previously selected executable because he is passing
          // through this code again.
          if (getHaveWeLoadedAnExecutableBefore() && mw->pidStr.isEmpty() ) {

#ifdef DEBUG_loadPanel
            printf("loadPanel::vSummaryPageFinishButtonSelected(), YOU WERE HERE BEFORE target panel=%s\n", p->getName() );
#endif

              if (getWasDoingParallel()) {
                mainWidgetStack->raiseWidget(vMPStackPage1);
              } else {
                clearPreviousSettings(mw); /* Since we are coming through a second time we could have
                                      set some multiprocessing items.  Clear them before going on. */
                mw->loadNewProgram();
              }
              if( mw->executableName.isEmpty() ) {
#ifdef DEBUG_loadPanel
                printf("loadPanel::vSummaryPageFinishButtonSelected(), NOW THERE IS NO EXECUTABLE\n");
                // FIX ME
#endif
                lao = NULL; // Clear away the object if no executable name is present
              }
            
          }

#ifdef DEBUG_loadPanel
        if ( !mw->executableName.isEmpty() ) {
          printf("loadPanel::vSummaryPageFinishButtonSelected(), executable name was specified as: %s\n", mw->executableName.ascii());
        }
        if ( !mw->parallelPrefixCommandStr.isEmpty() ) {
          printf("loadPanel::vSummaryPageFinishButtonSelected(), parallelPrefix was specified as: %s\n", mw->parallelPrefixCommandStr.ascii());
        }
#endif

        // Do this check again because the user may have canceled out of
        // the loadNewProgram dialog in the haveWeLoadedAnExecutableBefore check above
        if( !mw->executableName.isEmpty() ) {
           lao = new LoadAttachObject(mw->executableName, (char *)NULL, mw->parallelPrefixCommandStr, paramList, TRUE);
           setHaveWeLoadedAnExecutableBefore(TRUE);
        }

      } else if( !mw->pidStr.isEmpty() ) {

#ifdef DEBUG_loadPanel
          printf("loadPanel::vSummaryPageFinishButtonSelected(), pid was specified as: %s, getHaveWeAttachedToPidBefore()=%d\n", 
                 mw->pidStr.ascii(), getHaveWeAttachedToPidBefore() );
#endif

          // We should ask the user if he wants to change the
          // previously selected attached pid because he is passing through this code again.
          //
          if (getHaveWeAttachedToPidBefore() && mw->executableName.isEmpty() ) {

#ifdef DEBUG_loadPanel
          printf("loadPanel::vSummaryPageFinishButtonSelected(), ATTACH, YOU WERE HERE BEFORE target panel=%s\n", p->getName() );
#endif

            clearPreviousSettings(mw); /* Since we are coming through a second time we could have
                                    set some multiprocessing items.  Clear them before going on. */
            mw->attachNewProcess();
//            mainWidgetStack->raiseWidget(vALStackPage0);

            if( mw->pidStr.isEmpty() ) {

#ifdef DEBUG_loadPanel
              printf("loadPanel::vSummaryPageFinishButtonSelected(), NOW THERE IS NO ATTACHED PID\n");
              // FIX ME
#endif

              lao = NULL; // Clear away the object if no pid name is present
            }
            
        }

        // Do this check again because the user may have canceled out of
        // the attach dialog in the haveWeAttachedAnPidBefore check above
        // Plus we've delayed the creation of the LAO until here because people
        // may have changed their selection by going back and reselecting.
        // This code would have been executed the first time also but the code
        // has been rearranged to support changing when the user goes back.

        if( !mw->pidStr.isEmpty() ) {
           lao = new LoadAttachObject((char *)NULL, mw->pidStr, (char *)NULL, paramList, TRUE);
           setHaveWeAttachedToPidBefore(TRUE);
        }

      } else {

#ifdef DEBUG_loadPanel
        printf("loadPanel::vSummaryPageFinishButtonSelected(), Warning: No attach or load paramaters available.\n");
#endif

      }

      if( lao != NULL ) {
//        vSummaryPageFinishButton->setEnabled(FALSE);
//        vSummaryPageBackButton->setEnabled(FALSE);
        qApp->flushX();

        if( !p ) {

#ifdef DEBUG_loadPanel
          printf("loadPanel::vSummaryPageFinishButtonSelected p is empty need to create, targetPanel->getName()=%s\n", targetPanel->getName() );
#endif
          ArgumentObject *ao = new ArgumentObject("ArgumentObject", -1 );
          ao->lao = lao;
          p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel(targetPanel->getName(), getPanelContainer(), ao);
          delete ao;

        } else {

#ifdef DEBUG_loadPanel
          printf("loadPanel::vSummaryPageFinishButtonSelected calling p->listener with lao\n");
          printf("loadPanel::vSummaryPageFinishButtonSelected calling this=0x%x,  p->listener with Wizard_Raise_Summary_Page msg\n", this);
#endif

          p->getPanelContainer()->raisePanel(p);
          p->listener((void *)lao);
          MessageObject *msg = new MessageObject("Wizard_Raise_Summary_Page");

#ifdef DEBUG_loadPanel
          printf("loadPanel::vSummaryPageFinishButtonSelected(), sending Wizard_Raise_Summary_Page, targetPanels name is: p->getName()=%s\n", p->getName() );
#endif

          p->listener((void *)msg);
          delete msg;
       }

// The receiving routine should delete this...
// delete paramList;
      } // (lao != NULL)
    } // (mw)
  } // (getPanelContainer)

}



/*! This calls the user 'menu()' function
    if the user provides one.   The user can attach any specific panel
    menus to the passed argument and they will be displayed on a right
    mouse down in the panel.
    /param  contextMenu is the QPopupMenu * that use menus can be attached.
 */
bool
loadPanel::menu(QPopupMenu* contextMenu)
{
  return( FALSE );
}

/*! If the user panel save functionality, their function
     should provide the saving.
 */
void 
loadPanel::save()
{
}

/*! If the user panel provides save to functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
loadPanel::saveAs()
{
}

/*! This function listens for messages.
    When a message has been sent (from anyone) and the message broker is
    notifying panels that they may want to know about the message, this is the
    function the broker notifies.   The listener then needs to determine
    if it wants to handle the message.
    \param msg is the incoming message.
    \return 0 means you didn't do anything with the message.
    \return 1 means you handled the message.
 */
int
loadPanel::listener(void *msg)
{
  nprintf(DEBUG_PANELS) ("loadPanel::listener() requested.\n");
  MessageObject *messageObject = (MessageObject *)msg;

#ifdef DEBUG_loadPanel
  if ( !messageObject->msgType.isEmpty() ) {
    printf("loadPanel::listener() messageObject->msgType = %s\n", 
            messageObject->msgType.ascii() );
  }
#endif
  if( messageObject->msgType == getName() )
  {
//    vSummaryPageFinishButton->setEnabled(TRUE);
//    eSummaryPageFinishButton->setEnabled(TRUE);
//    vSummaryPageBackButton->setEnabled(TRUE);
//    eSummaryPageBackButton->setEnabled(TRUE);
    vMPLoadPageFinishButton->setEnabled(TRUE);
    qApp->flushX();
    return 1;
  }
  if( messageObject->msgType == "Wizard_Raise_First_Page" )
  {
//    vSummaryPageFinishButton->setEnabled(TRUE);
//    eSummaryPageFinishButton->setEnabled(TRUE);
//    vSummaryPageBackButton->setEnabled(TRUE);
//    eSummaryPageBackButton->setEnabled(TRUE);
    qApp->flushX();
    {
      mainWidgetStack->raiseWidget(vALStackPage0);
    } 
    return 1;
  }
  if( messageObject->msgType == "Wizard_Raise_Second_Page" )
  {
//    vSummaryPageFinishButton->setEnabled(TRUE);
//    eSummaryPageFinishButton->setEnabled(TRUE);
//    vSummaryPageBackButton->setEnabled(TRUE);
//    eSummaryPageBackButton->setEnabled(TRUE);
    qApp->flushX();
    if (getWasDoingParallel()) {
#ifdef DEBUG_loadPanel
      printf("listener, getWasDoingParallel()==TRUE\n");
#endif
      mainWidgetStack->raiseWidget(vMPStackPage1);
    } else {
#ifdef DEBUG_loadPanel
      printf("listener, getWasDoingParallel()==FALSE\n");
#endif
      mainWidgetStack->raiseWidget(vALStackPage0);
    } 
    return 1;
  }

  if( messageObject->msgType == "PreferencesChangedObject" ) {

#ifdef DEBUG_loadPanel
   printf("loadPanel::listener, PREFERENCE-CHANGED-OBJECT\n");
#endif // DEBUG_StatsPanel

#ifdef NEEDSTHOUGHT
    QSettings *settings = new QSettings();
    bool temp_instrumentorIsOffline = settings->readBoolEntry( "/openspeedshop/general/instrumentorIsOffline");
    setInstrumentorIsOffline(temp_instrumentorIsOffline);
#ifdef DEBUG_StatsPanel
    printf("loadPanel: listener, PREFERENCE-CHANGED-OBJECT, /openspeedshop/general/instrumentorIsOffline == instrumentorIsOffline=(%d)\n", temp_instrumentorIsOffline );
#endif
    delete settings;
#endif

  }

  if( messageObject->msgType == "Wizard_Hide_First_Page" ) {
    qApp->flushX();
  }

  if( messageObject->msgType == "Wizard_Hide_Second_Page" ) {
    qApp->flushX();
  }

  return 0;  // 0 means, did not want this message and did not act on anything.
}

/*! If the user wants to override the Panel::broadcast() functionality, put
    that logic here.  Otherwise, based on the broadcast type, send out the
    message.
 */
int 
loadPanel::broadcast(char *msg)
{
  return 0;
}

