ROOT?=/home/amaury/futuretool/ROOT


PYTHON=python2.3
PYTHON_INCLUDE=-I$(ROOT)/include/$(PYTHON)
PYTHON_LIBS= -L$(ROOT)/lib -l$(PYTHON)

BOOST=boost-1_31
BOOST_INCLUDE=-I$(ROOT)/include/$(BOOST)
BOOST_LIBS=-L$(ROOT)/lib  -lboost_python-gcc

GCXX=g++

INCS= $(BOOST_INCLUDE) $(PYTHON_INCLUDE) -I../include
LIBS= $(BOOST_LIBS) $(PYTHON_LIBS)

GCXX_FLAGS=-fPIC -g

LINKFORSHARED=-Xlinker -export-dynamic

tcli: tcli.cxx
	$(GCXX) $(GCXX_FLAGS) -o $@ $(INCS) tcli.cxx $(LIBS) $(LINKFORSHARED)

