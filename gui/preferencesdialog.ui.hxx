#include <qsettings.h>
#include <qapplication.h>

#include "openspeedshop.hxx"
#include "PluginInfo.hxx"
#include <dlfcn.h>

void PreferencesDialog::readPreferencesOnEntry()
{
// printf("readPreferencesOnEntry() entered\n");

//  settings.insertSearchPath( QSettings::Unix, "openspeedshop" );
  // No search path needed for Unix; see notes further on.
  QString ds = QString::null;
  bool ok;

  settings.readEntry( "/openspeedshop/general/globalFontFamily", ds, &ok);
  if( ok == FALSE )
  {
    printf("Didn't find any defaults file.  Just take the defaults.\n");
    return;
  }

  preferencesAvailable = TRUE;

  globalFontFamily =
    settings.readEntry( "/openspeedshop/general/globalFontFamily" );
  fontLineEdit->setText( globalFontFamily );

  globalFontPointSize =
    settings.readNumEntry("/openspeedshop/general/globalFontPointSize");
  globalFontWeight =
    settings.readNumEntry("/openspeedshop/general/globalFontWeight");
  globalFontItalic =
     settings.readBoolEntry("/openspeedshop/general/globalFontItalic");



  precisionLineEdit->setText(
    settings.readEntry( "/openspeedshop/general/globalPrecision") );

  setShowSplashScreenCheckBox->setChecked(
    settings.readBoolEntry( "/openspeedshop/general/showSplashScreen") );
  setShowColoredTabsCheckBox->setChecked(settings.readBoolEntry( "/openspeedshop/general/showColoredTabs") );
  deleteEmptyPCCheckBox->setChecked(
    settings.readBoolEntry( "/openspeedshop/general/deleteEmptyPC") );
  showGraphicsCheckBox->setChecked(
    settings.readBoolEntry( "/openspeedshop/general/showGraphics") );
}

void PreferencesDialog::resetPreferenceDefaults()
{
    qWarning( "PreferencesDialog::resetPreferenceDefaults(): Not implemented yet" );
  languageChange();
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


  // NOTIFY EVERYONE THAT PREFERENCES HAVE CHANGED!
  printf("NOTIFY EVERYONE THAT PREFERENCES HAVE CHANGED!\n");
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
//  settings.insertSearchPath( QSettings::Unix, "openspeedshop" );
  // No search path needed for Unix; see notes further on.

  settings.writeEntry( "/openspeedshop/general/globalFontFamily", globalFontFamily );

  settings.writeEntry("/openspeedshop/general/globalFontPointSize", globalFontPointSize );
  settings.writeEntry("/openspeedshop/general/globalFontWeight", globalFontWeight );
  settings.writeEntry("/openspeedshop/general/globalFontItalic", globalFontItalic );


  settings.writeEntry( "/openspeedshop/general/globalPrecision", precisionLineEdit->text() );
  settings.writeEntry( "/openspeedshop/general/showSplashScreen", setShowSplashScreenCheckBox->isChecked() );
  settings.writeEntry( "/openspeedshop/general/showColoredTabs", setShowColoredTabsCheckBox->isChecked() );
  settings.writeEntry( "/openspeedshop/general/deleteEmptyPC", deleteEmptyPCCheckBox->isChecked() );
  settings.writeEntry( "/openspeedshop/general/showGraphics", showGraphicsCheckBox->isChecked() );

  // Begin save all preferences
  //This is the base plugin directory.   In this directory there should
  // be a list of dso (.so) which are the plugins.
  char plugin_file[1024];
  if( panelContainer->getMasterPC() && panelContainer->getMasterPC()->_pluginRegistryList )
  {
    char *plugin_directory = getenv("OPENSPEEDSHOP_PLUGIN_PATH");
    if( !plugin_directory )
    {
      fprintf(stderr, "Can't find the PanelContainer plugin. $OPENSPEEDSHOP_PLUGIN_PATH not set correctly.\n");
        return;
    }
    PluginInfo *pi = NULL;
    for( PluginRegistryList::Iterator it = panelContainer->getMasterPC()->_pluginRegistryList->begin();
         it != panelContainer->getMasterPC()->_pluginRegistryList->end();
         it++ )
    {
      pi = (PluginInfo *)*it;
      sprintf(plugin_file, "%s/%s", plugin_directory, pi->plugin_name );
      void *dl_object = dlopen((const char *)plugin_file, (int)RTLD_LAZY );
  
      if( dl_object )
      {
// printf("about to lookup(%s).\n", "save_preferences_entry_point");
        void (*dl_plugin_info_init_preferences_routine)(QSettings *, char *) =
          (void (*)(QSettings *, char *))dlsym(dl_object, "save_preferences_entry_point" );
         if( dl_plugin_info_init_preferences_routine )
         {
// printf("about to call the routine.\n");
           (*dl_plugin_info_init_preferences_routine)(&settings, pi->preference_category);
         }
         dlclose(dl_object);
      }
    }
  }
  // End save all preferences
}
