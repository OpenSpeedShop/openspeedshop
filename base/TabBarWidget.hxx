#ifndef TABBARWIDGET_H
#define TABBARWIDGET_H

#include <qwidget.h>
#include <qtabbar.h>

class PanelContainer;

//! Manages the tab bar events.
class TabBarWidget : public QTabBar
{
    Q_OBJECT
  public:
    //! default constructor.   Here for completeness only. (unused)
    TabBarWidget( );

    //! This is the work constructor.
    TabBarWidget( PanelContainer *pc, QWidget *parent=0, const char *name=0 );

    //! The default destructor.
    ~TabBarWidget( );

    //! Function to return the panelContainer pointer.
    PanelContainer *getPanelContainer() { return _panelContainer; };

    //! Function to set the panelContainer pointer.
    void setPanelContainer(PanelContainer *pc)  { _panelContainer = pc; }
  protected slots:
    //! The standard Qt slot to change language information.
    virtual void languageChange();

  private:
    //! Fields the mousePressEvent for drag and drop.
    void mousePressEvent(QMouseEvent *e=NULL);

    //! A pointer to the parent panelContainer.
    PanelContainer *_panelContainer;
};
#endif // TABBARWIDGET_H
