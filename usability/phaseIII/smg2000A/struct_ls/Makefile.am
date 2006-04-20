#BHEADER***********************************************************************
# (c) 2000   The Regents of the University of California
#
# See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
# notice, contact person, and disclaimer.
#
#EHEADER***********************************************************************

lib_LIBRARIES = libHYPRE_struct_ls.a

smgtopdir = ../.

libHYPRE_struct_ls_a_CFLAGS = \
	$(MPI_CPPFLAGS) \
	-I$(smgtopdir)\
	-I$(smgtopdir)/utilities\
	-I$(smgtopdir)/krylov\
	-I$(smgtopdir)/struct_mv\
	-I -g -gdwarf-2 -O -DTIMER_USE_MPI

libHYPRE_struct_ls_a_LDFLAGS = \
        -lm $(MPI_LDFLAGS) $(MPI_LIBS)

libHYPRE_struct_ls_a_SOURCES = \
	headers.h\
	smg.h\
	struct_ls.h\
	HYPRE_struct_ls.h\
	coarsen.c\
	cyclic_reduction.c\
	general.c\
	pcg_struct.c\
	point_relax.c\
	semi_interp.c\
	semi_restrict.c\
	smg2_setup_rap.c\
	smg3_setup_rap.c\
	smg.c\
	smg_axpy.c\
	smg_relax.c\
	smg_residual.c\
	smg_setup.c\
	smg_setup_interp.c\
	smg_setup_rap.c\
	smg_setup_restrict.c\
	smg_solve.c\
	HYPRE_struct_smg.c\
	HYPRE_struct_pcg.c
