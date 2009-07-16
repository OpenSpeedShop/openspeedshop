////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2009 Krell Institute. All Rights Reserved.
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

//#define DEBUG_SRC_PREF 1

#ifndef PREFERENCE_PLUGIN_INFO_H
#define PREFERENCE_PLUGIN_INFO_H

#include <qwidgetstack.h>
#include <qlayout.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qsettings.h>

typedef QValueList<QLineEdit *> LineEditList;
static int numEntries = 10;
extern "C"
{
  QWidget* sourcePanelStackPage;
  QGroupBox* sourcePanelGroupBox;
  QCheckBox* showStatisticsCheckBox;
  QCheckBox* showLineNumbersCheckBox;
  QVBoxLayout* generalStackPageLayout_2;
  QVBoxLayout* layout6;

  QHBoxLayout *headerLayout;

  QLabel *leftSideHeaderLabel;
  QLabel *rightSideHeaderLabel;



  QHBoxLayout *gridLayout;
  QGridLayout *glayout;
  LineEditList leftSideLineEditList;
  LineEditList rightSideLineEditList;

  static char *pname = NULL;

  bool getShowStatistics()
  {
#ifdef DEBUG_SRC_PREF
    printf("getShowStatistics(%s)=%d\n", pname, showStatisticsCheckBox->isChecked() );
#endif
    return( showStatisticsCheckBox->isChecked() );
  }

  bool getShowLineNumbers()
  {
#ifdef DEBUG_SRC_PREF
    printf("getShowLineNumbers(%s)=%d\n", pname, showLineNumbersCheckBox->isChecked() );
#endif
    return( showLineNumbersCheckBox->isChecked() );
  }

  LineEditList *getLeftSideLineEditList()
  {
    return( &leftSideLineEditList );
  }

  LineEditList *getRightSideLineEditList()
  {
    return( &rightSideLineEditList );
  }

// #include <assert.h>
  void initPreferenceSettings()
  {
#ifdef DEBUG_SRC_PREF
    printf("SourcePanel: initPreferenceSettings(%s)\n", pname);
#endif
// assert(i);
    showStatisticsCheckBox->setChecked(FALSE);
    showLineNumbersCheckBox->setChecked(TRUE);

    for(int i=0;i<numEntries;i++)
    {
      QString ls_name = QString("leftSideLineEdit%1").arg(i);
      QString rs_name = QString("rightSideLineEdit%1").arg(i);

      // Now try to find this named widget and set the value
      for( LineEditList::Iterator it = leftSideLineEditList.begin();
           it != leftSideLineEditList.end(); ++it )
      {
      //   QLineEdit *lsle = (QLineEdit *)it.current(); 
         QLineEdit *lsle = (QLineEdit *)*it; 

         if( lsle->name() == ls_name )
         {
           lsle->setText("");
           break;
         }

      }
      for( LineEditList::Iterator it = rightSideLineEditList.begin();
           it != rightSideLineEditList.end(); ++it )
      {
      //   QLineEdit *lsle = (QLineEdit *)it.current(); 
         QLineEdit *rsle = (QLineEdit *)*it; 

         if( rsle->name() == rs_name )
         {
           rsle->setText("");
           break;
         }
      }
    }
  }

  QWidget *initialize_preferences_entry_point(QSettings *settings, QWidgetStack *stack, char *name)
  {
#ifdef DEBUG_SRC_PREF
    printf("initialize_preferences_entry_point(0x%x 0x%x %s) entered\n", settings, stack, name);
#endif

    sourcePanelStackPage = new QWidget( stack, name );
    pname = strdup(name);
    generalStackPageLayout_2 = new QVBoxLayout( sourcePanelStackPage, 11, 6, "generalStackPageLayout_2"); 

    sourcePanelGroupBox = new QGroupBox( sourcePanelStackPage, "sourcePanelGroupBox" );
    sourcePanelGroupBox->setColumnLayout(0, Qt::Vertical );
    sourcePanelGroupBox->layout()->setSpacing( 6 );
    sourcePanelGroupBox->layout()->setMargin( 11 );


    layout6 = new QVBoxLayout( sourcePanelGroupBox->layout(), 11, "layout6"); 

    showStatisticsCheckBox =
      new QCheckBox( sourcePanelGroupBox, "showStatisticsCheckBox" );
    showStatisticsCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showStatisticsCheckBox->sizePolicy().hasHeightForWidth() ) );
    layout6->addWidget( showStatisticsCheckBox );

    showLineNumbersCheckBox = new QCheckBox( sourcePanelGroupBox, "showLineNumbersCheckBox" );
    showLineNumbersCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showLineNumbersCheckBox->sizePolicy().hasHeightForWidth() ) );
    layout6->addWidget( showLineNumbersCheckBox );
    generalStackPageLayout_2->addWidget( sourcePanelGroupBox );
    stack->addWidget( sourcePanelStackPage, 1 );

// languageChange();
    sourcePanelGroupBox->setTitle( "Source Panel" );
    showStatisticsCheckBox->setText( "Show statistics" );
    showLineNumbersCheckBox->setText( "Show line numbers" );

    //Begin PathRempping Section
    QFrame *remapFrame = new QFrame( sourcePanelGroupBox, "remapFrameHeader" );
    remapFrame->setFrameShape( QFrame::Box );
    remapFrame->setFrameShadow( QFrame::Plain );
    QVBoxLayout *remapLayout = new QVBoxLayout( remapFrame, 11, 6, "remapFrameLayout" );
    layout6->addWidget(remapFrame);
    headerLayout = new QHBoxLayout( remapLayout );

    leftSideHeaderLabel = new QLabel("Old Path Name", remapFrame, "leftSideHeaderLabel");
    leftSideHeaderLabel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed, 0, 0) );
    rightSideHeaderLabel = new QLabel("New Path Name", remapFrame, "rightSideHeaderLabel");
  
    headerLayout->addWidget(leftSideHeaderLabel);
    headerLayout->addWidget(rightSideHeaderLabel);

    gridLayout = new QHBoxLayout( remapLayout );


    glayout = new QGridLayout(gridLayout,10,2);


    for(int i=0;i<numEntries;i++)
    {
      QLineEdit *lsle = new QLineEdit(remapFrame,QString("leftSideLineEdit%1").arg(i));
      leftSideLineEditList.push_back(lsle);
  
      QLineEdit *rsle = new QLineEdit(remapFrame,QString("rightSideLineEdit%1").arg(i));
      rightSideLineEditList.push_back(rsle);
  
      glayout->addWidget(lsle, i, 0);
      glayout->addWidget(rsle, i, 1);
    }
    //End PathRempping Section

    initPreferenceSettings();

    if( settings != NULL )
    {
#ifdef DEBUG_SRC_PREF
      printf("Now set the initial settings from the preferences file.\n");
#endif
      char settings_buffer[1024];
      sprintf(settings_buffer, "/%s/%s/%s", "openspeedshop", name, showStatisticsCheckBox->name() );
      showStatisticsCheckBox->setChecked( settings->readBoolEntry(settings_buffer, FALSE) );

      sprintf(settings_buffer, "/%s/%s/%s", "openspeedshop", name, showLineNumbersCheckBox->name() );
      showLineNumbersCheckBox->setChecked( settings->readBoolEntry(settings_buffer, TRUE) );

#ifdef DEBUG_SRC_PREF
      printf("SRC pref, settings->readBoolEntry(settings_buffer)=%d\n", settings->readBoolEntry(settings_buffer));
#endif

      for(int i=0;i<numEntries;i++)
      {
        QString ls_name = QString("leftSideLineEdit%1").arg(i);
        QString rs_name = QString("rightSideLineEdit%1").arg(i);

        sprintf(settings_buffer, "/%s/%s/%s",
                "openspeedshop", name, ls_name.ascii() );
        QString lsentryStr = settings->readEntry(settings_buffer);
        sprintf(settings_buffer, "/%s/%s/%s",
                "openspeedshop", name, rs_name.ascii() );
        QString rsentryStr = settings->readEntry(settings_buffer);


        // Now try to find this named widget and set the value
        for( LineEditList::Iterator it = leftSideLineEditList.begin();
             it != leftSideLineEditList.end(); ++it )
        {
        //   QLineEdit *lsle = (QLineEdit *)it.current(); 
           QLineEdit *lsle = (QLineEdit *)*it; 

           if( lsle->name() == ls_name )
           {
             lsle->setText(lsentryStr);
             break;
           }

        }
        for( LineEditList::Iterator it = rightSideLineEditList.begin();
             it != rightSideLineEditList.end(); ++it )
        {
        //   QLineEdit *lsle = (QLineEdit *)it.current(); 
           QLineEdit *rsle = (QLineEdit *)*it; 

           if( rsle->name() == rs_name )
           {
             rsle->setText(rsentryStr);
             break;
           }
        }
      }
    }

    return( sourcePanelStackPage );
  }
  void save_preferences_entry_point(QSettings *settings, char *name)
  {
#ifdef DEBUG_SRC_PREF
    printf("SRC pref, save_preferences_entry_point(0x%x %s) entered\n", settings, name);
#endif

    char settings_buffer[1024];

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showStatisticsCheckBox->name() );

#ifdef DEBUG_SRC_PREF
      printf("SRC pref, for showStatisticsCheckBox, write (%s)\n", settings_buffer );
#endif

    if( ! settings->writeEntry(settings_buffer, showStatisticsCheckBox->isChecked() ) )
    {
      printf("error writting %s\n", settings_buffer);
    }

    sprintf(settings_buffer, "/%s/%s/%s", "openspeedshop", name, showLineNumbersCheckBox->name() );

#ifdef DEBUG_SRC_PREF
      printf("SRC pref, for showLineNumbers, write (%s)\n", settings_buffer );
#endif

    if( !settings->writeEntry(settings_buffer, showLineNumbersCheckBox->isChecked()) )
    {
      printf("error writting %s\n", settings_buffer);
    }

    for(int i=0;i<numEntries;i++)
    {
      QString ls_name = QString("leftSideLineEdit%1").arg(i);
      QString rs_name = QString("rightSideLineEdit%1").arg(i);

      // Now try to find this named widget and set the value
      for( LineEditList::Iterator it = leftSideLineEditList.begin();
           it != leftSideLineEditList.end(); ++it )
      {
      //   QLineEdit *lsle = (QLineEdit *)it.current(); 
         QLineEdit *lsle = (QLineEdit *)*it; 
      
         if( lsle->name() == ls_name )
         {
           QString lsText = lsle->text();
           sprintf(settings_buffer, "/%s/%s/%s",
               "openspeedshop", name, ls_name.ascii() );
           settings->writeEntry(settings_buffer, lsText.ascii() );
           break;
         }
      }
      for( LineEditList::Iterator it = rightSideLineEditList.begin();
           it != rightSideLineEditList.end(); ++it )
      {
      //   QLineEdit *rsle = (QLineEdit *)it.current(); 
         QLineEdit *rsle = (QLineEdit *)*it; 
      
         if( rsle->name() == rs_name )
         {
           QString rsText = rsle->text();
           sprintf(settings_buffer, "/%s/%s/%s",
               "openspeedshop", name, rs_name.ascii() );
           settings->writeEntry(settings_buffer, rsText.ascii() );
           break;
         }
      }
    }
  }
}

#endif // PREFERENCE_PLUGIN_INFO_H
