#ifndef PROCESS_CONTROL_OBJECT_H
#define PROCESS_CONTROL_OBJECT_H

#include "AnimatedQPushButton.hxx"
#include <qpushbutton.h>

#include <qlayout.h>
#include <qhbox.h>

class QLabel;
class QLineEdit;
class QButtonGroup;
class QHBoxLayout;

class ProcessControlObject : public QWidget
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
  //! ProcessControlObject() - A default constructor the the Panel Class.
  ProcessControlObject(QVBoxLayout *, QWidget *);  // Default construct

  //! ~ProcessControlObject() - The default destructor.
  ~ProcessControlObject();  // Active destructor

  QButtonGroup *buttonGroup;
  AnimatedQPushButton *attachCollectorButton;
  AnimatedQPushButton *detachCollectorButton;
  AnimatedQPushButton *attachProcessButton;
  AnimatedQPushButton *detachProcessButton;
  AnimatedQPushButton *runButton;
  AnimatedQPushButton *pauseButton;
  AnimatedQPushButton *continueButton;
  AnimatedQPushButton *updateButton;
  AnimatedQPushButton *interruptButton;
  AnimatedQPushButton *terminateButton;

public slots:
  virtual void attachCollectorButtonSlot();
  virtual void detachCollectorButtonSlot();
  virtual void attachProcessButtonSlot();
  virtual void detachProcessButtonSlot();
  virtual void runButtonSlot();
  virtual void pauseButtonSlot();
  virtual void continueButtonSlot();
  virtual void updateButtonSlot();
  virtual void interruptButtonSlot();
  virtual void terminateButtonSlot();

protected slots:
  virtual void languageChange();

protected:

private:
};
#endif // PROCESS_CONTROL_OBJECT_H
