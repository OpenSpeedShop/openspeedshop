#ifndef SLOTINFO_H
#define SLOTINFO_H


#include <unistd.h>
#include <qobject.h>

class PluginInfo;

//! SlotInfo Class
class SlotInfo : public QObject
{
  Q_OBJECT
public:
    //! SlotInfo() -  A default constructor.
    SlotInfo();

    //! SlotInfo(QObject *parent, const char *name, PluginInfo *pi)
    SlotInfo(QObject *parent, const char *name, PluginInfo *pi);

    //! The destructor for this object.
    ~SlotInfo();

    //! A pointer to the PluginInfo. Which is all the information about the plugin loaded when the plugin was initially opened.
    PluginInfo *pluginInfo;

public slots:
    //! The call to do the menu work.
    void dynamicMenuCallback();
signals:
};

#endif // SLOTINFO_H
