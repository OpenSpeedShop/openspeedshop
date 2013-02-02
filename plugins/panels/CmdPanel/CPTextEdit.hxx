////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 The Krell Institute All Rights Reserved.
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


#ifndef CPTEXTEDIT_H
#define CPTEXTEDIT_H

class CmdPanel;

#include <stddef.h>
#include <qwidget.h>
#include <qtextedit.h>
#include <qscrollbar.h>
#include <qpixmap.h>

//! Simply overloads the QTextEdit so we can detect the context menu event.
class CPTextEdit : public QTextEdit
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
    CPTextEdit( );

    //! CPTextEdit(CmdPanel *cp, QWidget *parent, const char *name)
    CPTextEdit( CmdPanel *cp, QWidget *parent=0, const char *name=0 );

    ~CPTextEdit( );

    QPopupMenu *createPopupMenu(const QPoint & pos);
protected: 
public slots:
protected slots:
    void textChanged ();
    void selectionChanged ();
    void copyAvailable ( bool yes );
    void cursorPositionChanged ( int para, int pos );
    void returnPressed ();
    void modificationChanged ( bool m );
    void clicked ( int para, int pos );
    void doubleClicked ( int para, int pos );

    void copy ();
    void cut ();
    void paste ();

private:
    CmdPanel *cmdPanel;
};
#endif // CPTEXTEDIT_H
