

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}






























TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release


SOURCES	+= main.cpp
FORMS	= mydialog1.ui
IMAGES	= images/attach_hand.png \
	images/rightarrow.png \
	images/leftarrow.png

