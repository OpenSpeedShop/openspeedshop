/*! \class PreferencesChangedObject
 */
#include "PreferencesChangedObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"

/*! Constructor for the PreferencesChangedObject. 
 */
PreferencesChangedObject::PreferencesChangedObject() : MessageObject("PreferencesChangedObject")
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("PreferencesChangedObject::PreferencesChangedObject(entered.\n");
}

/*! Destructor */
PreferencesChangedObject::~PreferencesChangedObject()
{
}
