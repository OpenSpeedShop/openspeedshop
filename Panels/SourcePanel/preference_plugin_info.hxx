#ifndef PREFERENCE_PLUGIN_INFO_H
#define PREFERENCE_PLUGIN_INFO_H
extern "C"
{
  int preference_info_init(QWidgetStack *stack, char *name)
  {
    printf("preference_info_init(0x%x %s) entered\n", stack, name);
    return 1;
  }
}

#endif // PREFERENCE_PLUGIN_INFO_H
