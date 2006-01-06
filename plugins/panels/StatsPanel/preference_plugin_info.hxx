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
  QWidget* statsPanelStackPage = NULL;
  QVBoxLayout* generalStackPageLayout_3;
  QGroupBox* statsPanelGroupBox;
  QVBoxLayout* layout8;

  QCheckBox* sortDecendingCheckBox;

  QHBoxLayout* layoutTopN;
  QHBoxLayout* layoutTopNChart;

  QLabel* levelsToOpenTextLabel;
  QLineEdit* levelsToOpenLineEdit;

  QLabel* showTopNTextLabel;
  QLineEdit* showTopNLineEdit;

  QLabel* showTopNChartTextLabel;
  QLineEdit* showTopNChartLineEdit;

  QHBoxLayout* layoutColumnToSort;
  QLabel* showColumnToSortTextLabel;

  QLineEdit* showColumnToSortLineEdit;

  QCheckBox* showTextInChartCheckBox;

  static char *pname = NULL;

  bool getPreferenceSortDecending()
  {
// printf("getPreferenceSortDecending(%s)\n", pname);
    return( sortDecendingCheckBox->isChecked() );
  }

  QString getPreferenceLevelsToOpen()
  {
// printf("getPreferenceLevelsToOpen(%s)\n", pname);
    return( levelsToOpenLineEdit->text() );
  }

  QString getPreferenceColumnToSortLineEdit()
  {
// printf("getPreferenceColumnToSortLineEdit(%s)\n", pname);
    return( showColumnToSortLineEdit->text() );
  }

  QString getPreferenceTopNLineEdit()
  {
// printf("getPreferenceTopNLineEdit(%s)\n", pname);
    return( showTopNLineEdit->text() );
  }

  QString getPreferenceTopNChartLineEdit()
  {
// printf("getPreferenceTopNChartLineEdit(%s)\n", pname);
    return( showTopNChartLineEdit->text() );
  }


  bool getPreferenceShowTextInChart()
  {
// printf("getPreferenceShowTextInChart(%s)\n", pname);
    return( showTextInChartCheckBox->isChecked() );
  }


  void initPreferenceSettings()
  {
// printf("StatsPanel: initPreferenceSettings(%s)\n", pname);
    sortDecendingCheckBox->setChecked(TRUE);
    levelsToOpenLineEdit->setText( "-1" );
    showTopNLineEdit->setText( "10" );
    showTopNChartLineEdit->setText( "5" );
    showColumnToSortLineEdit->setText( "0" );
    showTextInChartCheckBox->setChecked(TRUE);
  }

  QWidget *initialize_preferences_entry_point(QSettings *settings, QWidgetStack *stack, char *name)
  {
// printf("StatsPanel: initialize_preferences_entry_point(0x%x 0x%x %s) entered\n", settings, stack, name);

    statsPanelStackPage = new QWidget( stack, name );
    pname = strdup(name);

    generalStackPageLayout_3 =
      new QVBoxLayout( statsPanelStackPage, 11, 6, "generalStackPageLayout_3");

    statsPanelGroupBox = new QGroupBox( statsPanelStackPage, "statsPanelGroupBox" );
    statsPanelGroupBox->setColumnLayout(0, Qt::Vertical );
    statsPanelGroupBox->layout()->setSpacing( 6 );
    statsPanelGroupBox->layout()->setMargin( 11 );

    layout8 = new QVBoxLayout( statsPanelGroupBox->layout(), 11, "layout8");

    QHBoxLayout *layoutLevelsToOpen = new QHBoxLayout( 0, 0, 6, "layoutLevelsToOpen");

    levelsToOpenTextLabel =
      new QLabel( statsPanelGroupBox, "levelsToOpenTextLabel" );
    layoutLevelsToOpen->addWidget( levelsToOpenTextLabel );

    levelsToOpenLineEdit =
      new QLineEdit( statsPanelGroupBox, "levelsToOpenLineEdit" );

    layoutLevelsToOpen->addWidget( levelsToOpenLineEdit );

    layout8->addLayout( layoutLevelsToOpen );

    layoutTopN = new QHBoxLayout( 0, 0, 6, "layoutTopN");

    showTopNTextLabel =
      new QLabel( statsPanelGroupBox, "showTopNTextLabel" );
    layoutTopN->addWidget( showTopNTextLabel );

    showTopNLineEdit =
      new QLineEdit( statsPanelGroupBox, "showTopNLineEdit" );

    layoutTopN->addWidget( showTopNLineEdit );

    layout8->addLayout( layoutTopN );

    layoutTopNChart = new QHBoxLayout( 0, 0, 6, "layoutTopNChart");

    showTopNChartTextLabel =
      new QLabel( statsPanelGroupBox, "showTopNChartTextLabel" );
    layoutTopNChart->addWidget( showTopNChartTextLabel );

    showTopNChartLineEdit =
      new QLineEdit( statsPanelGroupBox, "showTopNChartLineEdit" );

    layoutTopNChart->addWidget( showTopNChartLineEdit );

    layout8->addLayout( layoutTopNChart );


    showTextInChartCheckBox =
      new QCheckBox( statsPanelGroupBox, "showTextInChartCheckBox" );
    showTextInChartCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showTextInChartCheckBox->sizePolicy().hasHeightForWidth() ) );
    layout8->addWidget( showTextInChartCheckBox );

    sortDecendingCheckBox =
      new QCheckBox( statsPanelGroupBox, "sortDecendingCheckBox" );
    sortDecendingCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, sortDecendingCheckBox->sizePolicy().hasHeightForWidth() ) );
    layout8->addWidget( sortDecendingCheckBox );



    layoutColumnToSort = new QHBoxLayout( 0, 0, 6, "layoutTopN");

    showColumnToSortTextLabel =
      new QLabel( statsPanelGroupBox, "showColumnToSortTextLabel" );
    layoutColumnToSort->addWidget( showColumnToSortTextLabel );

    showColumnToSortLineEdit =
      new QLineEdit( statsPanelGroupBox, "showColumnToSortLineEdit" );

    layoutColumnToSort->addWidget( showColumnToSortLineEdit );
    layout8->addLayout( layoutColumnToSort );



    generalStackPageLayout_3->addWidget( statsPanelGroupBox );
    stack->addWidget( statsPanelStackPage, 2 );

    statsPanelGroupBox->setTitle( "Stats Panel" );
    sortDecendingCheckBox->setText( "Sort Descending" );
    levelsToOpenTextLabel->setText( "Open this many levels in display:" );
    showTopNTextLabel->setText( "Show top N items in list:" );
    showTopNChartTextLabel->setText( "Show top N items in chart:" );
    showColumnToSortTextLabel->setText( "Column to sort:" );
    showTextInChartCheckBox->setText( "Show text in chart:" );

//    initStatsPanelPreferenceSettings();
    initPreferenceSettings();

    if( settings != NULL )
    {
      char settings_buffer[1024];
      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, sortDecendingCheckBox->name() );
      sortDecendingCheckBox->setChecked(
        settings->readBoolEntry(settings_buffer, TRUE) );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, levelsToOpenLineEdit->name() );
      levelsToOpenLineEdit->setText(
        settings->readEntry(settings_buffer, "-1") );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showTopNLineEdit->name() );
      showTopNLineEdit->setText(
        settings->readEntry(settings_buffer, "10") );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showTopNChartLineEdit->name() );
      showTopNChartLineEdit->setText(
        settings->readEntry(settings_buffer, "5") );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showColumnToSortLineEdit->name() );
      showColumnToSortLineEdit->setText(
        settings->readEntry(settings_buffer, "0") );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showTextInChartCheckBox->name() );
      showTextInChartCheckBox->setChecked(
        settings->readBoolEntry(settings_buffer, TRUE) );
    }

    return statsPanelStackPage;
  }
  int save_preferences_entry_point(QSettings *settings, char *name)
  {
// printf("save_preferences_entry_point(0x%x %s) entered\n", settings, name);

    char settings_buffer[1024];

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, levelsToOpenLineEdit->name() );
    settings->writeEntry(settings_buffer, levelsToOpenLineEdit->text() );

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, sortDecendingCheckBox->name() );
    settings->writeEntry(settings_buffer, sortDecendingCheckBox->isChecked() );

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showTopNLineEdit->name() );
    settings->writeEntry(settings_buffer, showTopNLineEdit->text() );

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showTopNChartLineEdit->name() );
    settings->writeEntry(settings_buffer, showTopNChartLineEdit->text() );

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showTextInChartCheckBox->name() );
    settings->writeEntry(settings_buffer, showTextInChartCheckBox->isChecked() );

  }
}

#endif // PREFERENCE_PLUGIN_INFO_H

