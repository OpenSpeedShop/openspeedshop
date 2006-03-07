INCLUDE_DIR = \
    -I ../../libopenss-cli \
    -I ../../libopenss-guibase \
    -I ../../libopenss-message \
    -I ../../libopenss-framework \
    -I ../../libopenss-queries \
    -I ../../libopenss-guiobjects \
    -I /usr/include/python2.3

CXXFLAGS = -g
CFLAGS = -g

CC = cc  
C++ = g++

LIB = ${OPENSS_PLUGIN_PATH}/../libopenss-cli.so

######################################################

DSO_SRCS = \
    script_data.cxx \
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

