#BHEADER***********************************************************************
# (c) 2000   The Regents of the University of California
#
# See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
# notice, contact person, and disclaimer.
#
#EHEADER***********************************************************************

lib_LIBRARIES = libHYPRE_struct_mv.a

MPI_IMPL = @DEFAULT_MPI_IMPL@

MPICC	= $($(MPI_IMPL)_CC)
CC	= $(MPICC)

smgtopdir = ../.

libHYPRE_struct_mv_a_CFLAGS =\
	$($(MPI_IMPL)_CPPFLAGS) \
	-I$(smgtopdir)\
	-I$(smgtopdir)/utilities\
	-I -g -gdwarf-2 -O1 -DTIMER_USE_MPI -DHYPRE_USING_OPENMP -fopenmp

libHYPRE_struct_mv_a_LDFLAGS = \
        -lm $($(MPI_IMPL)_LDFLAGS) $($(MPI_IMPL)_LIBS)

libHYPRE_struct_mv_a_SOURCES =\
	headers.h\
	struct_mv.h\
	HYPRE_struct_mv.h\
	box.c\
	box_algebra.c\
	box_alloc.c\
	box_neighbors.c\
	communication.c\
	communication_info.c\
	computation.c\
	grow.c\
	project.c\
	struct_grid.c\
	struct_io.c\
	struct_axpy.c\
	struct_copy.c\
	struct_innerprod.c\
	struct_matrix.c\
	struct_matrix_mask.c\
	struct_matvec.c\
	struct_scale.c\
	struct_stencil.c\
	struct_vector.c\
	HYPRE_struct_grid.c\
	HYPRE_struct_matrix.c\
	HYPRE_struct_stencil.c\
	HYPRE_struct_vector.c
