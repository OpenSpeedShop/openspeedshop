#ifndef ATTACHPROCESSDIALOG_H
#define ATTACHPROCESSDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

#include "ProcessListObject.hxx"  // For getting pid list off a host...
#include "ProcessEntryClass.hxx"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QPushButton;
class QLabel;
class QComboBox;
class QListView;
class QListViewItem;

class AttachProcessDialog : public QDialog
{
    Q_OBJECT

public:
    AttachProcessDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~AttachProcessDialog();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

    QLabel* attachHostLabel;
    QComboBox * attachHostComboBox;
    QListView* availableProcessListView;

    QString selectedProcesses();
    void updateAttachableProcessList();

    ProcessListObject *plo;

protected:
    QVBoxLayout* AttachProcessDialogLayout;
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;

protected slots:
    virtual void languageChange();

public slots:
    virtual void attachHostComboBoxActivated();

};

#endif // ATTACHPROCESSDIALOG_H
