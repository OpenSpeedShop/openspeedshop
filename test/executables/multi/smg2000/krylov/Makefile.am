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
	-I$(smgtopdir)\
	-I$(smgtopdir)/utilities\
        -I -g -gdwarf-2 -DHYPRE_USING_OPENMP -DHYPRE_SEQUENTIAL  -fno-default-inline

libkrylov_a_LDFLAGS = \
        -lm 

libkrylov_a_SOURCES =\
	krylov.h \
	pcg.c \
	HYPRE_pcg.c

