#BHEADER***********************************************************************
# (c) 2000   The Regents of the University of California
#
# See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
# notice, contact person, and disclaimer.
#
#EHEADER***********************************************************************

.SUFFIXES:
.SUFFIXES: .c .o

srcdir = .

include ../Makefile.include

CFLAGS = \
 -I..\
 -I../utilities\
 -I../struct_mv\
 -I../struct_ls\
 -I../krylov\
 ${INCLUDE_CFLAGS}\
 -DHYPRE_TIMING
 
LFLAGS =\
 -L.\
 -L../struct_ls\
 -L../struct_mv\
 -L../krylov\
 -L../utilities\
 -lHYPRE_struct_ls\
 -lHYPRE_struct_mv\
 -lkrylov\
 -lHYPRE_utilities\
 ${INCLUDE_LFLAGS}

##################################################################
# Targets
##################################################################

all: smg2000

clean:
	@rm -f *.o

veryclean: clean
	@rm -f smg2000

##################################################################
# Rules
##################################################################

smg2000: smg2000.o
	@echo  "Linking" $@ "... "
	${CC} -o smg2000 smg2000.o ${LFLAGS}

##################################################################
# Generic rules
##################################################################

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<


