#ifndef PREFERENCE_PLUGIN_INFO_H
#define PREFERENCE_PLUGIN_INFO_H

extern "C"
{
  QWidget *initialize_preferences_entry_point(QWidgetStack *stack, char *name)
  {
    printf("initialize_preferences_entry_point(0x%x %s) entered\n", stack, name);

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
