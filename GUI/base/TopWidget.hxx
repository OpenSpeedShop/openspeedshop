#ifndef TOPWIDGET_H
#define TOPWIDGET_H

class PanelContainer;

#include <qwidget.h>
#include <qtabwidget.h>

class TopWidget : public QWidget
{
    Q_OBJECT
public:
    TopWidget( );

    TopWidget( QWidget *parent=0, const char *name=0, WFlags f = 0 );

    ~TopWidget( );

    void setPanelContainer(PanelContainer *pc) { panelContainer = pc; };
  
    void closeEvent( QCloseEvent *e );

  public slots:


  protected slots:
    virtual void languageChange();

private:
    PanelContainer *panelContainer;

    char *name;

};
#endif // TOPWIDGET_H
