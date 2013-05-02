#BHEADER***********************************************************************
# (c) 2000   The Regents of the University of California
#
# See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
# notice, contact person, and disclaimer.
#
#EHEADER***********************************************************************

noinst_PROGRAMS = smg2000

smgtopdir = ../.

smg2000_CFLAGS = \
	-I$(smgtopdir)\
	-I$(smgtopdir)/utilities\
	-I$(smgtopdir)/struct_mv\
	-I$(smgtopdir)/struct_ls\
	-I$(smgtopdir)/krylov\
        -I -g -gdwarf-2 -DHYPRE_USING_OPENMP -DHYPRE_SEQUENTIAL -fno-default-inline -fopenmp \
	-DHYPRE_TIMING

smg2000_LDFLAGS = \
        -lm \
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
