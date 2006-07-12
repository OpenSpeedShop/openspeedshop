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
#include <qlistview.h>
#include <qtooltip.h>

#include <SPCheckBox.hxx>

#include <qcombobox.h>
#include "chartform.hxx"
#include "images/options_horizontalbarchart.xpm"
#include "images/options_piechart.xpm"
#include "images/options_verticalbarchart.xpm"

extern "C"
{
  SPCheckBoxList checkBoxList;
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


  QGroupBox *textLabelGroupBox;
  SPCheckBox* showTextInChartCheckBox;
  SPCheckBox* showTextByValueCheckBox;
  SPCheckBox* showTextByPercentCheckBox;
  SPCheckBox* showTextByLocationCheckBox;
  QComboBox *chartTypeComboBox;

  QHBoxLayout* layoutSkyline;
  QCheckBox* showToolbarCheckBox;
  QCheckBox* showSkylineCheckBox;
  QLabel* showSkylineLabel;
  QLineEdit* showSkylineLineEdit;

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

  STATSPANEL_TEXT_ENUM getPreferenceShowTextInChart()
  {
// printf("getPreferenceShowTextInChart(%s)\n", pname);
    if( showTextInChartCheckBox->isChecked() )
    {
      if( showTextByValueCheckBox->isChecked() )
      {
        return( TEXT_BYVALUE );
      }

      if( showTextByPercentCheckBox->isChecked() )
      {
        return( TEXT_BYPERCENT );
      }

      if( showTextByLocationCheckBox->isChecked() )
      {
        return( TEXT_BYLOCATION );
      }
    }
    return( TEXT_NONE );
  }

  int getChartTypeComboBox()
  {
// printf("getChartTypeComboBox(%s)\n", pname);
    return( chartTypeComboBox->currentItem() );
  }

  bool getPreferenceShowToolbarCheckBox()
  {
// printf("getPreferenceShowToolbarCheckBox(%s)\n", pname);
    return( showToolbarCheckBox->isChecked() );
  }

  bool getPreferenceShowSkyline()
  {
// printf("getPreferenceShowSkyline(%s)\n", pname);
    return( showSkylineCheckBox->isChecked() );
  }

  QString getPreferenceShowSkyLineLineEdit()
  {
// printf("getPreferenceshowSkylineLineEdit(%s)\n", pname);
    return( showSkylineLineEdit->text() );
  }

  void initPreferenceSettings()
  {
    checkBoxList.clear();
// printf("StatsPanel: initPreferenceSettings(%s)\n", pname);
    sortDecendingCheckBox->setChecked(TRUE);
    levelsToOpenLineEdit->setText( "-1" );
    showTopNLineEdit->setText( "20" );
    showTopNChartLineEdit->setText( "5" );
    showColumnToSortLineEdit->setText( "0" );
    showTextInChartCheckBox->setChecked(TRUE);
    showTextByValueCheckBox->setChecked(FALSE);
    showTextByPercentCheckBox->setChecked(TRUE);
    showTextByLocationCheckBox->setChecked(FALSE);
    chartTypeComboBox->setCurrentItem(0);
    showToolbarCheckBox->setChecked(FALSE);
    showSkylineCheckBox->setChecked(FALSE);
    showSkylineLabel->setText("Percentage breakdown for skyline view:");
    showSkylineLineEdit->setText("25");
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

    textLabelGroupBox = new QGroupBox( statsPanelGroupBox, "textLabelGroupBox" );
    textLabelGroupBox->setColumnLayout(0, Qt::Vertical );
    textLabelGroupBox->layout()->setSpacing( 6 );
    textLabelGroupBox->layout()->setMargin( 11 );
    textLabelGroupBox->setTitle( "Chart Preferences:" );

    QVBoxLayout *textLabelLayout = new QVBoxLayout( textLabelGroupBox->layout(), 11, "layout8");

    showTextInChartCheckBox =
      new SPCheckBox( textLabelGroupBox, "showTextInChartCheckBox");
    showTextInChartCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showTextInChartCheckBox->sizePolicy().hasHeightForWidth() ) );
    textLabelLayout->addWidget( showTextInChartCheckBox );

    showTextByValueCheckBox =
      new SPCheckBox( textLabelGroupBox, "showTextByValueCheckBox", &checkBoxList );
    showTextByValueCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showTextByValueCheckBox->sizePolicy().hasHeightForWidth() ) );
    textLabelLayout->addWidget( showTextByValueCheckBox );
    checkBoxList.push_back( showTextByValueCheckBox );


    showTextByPercentCheckBox =
      new SPCheckBox( textLabelGroupBox, "showTextByPercentCheckBox", &checkBoxList );
    showTextByPercentCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showTextByPercentCheckBox->sizePolicy().hasHeightForWidth() ) );
    textLabelLayout->addWidget( showTextByPercentCheckBox );
    checkBoxList.push_back( showTextByPercentCheckBox );

    showTextByLocationCheckBox =
      new SPCheckBox( textLabelGroupBox, "showTextByLocationCheckBox", &checkBoxList );
    showTextByLocationCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showTextByLocationCheckBox->sizePolicy().hasHeightForWidth() ) );
    textLabelLayout->addWidget( showTextByLocationCheckBox );
    checkBoxList.push_back( showTextByLocationCheckBox );


chartTypeComboBox = new QComboBox( FALSE, textLabelGroupBox, "chartTypeComboBox" );
chartTypeComboBox->insertItem( QPixmap( options_piechart ), "Pie Chart" );
chartTypeComboBox->insertItem( QPixmap( options_verticalbarchart ),
                   "Vertical Bar Chart" );
chartTypeComboBox->insertItem( QPixmap( options_horizontalbarchart ),
                   "Horizontal Bar Chart" );
textLabelLayout->addWidget( chartTypeComboBox );

    layout8->addWidget( textLabelGroupBox );

    showToolbarCheckBox =
      new QCheckBox( statsPanelGroupBox, "showToolbarCheckBox" );
    showToolbarCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showToolbarCheckBox->sizePolicy().hasHeightForWidth() ) );
    layout8->addWidget( showToolbarCheckBox );

    showSkylineCheckBox =
      new QCheckBox( statsPanelGroupBox, "showSkylineCheckBox" );
    showSkylineCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showSkylineCheckBox->sizePolicy().hasHeightForWidth() ) );
    layout8->addWidget( showSkylineCheckBox );

    layoutSkyline = new QHBoxLayout( 0, 0, 6, "layoutSkyline");
    showSkylineLabel =
      new QLabel( statsPanelGroupBox, "showSkylineLabel" );
    layoutSkyline->addWidget( showSkylineLabel );

    showSkylineLineEdit =
      new QLineEdit( statsPanelGroupBox, "showSkylineLineEdit" );
    layoutSkyline->addWidget( showSkylineLineEdit );
    layout8->addLayout( layoutSkyline );


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
    showTextByValueCheckBox->setText( "  Show text by value:" );
    showSkylineLabel->setText("Percentage breakdown for skyline view:");
    showTextByPercentCheckBox->setText( "  Show text by percent:" );
    showTextByLocationCheckBox->setText( "  Show text by location:" );
    showToolbarCheckBox->setText( "  Show toolbar:" );
    showSkylineCheckBox->setText( "  Show skyline view in time dialog:" );

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
        settings->readEntry(settings_buffer, "20") );

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

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showTextByValueCheckBox->name() );
      showTextByValueCheckBox->setChecked(
        settings->readBoolEntry(settings_buffer, TRUE) );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showTextByPercentCheckBox->name() );
      showTextByPercentCheckBox->setChecked(
        settings->readBoolEntry(settings_buffer, TRUE) );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showTextByLocationCheckBox->name() );
      showTextByLocationCheckBox->setChecked(
        settings->readBoolEntry(settings_buffer, TRUE) );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, chartTypeComboBox->name() );
      chartTypeComboBox->setCurrentItem(
        settings->readNumEntry(settings_buffer, 0) );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showToolbarCheckBox->name() );
      showToolbarCheckBox->setChecked(
        settings->readBoolEntry(settings_buffer, FALSE) );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showSkylineCheckBox->name() );
      showSkylineCheckBox->setChecked(
        settings->readBoolEntry(settings_buffer, FALSE) );

      sprintf(settings_buffer, "/%s/%s/%s",
        "openspeedshop", name, showSkylineLineEdit->name() );
      showSkylineLineEdit->setText(
        settings->readEntry(settings_buffer, "25") );
    }

    QToolTip::add(showTopNLineEdit,
      "Define the top number of statistic entries to display.\nTo show all entries set to -1 or blank the field.  The default is 10." );
    QToolTip::add(showTopNChartLineEdit,
      "Define the top number of entries to display in the graphic.\nThe default is 5.");

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

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showTextByValueCheckBox->name() );
    settings->writeEntry(settings_buffer, showTextByValueCheckBox->isChecked() );

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showTextByPercentCheckBox->name() );
    settings->writeEntry(settings_buffer, showTextByPercentCheckBox->isChecked() );

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showTextByLocationCheckBox->name() );
    settings->writeEntry(settings_buffer, showTextByLocationCheckBox->isChecked() );

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, chartTypeComboBox->name() );
    settings->writeEntry(settings_buffer, chartTypeComboBox->currentItem() );

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showToolbarCheckBox->name() );
    settings->writeEntry(settings_buffer, showToolbarCheckBox->isChecked() );

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showSkylineCheckBox->name() );
    settings->writeEntry(settings_buffer, showSkylineCheckBox->isChecked() );

    sprintf(settings_buffer, "/%s/%s/%s",
      "openspeedshop", name, showSkylineLineEdit->name() );
    settings->writeEntry(settings_buffer, showSkylineLineEdit->text() );
  }
}

#endif // PREFERENCE_PLUGIN_INFO_H

