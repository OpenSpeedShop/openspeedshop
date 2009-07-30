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
 ${INCLUDE_CFLAGS}\
 -DHYPRE_TIMING

RANLIB= ranlib

##################################################################
# Targets
##################################################################

all: libHYPRE_utilities.a

clean:
	@rm -f *.o

veryclean: clean
	@rm -f libHYPRE_*.a

##################################################################
# Utilities rules
##################################################################
 
UTILITIES_HEADERS =\
 memory.h\
 mpistubs.h\
 threading.h\
 timing.h\
 utilities.h\
 HYPRE_utilities.h

UTILITIES_FILES =\
 memory.c\
 mpistubs.c\
 threading.c\
 timer.c\
 timing.c\
 random.c

UTILITIES_OBJS = ${UTILITIES_FILES:.c=.o}
 
libHYPRE_utilities.a: ${UTILITIES_OBJS}
	@echo  "Building $@ ... "
	ar -ru $@ ${UTILITIES_OBJS}
	${RANLIB} $@

${UTILITIES_OBJS}: ${UTILITIES_HEADERS}

timer.o: timer.c
	${CC} -D_POSIX_SOURCE -o $@ -c ${CFLAGS} $<

##################################################################
# Generic rules
##################################################################

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

