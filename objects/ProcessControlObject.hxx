#ifndef PROCESS_CONTROL_OBJECT_H
#define PROCESS_CONTROL_OBJECT_H

#include "AnimatedQPushButton.hxx"
#include "Panel.hxx"
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
  ProcessControlObject(QVBoxLayout *, QWidget *, Panel *);  // Default construct

  //! ~ProcessControlObject() - The default destructor.
  ~ProcessControlObject();  // Active destructor

  QButtonGroup *buttonGroup;
#ifdef OLDWAY
  AnimatedQPushButton *attachCollectorButton;
  AnimatedQPushButton *detachCollectorButton;
  AnimatedQPushButton *detachProcessButton;
  AnimatedQPushButton *attachProcessButton;
#endif // OLDWAY
  AnimatedQPushButton *runButton;
  AnimatedQPushButton *pauseButton;
  AnimatedQPushButton *continueButton;
  AnimatedQPushButton *updateButton;
  AnimatedQPushButton *interruptButton;
  AnimatedQPushButton *terminateButton;

  Panel *panel;

#ifdef OLDWAY  // Move this back into "public slots:" if needed.
  virtual void attachCollectorButtonSlot();
  virtual void attachProcessButtonSlot();
  virtual void detachCollectorButtonSlot();
  virtual void detachProcessButtonSlot();
#endif // OLDWAY
public slots:
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
