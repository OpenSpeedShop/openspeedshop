INCLUDE_DIR = \
    -I ../.. \
    -I ../../../base \
    -I ../../../message \
    -I ../../../libopenss-framework \
    -I ../../../objects \
    -I /usr/include/python2.3

CXXFLAGS = -g
CFLAGS = -g

CC = cc  
C++ = g++

LIB =  /home/jcarter/workarea/bits/lib/libopenss-cli.so

######################################################

DSO_SRCS = \
    script_test.cxx

DSO = script_test.so

######################################################

all : ${DSO}

######################################################

$(DSO): ${DSO_SRCS} ${DSO_HDRS}
	${C++} ${CXXFLAGS} -shared -o $(DSO) ${DSO_SRCS} \
	${INCLUDE_DIR} ${LIB}

######################################################

clobber:
	rm -f \
	${DSO_OBJS} \
	$(DSO) \
	core*

