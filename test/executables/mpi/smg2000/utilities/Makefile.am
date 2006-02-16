#BHEADER***********************************************************************
# (c) 2000   The Regents of the University of California
#
# See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
# notice, contact person, and disclaimer.
#
#EHEADER***********************************************************************

lib_LIBRARIES = libHYPRE_utilities.a

smgtopdir = ../.

libHYPRE_utilities_a_CFLAGS = \
	$(MPI_CPPFLAGS) \
	-I$(smgtopdir)\
	-I -g -gdwarf-2 -O -DTIMER_USE_MPI \
	-DHYPRE_TIMING

libHYPRE_utilities_a_LDFLAGS = \
        -lm $(MPI_LDFLAGS) $(MPI_LIBS)

libHYPRE_utilities_a_SOURCES = \
	memory.h\
	mpistubs.h\
	threading.h\
	timing.h\
	utilities.h\
	HYPRE_utilities.h\
	memory.c\
	mpistubs.c\
	threading.c\
	timer.c\
	timing.c\
	random.c

timer.o: timer.c
	${CC} -D_POSIX_SOURCE -o $@ -c ${CFLAGS} $<

