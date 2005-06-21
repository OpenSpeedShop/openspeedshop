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


#ifndef PREFERENCE_PLUGIN_INFO_H
#define PREFERENCE_PLUGIN_INFO_H

#include <qwidgetstack.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qsettings.h>

extern "C"
{
  QWidget* sourcePanelStackPage;
  QGroupBox* sourcePanelGroupBox;
  QCheckBox* showStatisticsCheckBox;
  QCheckBox* showLineNumbersCheckBox;
  QVBoxLayout* generalStackPageLayout_2;
  QVBoxLayout* layout6;

  static char *pname = NULL;

  bool getShowStatistics()
  {
// printf("getShowStatistics(%s)=%d\n", pname, showStatisticsCheckBox->isChecked() );
    return( showStatisticsCheckBox->isChecked() );
  }

  bool getShowLineNumbers()
  {
// printf("getShowLineNumbers(%s)=%d\n", pname, showLineNumbersCheckBox->isChecked() );
    return( showLineNumbersCheckBox->isChecked() );
  }

// #include <assert.h>
static int i = 1;
  void initSourcePanelPreferenceSettings()
  {
// printf("initSourcePanelPreferenceSettings(%s)\n", pname);
// assert(i);
    showStatisticsCheckBox->setChecked(FALSE);
    showLineNumbersCheckBox->setChecked(FALSE);
i--;
  }

  QWidget *initialize_preferences_entry_point(QSettings *settings, QWidgetStack *stack, char *name)
  {
// printf("initialize_preferences_entry_point(0x%x 0x%x %s) entered\n", settings, stack, name);

    sourcePanelStackPage = new QWidget( stack, name );
    pname = strdup(name);
    generalStackPageLayout_2 = new QVBoxLayout( sourcePanelStackPage, 11, 6, "generalStackPageLayout_2"); 

    sourcePanelGroupBox = new QGroupBox( sourcePanelStackPage, "sourcePanelGroupBox" );

    QWidget* sourcePanelPrivateLayout = new QWidget( sourcePanelGroupBox, "layout6" );
//    sourcePanelPrivateLayout->setGeometry( QRect( 10, 40, 150, 52 ) );
    sourcePanelPrivateLayout->setGeometry( QRect( 10, 40, 200, 100 ) );
    layout6 = new QVBoxLayout( sourcePanelPrivateLayout, 11, 6, "layout6"); 

    showStatisticsCheckBox =
      new QCheckBox( sourcePanelPrivateLayout, "showStatisticsCheckBox" );
    showStatisticsCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showStatisticsCheckBox->sizePolicy().hasHeightForWidth() ) );
    layout6->addWidget( showStatisticsCheckBox );

    showLineNumbersCheckBox = new QCheckBox( sourcePanelPrivateLayout, "showLineNumbersCheckBox" );
    showLineNumbersCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showLineNumbersCheckBox->sizePolicy().hasHeightForWidth() ) );
    layout6->addWidget( showLineNumbersCheckBox );
    generalStackPageLayout_2->addWidget( sourcePanelGroupBox );
    stack->addWidget( sourcePanelStackPage, 1 );

// languageChange();
/*
    sourcePanelGroupBox->setTitle( tr( "Source Panel" ) );
    showStatisticsCheckBox->setText( tr( "Show statistics" ) );
    showLineNumbersCheckBox->setText( tr( "Show line numbers" ) );
*/
    sourcePanelGroupBox->setTitle( "Source Panel" );
    showStatisticsCheckBox->setText( "Show statistics" );
    showLineNumbersCheckBox->setText( "Show line numbers" );

    initSourcePanelPreferenceSettings();

    if( settings != NULL )
    {
// printf("Now set the initial settings from the preferences file.\n");
      char settings_buffer[1024];
      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showStatisticsCheckBox->name() );
      showStatisticsCheckBox->setChecked(
        settings->readBoolEntry(settings_buffer) );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showLineNumbersCheckBox->name() );
      showLineNumbersCheckBox->setChecked(
        settings->readBoolEntry(settings_buffer) );
    }

    return( sourcePanelStackPage );
  }
  void save_preferences_entry_point(QSettings *settings, char *name)
  {
// printf("save_preferences_entry_point(0x%x %s) entered\n", settings, name);

    char settings_buffer[1024];

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showStatisticsCheckBox->name() );
// printf("write (%s)\n", settings_buffer );
    if( ! settings->writeEntry(settings_buffer, showStatisticsCheckBox->isChecked() ) )
    {
      printf("error writting %s\n", settings_buffer);
    }

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showLineNumbersCheckBox->name() );
// printf("write (%s)\n", settings_buffer );
    if( !settings->writeEntry(settings_buffer, showLineNumbersCheckBox->isChecked()) )
    {
      printf("error writting %s\n", settings_buffer);
    }
  }
}

#endif // PREFERENCE_PLUGIN_INFO_H
