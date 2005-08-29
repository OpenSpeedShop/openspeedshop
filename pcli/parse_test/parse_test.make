INCLUDE_DIR = \
    -I .. \
    -I ../../base \
    -I ../../message \
    -I ../../libopenss-framework \
    -I ../../objects

CXXFLAGS = -g
CFLAGS = -g

CC = cc  
C++ = g++

#LIB =  /home/jcarter/workarea/bits/lib/libopenss-cli.so
LIB = ${OPENSS_PLUGIN_PATH}/../libopenss-cli.so

######################################################

PARSER_SRCS = \
    test_main.cxx

PARSER = parser

######################################################

INPUT_SRCS = \
    input.cxx

INPUT = gen_input

######################################################

all : ${PARSER} ${INPUT}

######################################################

$(PARSER): ${PARSER_SRCS} ${PARSER_HDRS}
	${C++} ${PARSER_SRCS} \
	${LIB} \
	${CXXFLAGS} -o $(PARSER) \
	${INCLUDE_DIR} 

######################################################

$(INPUT): ${INPUT_SRCS} 
	${C++} ${CXXFLAGS} -o $(INPUT) \
	${INPUT_SRCS} 

######################################################

clobber:
	rm -f \
	$(PARSER) \
	$(INPUT) \
	core*

