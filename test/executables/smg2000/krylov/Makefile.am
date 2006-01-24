#BHEADER***********************************************************************
# (c) 2000   The Regents of the University of California
#
# See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
# notice, contact person, and disclaimer.
#
#EHEADER***********************************************************************

lib_LIBRARIES = libkrylov.a

smgtopdir = ../.


libkrylov_a_CFLAGS =\
	$(MPI_CPPFLAGS) \
	-I$(smgtopdir)\
	-I$(smgtopdir)/utilities\
	-I -g -gdwarf-2 -O -DTIMER_USE_MPI 

libkrylov_a_LDFLAGS = \
        -lm $(MPI_LDFLAGS) $(MPI_LIBS)

libkrylov_a_SOURCES =\
	krylov.h \
	pcg.c \
	HYPRE_pcg.c

