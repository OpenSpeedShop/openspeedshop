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
  QWidget* statsPanelStackPage;
  QVBoxLayout* generalStackPageLayout_3;
  QGroupBox* statsPanelGroupBox;
  QVBoxLayout* layout8;

  QCheckBox* sortDecendingCheckBox;

  QHBoxLayout* layoutTopN;
  QLabel* showTopNTextLabel;
  QLineEdit* showTopNLineEdit;

  QHBoxLayout* layoutColumnToSort;
  QLabel* showColumnToSortTextLabel;
  QLineEdit* showColumnToSortLineEdit;

  static char *pname = NULL;

  bool getPreferenceSortDecending()
  {
printf("getPreferenceSortDecending(%s)\n", pname);
    return( sortDecendingCheckBox->isChecked() );
  }

  QString getPreferenceColumnToSortLineEdit()
  {
printf("getPreferenceColumnToSortLineEdit(%s)\n", pname);
    return( showColumnToSortLineEdit->text() );
  }

  QString getPreferenceTopNLineEdit()
  {
printf("getPreferenceTopNLineEdit(%s)\n", pname);
    return( showTopNLineEdit->text() );
  }

  void initPreferenceSettings()
  {
printf("initPreferenceSettings(%s)\n", pname);
    sortDecendingCheckBox->setChecked(TRUE);
    showTopNLineEdit->setText( "5" );
    showColumnToSortLineEdit->setText( "0" );
  }

  QWidget *initialize_preferences_entry_point(QSettings *settings, QWidgetStack *stack, char *name)
  {
printf("initialize_preferences_entry_point(0x%x 0x%x %s) entered\n", settings, stack, name);

    QWidget *statsPanelStackPage = new QWidget( stack, name );
    pname = strdup(name);

    generalStackPageLayout_3 =
      new QVBoxLayout( statsPanelStackPage, 11, 6, "generalStackPageLayout_3");

    statsPanelGroupBox = new QGroupBox( statsPanelStackPage, "statsPanelGroupBox" );

    QWidget* statsPanelPrivateLayout = new QWidget( statsPanelGroupBox, "layout8" );
//    statsPanelPrivateLayout->setGeometry( QRect( 20, 86, 271, 57 ) );
    statsPanelPrivateLayout->setGeometry( QRect( 20, 86, 300, 100 ) );
    layout8 = new QVBoxLayout( statsPanelPrivateLayout, 11, 6, "layout8");

    sortDecendingCheckBox =
      new QCheckBox( statsPanelPrivateLayout, "sortDecendingCheckBox" );
    layout8->addWidget( sortDecendingCheckBox );

    layoutTopN = new QHBoxLayout( 0, 0, 6, "layoutTopN");

    showTopNTextLabel =
      new QLabel( statsPanelPrivateLayout, "showTopNTextLabel" );
    layoutTopN->addWidget( showTopNTextLabel );

    showTopNLineEdit =
      new QLineEdit( statsPanelPrivateLayout, "showTopNLineEdit" );

    layoutTopN->addWidget( showTopNLineEdit );
    layout8->addLayout( layoutTopN );




    layoutColumnToSort = new QHBoxLayout( 0, 0, 6, "layoutTopN");

    showColumnToSortTextLabel =
      new QLabel( statsPanelPrivateLayout, "showColumnToSortTextLabel" );
    layoutColumnToSort->addWidget( showColumnToSortTextLabel );

    showColumnToSortLineEdit =
      new QLineEdit( statsPanelPrivateLayout, "showColumnToSortLineEdit" );

    layoutColumnToSort->addWidget( showColumnToSortLineEdit );
    layout8->addLayout( layoutColumnToSort );



    generalStackPageLayout_3->addWidget( statsPanelGroupBox );
    stack->addWidget( statsPanelStackPage, 2 );

    statsPanelGroupBox->setTitle( "Stats Panel" );
    sortDecendingCheckBox->setText( "Sort Descending" );
    showTopNTextLabel->setText( "Show top N items:" );
    showColumnToSortTextLabel->setText( "Column to sort:" );

    initPreferenceSettings();

    if( settings != NULL )
    {
      char settings_buffer[1024];
      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, sortDecendingCheckBox->name() );
      sortDecendingCheckBox->setChecked(
        settings->readBoolEntry(settings_buffer) );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showTopNLineEdit->name() );
      showTopNLineEdit->setText(
        settings->readEntry(settings_buffer, "5") );
    }

    return statsPanelStackPage;
  }
  int save_preferences_entry_point(QSettings *settings, char *name)
  {
printf("save_preferences_entry_point(0x%x %s) entered\n", settings, name);

    char settings_buffer[1024];

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, sortDecendingCheckBox->name() );
    settings->writeEntry(settings_buffer, sortDecendingCheckBox->isChecked() );

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showTopNLineEdit->name() );
    settings->writeEntry(settings_buffer, showTopNLineEdit->text() );
  }
}

#endif // PREFERENCE_PLUGIN_INFO_H

