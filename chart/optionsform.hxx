#ifndef OPTIONSFORM_H
#define OPTIONSFORM_H

#include <qdialog.h>

class QButtonGroup;
class QComboBox;
class QFrame;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QVBoxLayout;


class OptionsForm : public QDialog
{
    Q_OBJECT
public:
    OptionsForm( QWidget* parent = 0, const char* name = "options form",
		 bool modal = FALSE, WFlags f = 0 );
    ~OptionsForm() {}

    QFont font() const { return m_font; }
    void setFont( QFont font );

    QLabel *chartTypeTextLabel;
    QComboBox *chartTypeComboBox;
    QPushButton *fontPushButton;
    QLabel *fontTextLabel;
    QFrame *addValuesFrame;
    QButtonGroup *addValuesButtonGroup;
    QRadioButton *noRadioButton;
    QRadioButton *yesRadioButton;
    QRadioButton *asPercentageRadioButton;
    QLabel *decimalPlacesTextLabel;
    QSpinBox *decimalPlacesSpinBox;
    QPushButton *okPushButton;
    QPushButton *cancelPushButton;

protected slots:
    void chooseFont();

protected:
    QVBoxLayout *optionsFormLayout;
    QHBoxLayout *chartTypeLayout;
    QHBoxLayout *fontLayout;
    QVBoxLayout *addValuesFrameLayout;
    QVBoxLayout *addValuesButtonGroupLayout;
    QHBoxLayout *decimalPlacesLayout;
    QHBoxLayout *buttonsLayout;

private:
    QFont m_font;
};

#endif

