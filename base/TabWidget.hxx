#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <qwidget.h>
#include <qtabwidget.h>

class AnimatedQPushButton;
class PanelContainer;

//! This overloads QTabWidget so the tabbar can accept mouse down events.
class TabWidget : public QTabWidget
{
    Q_OBJECT
  public:
    //! default constructor.
    TabWidget( );

    //! Constructor to overload QTabWiget
    TabWidget( PanelContainer *pc, QWidget *parent=0, const char *name=0 );

    //! The default destructor
    ~TabWidget( );

    //! Sets the tabbar
    void setTabBar( QTabBar *tb );

    //! Gets the tabbar.
    QTabBar *tabBar();

  protected slots:
    //! The standard Qt slot to change language information.
    virtual void languageChange();

  private slots:
    void deletePanelButtonSelected();

  private:
    AnimatedQPushButton *cw;

    PanelContainer *_panelContainer;

};
#endif // TABWIDGET_H
