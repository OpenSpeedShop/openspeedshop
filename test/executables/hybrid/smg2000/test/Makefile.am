#BHEADER***********************************************************************
# (c) 2000   The Regents of the University of California
#
# See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
# notice, contact person, and disclaimer.
#
#EHEADER***********************************************************************

noinst_PROGRAMS = smg2000

MPI_IMPL = @DEFAULT_MPI_IMPL@

MPICC	= $($(MPI_IMPL)_CC)
CC	= $(MPICC)

smgtopdir = ../.


smg2000_CFLAGS = \
	$($(MPI_IMPL)_CPPFLAGS) \
	-I$(smgtopdir)\
	-I$(smgtopdir)/utilities\
	-I$(smgtopdir)/struct_mv\
	-I$(smgtopdir)/struct_ls\
	-I$(smgtopdir)/krylov\
	-I -g -gdwarf-2 -O1 -DTIMER_USE_MPI -fopenmp -DHYPRE_USING_OPENMP\
	-DHYPRE_TIMING

smg2000_LDFLAGS = \
        -lm $($(MPI_IMPL)_LDFLAGS) $($(MPI_IMPL)_LIBS) \
	-L.\
	-L$(smgtopdir)/struct_ls\
	-L$(smgtopdir)/struct_mv\
	-L$(smgtopdir)/krylov\
	-L$(smgtopdir)/utilities

smg2000_LDADD = \
	-lHYPRE_struct_ls\
	-lHYPRE_struct_mv\
	-lkrylov\
	-lHYPRE_utilities 
