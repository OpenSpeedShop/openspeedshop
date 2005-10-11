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

GCXX_FLAGS=-fPIC -g -w


LINKFORSHARED=-Xlinker -export-dynamic

all: pySS.so pytst
pyAPI.o: pyAPI.cxx
	$(GCXX) -c $(GCXX_FLAGS) $(INCS) pyAPI.cxx 

pySS.so: pyAPI.cxx pySS.cxx
	$(GCXX) $(GCXX_FLAGS) $(INCS) pySS.cxx pyAPI.o $(LIBS) -shared -o $@

pytst.o: pytst.cxx 
	$(GCXX) $(GCXX_FLAGS) $(INCS) pytst.cxx -c

pytst: pytst.o pyAPI.o
	$(GCXX) -o $@ $(LIBS) $(LINKFORSHARED) pytst.o pyAPI.o





