#BHEADER***********************************************************************
# (c) 2000   The Regents of the University of California
#
# See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
# notice, contact person, and disclaimer.
#
#EHEADER***********************************************************************

.SUFFIXES:
.SUFFIXES: .c .f .o

srcdir = .

include ../Makefile.include

CFLAGS =\
 -I..\
 -I../utilities\
 ${INCLUDE_CFLAGS}

RANLIB = ranlib

HEADERS =\
 krylov.h

FILES =\
 pcg.c\
 HYPRE_pcg.c

OBJS = ${FILES:.c=.o}

##################################################################
# Targets
##################################################################

all: libkrylov.a 

clean:
	@rm -f *.o *.dxx 

veryclean: clean
	@rm -f lib*.a


##################################################################
# Rules
##################################################################

libkrylov.a: ${OBJS}
	@echo  "Building $@ ... "
	ar -rcu $@ ${OBJS}
	${RANLIB} $@

${OBJS}: ${HEADERS}



##################################################################
# Generic rules
##################################################################

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

