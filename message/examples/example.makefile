
CXXFLAGS = -g
CFLAGS = -g
LFLAGS = -L .. -lmessage

C++ = g++ \
    -LANG:std \
    -I .. \
    -I /usr/include 

SRCS = \
    example1.cxx \
    example2.cxx

OBJS = \
    example1.o \
    example2.o 

PROG= \
    example1 \
    example2

MESSAGE=../libmessage.so

all : ${PROG} 

example1: example1.cxx
	${C++} -o example1 \
	example1.cxx \
	${LFLAGS}

example2: example2.cxx
	${C++} -o example2 \
	example2.cxx \
	${LFLAGS}


run:
	./runit.sh

clobber:
	rm -f \
	${OBJS} \
	$(PROG) \
	core
	rm -rf ii_files

