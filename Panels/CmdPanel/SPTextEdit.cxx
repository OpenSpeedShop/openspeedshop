/*! \class SPTextEdit
  This is simple a reimplementation of the QTextEdit widget so we can
  popup a context sensitive menu.
*/
#include "SPTextEdit.hxx"

#include "CmdPanel.hxx"

#include "debug.hxx"

#include <stdlib.h>  // for the free() call below.

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
SPTextEdit::SPTextEdit( CmdPanel *cp, QWidget *parent, const char *n )
    : QTextEdit( parent, n )
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "SPTextEdit::SPTextEdit( ) constructor called\n");
  cmdPanel = cp;
}

/*! The default destructor. */
SPTextEdit::~SPTextEdit( )
{
  // default destructor.
  nprintf(DEBUG_CONST_DESTRUCT) ("  SPTextEdit::~SPTextEdit( ) destructor called\n");
}

void
SPTextEdit::contentsMouseMoveEvent( QMouseEvent *e )
{
  nprintf(DEBUG_PANELS) ("Hello from down under the hood.\n");
}
