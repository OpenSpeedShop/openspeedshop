#ifndef SETDATAFORM_H
#define SETDATAFORM_H

#include "element.hxx"

#include <qdialog.h>

class QHBoxLayout;
class QPushButton;
class QTable;
class QVBoxLayout;


class SetDataForm: public QDialog
{
    Q_OBJECT
public:
    SetDataForm( ElementVector *elements, int decimalPlaces,
		 QWidget *parent = 0, const char *name = "set data form",
		 bool modal = TRUE, WFlags f = 0 );
    ~SetDataForm() {}

public slots:
    void setColor();
    void setColor( int row, int col );
    void currentChanged( int row, int col );
    void valueChanged( int row, int col );

protected slots:
    void accept();

private:
    QTable *table;
    QPushButton *colorPushButton;
    QPushButton *okPushButton;
    QPushButton *cancelPushButton;

protected:
    QVBoxLayout *tableButtonBox;
    QHBoxLayout *buttonBox;

private:
    ElementVector *m_elements;
    int m_decimalPlaces;
};

#endif
