////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


#ifndef PROCESS_CONTROL_OBJECT_H
#define PROCESS_CONTROL_OBJECT_H

#include <cstddef>
#include "AnimatedQPushButton.hxx"
#include "Panel.hxx"
#include <qpushbutton.h>

#include <qlayout.h>
#include <qhbox.h>
#include <qtimer.h>

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
  AnimatedQPushButton *runButton;
  AnimatedQPushButton *continueButton;
  AnimatedQPushButton *pauseButton;
  AnimatedQPushButton *updateButton;
  AnimatedQPushButton *interruptButton;
  AnimatedQPushButton *terminateButton;

  Panel *panel;

  QString argtext;
public slots:
  virtual void runButtonSlot();
  virtual void runButtonPressedSlot();
  virtual void continueButtonSlot();
  virtual void continueButtonPressedSlot();
  virtual void pauseButtonSlot();
  virtual void updateButtonSlot();
  virtual void interruptButtonSlot();
  virtual void terminateButtonSlot();

protected slots:
  virtual void languageChange();
  void buttonTimerSlot();

protected:

private:
  bool menuFieldedFLAG;
  QTimer *buttonTimer;

};
#endif // PROCESS_CONTROL_OBJECT_H
