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
#include <qsettings.h>

#include <qwidgetstack.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qsettings.h>


extern "C"
{
  QWidget *manageProcessesPanelStackPage;
  QGroupBox* managerProcessesPanelGroupBox;
  QVBoxLayout* generalStackPageLayout_2;
  QVBoxLayout* layout6;
  QCheckBox* sortPreferenceCheckBox;

  static char *pname = NULL;

  bool getSortPreference()
  {
    return( sortPreferenceCheckBox->isChecked() );
  }

  void initManageProcessesPanelPreferenceSettings()
  {
    sortPreferenceCheckBox->setChecked(FALSE);
  }

  QWidget *initialize_preferences_entry_point(QSettings *settings, QWidgetStack *stack, char *name)
  {
//printf("initialize_preferences_entry_point(0x%x 0x%x %s) entered\n", settings, stack, name);

    // Add your preferences to initialize here.   See Source Panel for 
    // an example.

    manageProcessesPanelStackPage = new QWidget( stack, name );
    pname = strdup(name);

    generalStackPageLayout_2 = new QVBoxLayout( manageProcessesPanelStackPage, 11, 6, "generalStackPageLayout_2");

    managerProcessesPanelGroupBox = new QGroupBox( manageProcessesPanelStackPage, "managerProcessesPanelGroupBox" );
    managerProcessesPanelGroupBox->setColumnLayout(0, Qt::Vertical );
    managerProcessesPanelGroupBox->layout()->setSpacing( 6 );
    managerProcessesPanelGroupBox->layout()->setMargin( 11 );

    layout6 = new QVBoxLayout( managerProcessesPanelGroupBox->layout(), 11, "layout6");

    sortPreferenceCheckBox =
      new QCheckBox( managerProcessesPanelGroupBox, "sortPreferenceCheckBox" );
    sortPreferenceCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7
, (QSizePolicy::SizeType)0, 0, 0, sortPreferenceCheckBox->sizePolicy().hasHeightForWidth() ) );
    layout6->addWidget( sortPreferenceCheckBox );
    generalStackPageLayout_2->addWidget( managerProcessesPanelGroupBox );
    stack->addWidget( manageProcessesPanelStackPage, 1 );

    managerProcessesPanelGroupBox->setTitle( "Manage Processes Panel" );
    sortPreferenceCheckBox->setText( "Sort Preference" );

    initManageProcessesPanelPreferenceSettings();

    return( manageProcessesPanelStackPage );
  }
  void save_preferences_entry_point(QSettings *settings, char *name)
  {
//printf("save_preferences_entry_point() entered\n");
    // Add your preferences to save here.   See Source Panel for 
    // an example.
  }
}

#endif // PREFERENCE_PLUGIN_INFO_H
