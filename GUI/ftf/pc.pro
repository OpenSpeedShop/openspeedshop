unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}








































































TEMPLATE	= lib
LANGUAGE	= C++

CONFIG	+= qt warn_on release shared

LIBS	+= -rdynamic
INCLUDEPATH	+= ../base/ ../plugin/

SOURCES	+= main.cpp
FORMS	= performanceleader.ui
IMAGES	= images/editcopy \
	images/editcut \
	images/editpaste \
	images/filenew \
	images/fileopen \
	images/filesave \
	images/print \
	images/redo \
	images/searchfind \
	images/undo \
	images/filenew_1 \
	images/fileopen_1 \
	images/filesave_1 \
	images/print_1 \
	images/undo_1 \
	images/redo_1 \
	images/editcut_1 \
	images/editcopy_1 \
	images/editpaste_1 \
	images/searchfind_1

