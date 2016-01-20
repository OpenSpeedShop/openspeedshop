////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2016 Krell Institute  All Rights Reserved.
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
  

#include <qsettings.h>
#include <qapplication.h>
#include <qvalidator.h>
#include <qmessagebox.h>

#include "openspeedshop.hxx"
#include "PluginInfo.hxx"
#include <ltdl.h>
#include <assert.h>

#include "PreferencesChangedObject.hxx"

void PreferencesDialog::readPreferencesOnEntry()
{
// printf("readPreferencesOnEntry() entered\n");

//  settings->insertSearchPath( QSettings::Unix, "openspeedshop" );
  // No search path needed for Unix; see notes further on.
  QString ds = QString::null;
  bool ok;

  settings->readEntry( "/openspeedshop/general/globalFontFamily", ds, &ok);
  if( ok == FALSE )
  {
//    printf("Didn't find any defaults file.  Just take the defaults.\n");
    return;
  } else
  {
    // printf(" ******     found a defaults file.\n");
  }

  preferencesAvailable = TRUE;

  globalFontFamily =
    settings->readEntry( "/openspeedshop/general/globalFontFamily" );
  fontLineEdit->setText( globalFontFamily );

  globalRemoteShell =
    settings->readEntry( "/openspeedshop/general/globalRemoteShell" );
  remoteShellEdit->setText( globalRemoteShell );

  globalFontPointSize =
    settings->readNumEntry("/openspeedshop/general/globalFontPointSize");
  globalFontWeight =
    settings->readNumEntry("/openspeedshop/general/globalFontWeight");
  globalFontItalic =
     settings->readBoolEntry("/openspeedshop/general/globalFontItalic");


  viewFieldSize =
    settings->readNumEntry("/openspeedshop/general/viewFieldSize");
  viewFieldSizeLineEdit->setText( QString("%1").arg(viewFieldSize) );

  viewPrecision =
    settings->readNumEntry("/openspeedshop/general/viewPrecision");
  viewPrecisionLineEdit->setText( QString("%1").arg(viewPrecision) );

  historyLimit =
    settings->readNumEntry("/openspeedshop/general/historyLimit");
  historyLimitLineEdit->setText( QString("%1").arg(historyLimit) );

  historyDefault =
    settings->readNumEntry("/openspeedshop/general/historyDefault");
  historyDefaultLineEdit->setText( QString("%1").arg(historyDefault) );

  maxAsyncCommands =
    settings->readNumEntry("/openspeedshop/general/maxAsyncCommands");
  maxAsyncCommandsLineEdit->setText( QString("%1").arg(maxAsyncCommands) );

  helpLevelDefault =
    settings->readNumEntry("/openspeedshop/general/helpLevelDefault");
  helpLevelDefaultLineEdit->setText( QString("%1").arg(helpLevelDefault) );

  viewFullPathCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/viewFullPath") );

  saveExperimentDatabaseCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/saveExperimentDatabase") );

  askAboutSavingTheDatabaseCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/askAboutSavingTheDatabase") );

  askAboutChangingArgsCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/askAboutChangingArgs") );

  onRerunSaveCopyOfExperimentDatabaseCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/onRerunSaveCopyOfExperimentDatabase") );

  viewMangledNameCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/viewMangledName") );

  allowPythonCommandsCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/allowPythonCommands") );

  instrumentorIsOfflineCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/instrumentorIsOffline") );

  lessRestrictiveComparisonsCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/lessRestrictiveComparisons") );

  viewBlankInPlaceOfZeroCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/viewBlankInPlaceOfZero") );

  setShowSplashScreenCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/showSplashScreen") );

  showGraphicsCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/showGraphics") );

  viewDateTimePrecision =
    settings->readNumEntry("/openspeedshop/general/viewDateTimePrecision");

  viewFieldSizeIsDynamicCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/viewFieldSizeIsDynamic") );

  autoCreateDerivedMetricsCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/autoCreateDerivedMetrics") );

  saveViewsForReuseCheckBox->setChecked(
    settings->readBoolEntry( "/openspeedshop/general/saveViewsForReuse") );

  saveViewsTime =
    settings->readNumEntry("/openspeedshop/general/saveViewsTime");
  saveViewsTimeLineEdit->setText( QString("%1").arg(saveViewsTime) );
}

void PreferencesDialog::resetPreferenceDefaults()
{
//    qWarning( "PreferencesDialog::resetPreferenceDefaults(): Not implemented yet" );
// jeg - these didn't match with the other file causing a switch in fonts when the user changed other preferences.
//   globalFontFamily = "Sans Serif";
//   globalFontPointSize = 10;

   globalFontFamily = "Bitstream Charter";
   globalFontPointSize = 10;

   globalFontWeight = QFont::Normal;
   globalFontItalic = FALSE;

   fontLineEdit->setText( globalFontFamily );
   fontLineEdit->setReadOnly(TRUE);

   globalRemoteShell = "/usr/bin/ssh";
   remoteShellEdit->setText( globalRemoteShell );
   remoteShellEdit->setReadOnly(FALSE);

   setShowSplashScreenCheckBox->setChecked( TRUE );
   showGraphicsCheckBox->setChecked(FALSE);

   viewFieldSize = 20;
   viewFieldSizeLineEdit->setText(QString("%1").arg(viewFieldSize));
   viewPrecision = 6;
   viewPrecisionLineEdit->setText(QString("%1").arg(viewPrecision));
   historyLimit = 100;
   historyLimitLineEdit->setText(QString("%1").arg(historyLimit));
   historyDefault = 24;
   historyDefaultLineEdit->setText(QString("%1").arg(historyDefault));
   maxAsyncCommands = 20;
   maxAsyncCommandsLineEdit->setText(QString("%1").arg(maxAsyncCommands));
   helpLevelDefault = 2;
   helpLevelDefaultLineEdit->setText(QString("%1").arg(helpLevelDefault));
   viewFullPath = FALSE;
   viewFullPathCheckBox->setChecked(viewFullPath);
   saveExperimentDatabase = TRUE;
   saveExperimentDatabaseCheckBox->setChecked(saveExperimentDatabase);
   askAboutChangingArgs = TRUE;
   askAboutChangingArgsCheckBox->setChecked(askAboutChangingArgs);
   askAboutSavingTheDatabase = TRUE;
   askAboutSavingTheDatabaseCheckBox->setChecked(askAboutSavingTheDatabase);
   onRerunSaveCopyOfExperimentDatabase = TRUE;
   onRerunSaveCopyOfExperimentDatabaseCheckBox->setChecked(onRerunSaveCopyOfExperimentDatabase);
   viewMangledName = FALSE;
   viewMangledNameCheckBox->setChecked(viewMangledName);
   allowPythonCommands = TRUE;
   allowPythonCommandsCheckBox->setChecked(allowPythonCommands);
   instrumentorIsOffline = TRUE;
   instrumentorIsOfflineCheckBox->setChecked(instrumentorIsOffline);
   lessRestrictiveComparisons = TRUE;
   lessRestrictiveComparisonsCheckBox->setChecked(lessRestrictiveComparisons);
   viewBlankInPlaceOfZero = TRUE;
   viewBlankInPlaceOfZeroCheckBox->setChecked(viewBlankInPlaceOfZero);
   viewDateTimePrecision = 3;
   viewDateTimePrecisionLineEdit->setText(QString("%1").arg(viewDateTimePrecision));
   viewFieldSizeIsDynamic = TRUE;
   viewFieldSizeIsDynamicCheckBox->setChecked(viewFieldSizeIsDynamic);
   autoCreateDerivedMetrics = TRUE;
   saveViewsForReuse = FALSE;
   saveViewsForReuseCheckBox->setChecked(saveViewsForReuse);
   saveViewsTime = 10;
   saveViewsTimeLineEdit->setText(QString("%1").arg(saveViewsTime));


  // Begin reset all preferences to defaults
  //This is the base plugin directory.   In this directory there should
  // be a list of dso (.so) which are the plugins.
  char plugin_file[1024];
  if( panelContainer->getMasterPC() && panelContainer->getMasterPC()->_pluginRegistryList )
  {
    // Insure the libltdl user-defined library search path has been set
    assert(lt_dlgetsearchpath() != NULL);
    // Now search for plugins preferences in this path
    PluginInfo *pi = NULL;
    for( PluginRegistryList::Iterator it = panelContainer->getMasterPC()->_pluginRegistryList->begin();
         it != panelContainer->getMasterPC()->_pluginRegistryList->end();
         it++ )
    {
      pi = (PluginInfo *)*it;
      lt_dlhandle dl_object = lt_dlopenext((const char *)pi->plugin_name);
  
      if( dl_object != NULL )
      {
// printf("about to lookup(%s).\n", "initPreferenceSettings");
        void (*dl_plugin_init_preferences_settings)() =
          (void (*)())lt_dlsym(dl_object, "initPreferenceSettings" );
         if( dl_plugin_init_preferences_settings )
         {
// printf("about to call the routine.\n");
           (*dl_plugin_init_preferences_settings)();
         }
         lt_dlclose(dl_object);
      }
    }
  }
}

QWidget *
PreferencesDialog::matchPreferencesToStack(QString s)
{
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

void PreferencesDialog::listItemSelected(QListViewItem*lvi)
{
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

#include <qfontdialog.h>
void PreferencesDialog::selectGlobalFont()
{
  bool ok;
  QFont font = QFontDialog::getFont( &ok, this );
  if ( ok )
  { // font is set to the font the user selected
    globalFontFamily = font.family();
    fontLineEdit->setText( globalFontFamily );
    globalFontPointSize = font.pointSize();
//  fontLineEdit->setText( globalFontFamily );
    globalFontWeight = font.weight();
//  fontLineEdit->setText( globalFontFamily );
    globalFontItalic = font.italic();
//  fontLineEdit->setText( globalFontFamily );
  }
}

void PreferencesDialog::applyPreferences()
{

  QFont *m_font = new QFont( globalFontFamily,
                      globalFontPointSize,
                      globalFontWeight,
                      globalFontItalic );
  qApp->setFont(*m_font, TRUE);

  delete( m_font );

  globalRemoteShell = remoteShellEdit->text();

{ // cli specific prefrences
  extern int64_t OPENSS_VIEW_DATE_TIME_PRECISION;
  extern int64_t OPENSS_VIEW_FIELD_SIZE;
  extern int64_t OPENSS_VIEW_PRECISION;
  extern int64_t OPENSS_HISTORY_LIMIT;
  extern int64_t OPENSS_HISTORY_DEFAULT;
  extern int64_t OPENSS_MAX_ASYNC_COMMANDS;
  extern int64_t OPENSS_HELP_LEVEL_DEFAULT;
  extern bool    OPENSS_VIEW_FIELD_SIZE_IS_DYNAMIC;
  extern bool    OPENSS_AUTO_CREATE_DERIVED_METRICS;
  extern bool    OPENSS_SAVE_VIEWS_FOR_REUSE;
  extern int64_t OPENSS_SAVE_VIEWS_TIME;
  extern bool    OPENSS_VIEW_FULLPATH;
  extern bool    OPENSS_SAVE_EXPERIMENT_DATABASE;
  extern bool    OPENSS_ASK_ABOUT_CHANGING_ARGS;
  extern bool    OPENSS_ASK_ABOUT_SAVING_THE_DATABASE;
  extern bool    OPENSS_ON_RERUN_SAVE_COPY_OF_EXPERIMENT_DATABASE;
  extern bool    OPENSS_ALLOW_PYTHON_COMMANDS;
  extern bool    OPENSS_INSTRUMENTOR_IS_OFFLINE;
  extern bool    OPENSS_VIEW_USE_BLANK_IN_PLACE_OF_ZERO;
  extern bool    OPENSS_LESS_RESTRICTIVE_COMPARISONS;
  extern bool    OPENSS_LOG_BY_DEFAULT;
  extern bool    OPENSS_LIMIT_SIGNAL_CATCHING;
  extern bool    OPENSS_VIEW_MANGLED_NAME;


  OPENSS_ASK_ABOUT_SAVING_THE_DATABASE =
    askAboutSavingTheDatabaseCheckBox->isChecked();
  OPENSS_ASK_ABOUT_CHANGING_ARGS =
    askAboutChangingArgsCheckBox->isChecked();
  OPENSS_ON_RERUN_SAVE_COPY_OF_EXPERIMENT_DATABASE =
    onRerunSaveCopyOfExperimentDatabaseCheckBox->isChecked();
  OPENSS_SAVE_EXPERIMENT_DATABASE =
    saveExperimentDatabaseCheckBox->isChecked();
  OPENSS_VIEW_MANGLED_NAME =
    viewMangledNameCheckBox->isChecked();
  OPENSS_ALLOW_PYTHON_COMMANDS = 
    allowPythonCommandsCheckBox->isChecked();
  OPENSS_INSTRUMENTOR_IS_OFFLINE = 
    instrumentorIsOfflineCheckBox->isChecked();
  OPENSS_LESS_RESTRICTIVE_COMPARISONS = 
    lessRestrictiveComparisonsCheckBox->isChecked();
  OPENSS_VIEW_USE_BLANK_IN_PLACE_OF_ZERO = 
    viewBlankInPlaceOfZeroCheckBox->isChecked();
  OPENSS_VIEW_FULLPATH =
    viewFullPathCheckBox->isChecked();
  OPENSS_VIEW_FIELD_SIZE_IS_DYNAMIC = viewFieldSizeIsDynamicCheckBox->isChecked();
  OPENSS_AUTO_CREATE_DERIVED_METRICS = autoCreateDerivedMetricsCheckBox->isChecked();
  OPENSS_SAVE_VIEWS_FOR_REUSE = saveViewsForReuseCheckBox->isChecked();
  OPENSS_SAVE_VIEWS_TIME = saveViewsTimeLineEdit->text().toInt();
  OPENSS_HELP_LEVEL_DEFAULT = helpLevelDefaultLineEdit->text().toInt();
  OPENSS_MAX_ASYNC_COMMANDS = maxAsyncCommandsLineEdit->text().toInt();
  if(  OPENSS_MAX_ASYNC_COMMANDS <= 0 )
  {
    OPENSS_MAX_ASYNC_COMMANDS = 20;
    maxAsyncCommandsLineEdit->setText("20");
    QString msg(tr("Define the maximum number of commands that can be processed at\nthe same time. This is a limit on the parallel execution of\ncommands in Open|SpeedShop and controls the degree to which commands\ncan be overlapped.  The default is 20.  This value must be greater than zero.\n"));
    QMessageBox::information( (QWidget *)this, "Invalid maximum number of commands...", msg, QMessageBox::Ok );
  }
  OPENSS_HISTORY_DEFAULT = historyDefaultLineEdit->text().toInt();
  OPENSS_HISTORY_LIMIT = historyLimitLineEdit->text().toInt();
  OPENSS_VIEW_PRECISION = viewPrecisionLineEdit->text().toInt();
  OPENSS_VIEW_FIELD_SIZE = viewFieldSizeLineEdit->text().toInt();
  OPENSS_VIEW_DATE_TIME_PRECISION = viewDateTimePrecisionLineEdit->text().toInt();
} // cli specific preferences


  // NOTIFY EVERYONE THAT PREFERENCES HAVE CHANGED!
  PreferencesChangedObject msg = PreferencesChangedObject();
  panelContainer->getMasterPC()->notifyAll((char *)&msg);

}

void PreferencesDialog::buttonApplySelected()
{
    applyPreferences();
}

void PreferencesDialog::buttonOkSelected()
{
    applyPreferences();

    savePreferences();

    hide();
}

void PreferencesDialog::savePreferences()
{
//  settings->insertSearchPath( QSettings::Unix, "openspeedshop" );
  // No search path needed for Unix; see notes further on.
//printf("********   PreferencesDialog::savePreferences() entered!\n");
//printf("settings=0x%x\n", settings);

  if( !settings->writeEntry( "/openspeedshop/general/globalFontFamily", globalFontFamily ) )
  {
    printf("Unable to write globalFontFamily.\n");
  }

  if( !settings->writeEntry("/openspeedshop/general/globalFontPointSize", globalFontPointSize ) )
  {
    printf("Unable to write globalFontPointSize.\n");
  }

  if( !settings->writeEntry("/openspeedshop/general/globalFontWeight", globalFontWeight ) )
  {
    printf("Unable to write globalFontWeight.\n");
  }

  if( !settings->writeEntry("/openspeedshop/general/globalFontItalic", globalFontItalic ) )
  {
    printf("Unable to write globalFontItalic.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/globalRemoteShell", globalRemoteShell ) )
  {
    printf("Unable to write globalRemoteShell.\n");
  }


  if( !settings->writeEntry( "/openspeedshop/general/showSplashScreen", setShowSplashScreenCheckBox->isChecked() ) )
  {
    printf("Unable to write showSplashScreen.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/showGraphics", showGraphicsCheckBox->isChecked() ) )
  {
    printf("Unable to write showGraphics.\n");
  }

  if( !settings->writeEntry("/openspeedshop/general/viewDateTimePrecision", viewDateTimePrecisionLineEdit->text() ) )
  {
    printf("Unable to write viewDateTimePrecision.\n");
  }

  if( !settings->writeEntry("/openspeedshop/general/viewFieldSize", viewFieldSizeLineEdit->text() ) )
  {
    printf("Unable to write viewFieldSize.\n");
  }

  if( !settings->writeEntry("/openspeedshop/general/viewPrecision", viewPrecisionLineEdit->text() ) )
  {
    printf("Unable to write viewPrecision.\n");
  }

  if( !settings->writeEntry("/openspeedshop/general/historyLimit", historyLimitLineEdit->text() ) )
  {
    printf("Unable to write historyLimit.\n");
  }

  if( !settings->writeEntry("/openspeedshop/general/historyDefault", historyDefaultLineEdit->text() ) )
  {
    printf("Unable to write historyDefault.\n");
  }

  if( !settings->writeEntry("/openspeedshop/general/maxAsyncCommands", maxAsyncCommandsLineEdit->text() ) )
  {
    printf("Unable to write maxAsyncCommands.\n");
  }

  if( !settings->writeEntry("/openspeedshop/general/helpLevelDefault", helpLevelDefaultLineEdit->text() ) )
  {
    printf("Unable to write helpLevelDefault.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/viewFieldSizeIsDynamic", viewFieldSizeIsDynamicCheckBox->isChecked() ) )
  {
    printf("Unable to write viewFieldSizeIsDynamic.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/autoCreateDerivedMetrics", autoCreateDerivedMetricsCheckBox->isChecked() ) )
  {
    printf("Unable to write autoCreateDerivedMetrics.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/saveViewsForReuse", saveViewsForReuseCheckBox->isChecked() ) )
  {
    printf("Unable to write saveViewsForReuse.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/saveViewsTime", saveViewsTimeLineEdit->text() ) )
  {
    printf("Unable to write saveViewsTime.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/viewFullPath", viewFullPathCheckBox->isChecked() ) )
  {
    printf("Unable to write viewFullPath.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/saveExperimentDatabase", saveExperimentDatabaseCheckBox->isChecked() ) )
  {
    printf("Unable to write saveExperimentDatabase.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/askAboutSavingTheDatabase", askAboutSavingTheDatabaseCheckBox->isChecked() ) )
  {
    printf("Unable to write askAboutSavingTheDatabase\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/askAboutChangingArgs", askAboutChangingArgsCheckBox->isChecked() ) )
  {
    printf("Unable to write askAboutChangingArg.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/onRerunSaveCopyOfExperimentDatabase", onRerunSaveCopyOfExperimentDatabaseCheckBox->isChecked() ) )
  {
    printf("Unable to write onRerunSaveCopyOfExperimentDatabase.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/viewMangledName", viewMangledNameCheckBox->isChecked() ) )
  {
    printf("Unable to write viewMangeledName.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/allowPythonCommands", allowPythonCommandsCheckBox->isChecked() ) )
  {
    printf("Unable to write allowPythonCommands.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/instrumentorIsOffline", instrumentorIsOfflineCheckBox->isChecked() ) )
  {
    printf("Unable to write instrumentorIsOffline.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/lessRestrictiveComparisons", lessRestrictiveComparisonsCheckBox->isChecked() ) )
  {
    printf("Unable to write lessRestrictiveComparisons.\n");
  }

  if( !settings->writeEntry( "/openspeedshop/general/viewBlankInPlaceOfZero", viewBlankInPlaceOfZeroCheckBox->isChecked() ) )
  {
    printf("Unable to write viewBlankInPlaceOfZero.\n");
  }

  // Begin save all preferences
  //This is the base plugin directory.   In this directory there should
  // be a list of dso (.so) which are the plugins.
  char plugin_file[1024];
  if( panelContainer->getMasterPC() && panelContainer->getMasterPC()->_pluginRegistryList )
  {
    // Insure the libltdl user-defined library search path has been set
    assert(lt_dlgetsearchpath() != NULL);
    // Now search for plugins in this path
    PluginInfo *pi = NULL;
    for( PluginRegistryList::Iterator it = panelContainer->getMasterPC()->_pluginRegistryList->begin();
         it != panelContainer->getMasterPC()->_pluginRegistryList->end();
         it++ )
    {
      pi = (PluginInfo *)*it;
      lt_dlhandle dl_object = lt_dlopenext((const char *)pi->plugin_name);
  
      if( dl_object != NULL )
      {
// printf("about to lookup(%s).\n", "save_preferences_entry_point");
        void (*dl_plugin_info_init_preferences_routine)(QSettings *, char *) =
          (void (*)(QSettings *, char *))lt_dlsym(dl_object, "save_preferences_entry_point" );
         if( dl_plugin_info_init_preferences_routine )
         {
// printf("about to call the routine.\n");
           (*dl_plugin_info_init_preferences_routine)(settings, pi->preference_category);
         }
         lt_dlclose(dl_object);
      }
    }
  }
  settings->sync();
  // End save all preferences

//   printf("FINISHED WRITING!\n");
}
