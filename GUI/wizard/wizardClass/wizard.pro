SOURCES	+= main.cpp
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
FORMS	= wizardform.ui
TEMPLATE	=lib
CONFIG	+= qt warn_on release
INCLUDEPATH	+= ../../base ../../panels .. ../..
LANGUAGE	= C++
