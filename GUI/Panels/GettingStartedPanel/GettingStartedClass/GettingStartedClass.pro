SOURCES	+= main.cpp
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
FORMS	= gettingstartedclass.ui
TEMPLATE	=lib
CONFIG	+= qt warn_on release shared
INCLUDEPATH	+= .. ../../../base
LANGUAGE	= C++
