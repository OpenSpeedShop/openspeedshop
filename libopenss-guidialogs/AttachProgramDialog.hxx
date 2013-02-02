////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.
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
  

#ifndef ATTACHPROGRAMDIALOG_H
#define ATTACHPROGRAMDIALOG_H

#include <stddef.h>
#include <qcheckbox.h>
#include <qvariant.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qlineedit.h>

#include "ProcessListObject.hxx"  // For getting pid list off a host...
#include "ProcessEntryClass.hxx"

class QSettings;

class AttachProgramDialog : public QFileDialog
{
    Q_OBJECT

public:
//    AttachProgramDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    AttachProgramDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE);
    ~AttachProgramDialog();

  QLabel *label;
  QLineEdit *lineedit;
  QLabel *parallelPrefixLabel;
  QLineEdit *parallelPrefixLineedit;
  QLabel *pCBlabel;
  QLabel *psaveCBlabel;

  bool reuseParallelCommand() { return useParallelCommand;};
  void shouldWeReuseParallelCommand(bool flag) { useParallelCommand = flag;};

  QCheckBox *parallelCB;
  QCheckBox *parallelSaveCB;

  QSettings *guiSettings;
  QString usersParallelPrefixCommand;

protected:
  bool useParallelCommand;

protected slots:
    virtual void languageChange();
    virtual void accept();

public slots:
  void parallelCBSelected();
  void parallelSaveCBSelected();

};

#endif // ATTACHPROGRAMDIALOG_H
