#ifndef TABBARWIDGET_H
#define TABBARWIDGET_H

#include <qwidget.h>
#include <qtabbar.h>

//! Class  TabBarWidget

class PanelContainer;

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

    //! A pointer to the parent panelContainer.
    PanelContainer *panelContainer;

  protected slots:
    //! The standard Qt slot to change language information.
    virtual void languageChange();

  private:
    //! Fields the mousePressEvent for drag and drop.
    void mousePressEvent(QMouseEvent *e=NULL);
};
#endif // TABBARWIDGET_H
