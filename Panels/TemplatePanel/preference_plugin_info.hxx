#ifndef PREFERENCE_PLUGIN_INFO_H
#define PREFERENCE_PLUGIN_INFO_H

#include <qwidgetstack.h>
#include <qsettings.h>


extern "C"
{
  QWidget *initialize_preferences_entry_point(QSetttings *settings, QWidgetStack *stack, char *name)
  {
    printf("initialize_preferences_entry_point(0x%x 0x%x %s) entered\n", settings, tack, name);

    // Add your preferences to initialize here.   See Source Panel for 
    // an example.
    return( NULL );
  }
  void save_preferences_entry_point(QSettings *settings, char *name)
  {
    printf("save_preferences_entry_point() entered\n");
    // Add your preferences to save here.   See Source Panel for 
    // an example.
  }
}

#endif // PREFERENCE_PLUGIN_INFO_H
