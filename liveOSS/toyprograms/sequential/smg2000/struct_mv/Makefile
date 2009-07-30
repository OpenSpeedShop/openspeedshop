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

RANLIB= ranlib

HEADERS =\
 headers.h\
 struct_mv.h\
 HYPRE_struct_mv.h

FILES =\
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

OBJS = ${FILES:.c=.o}

##################################################################
# Targets
##################################################################

all: libHYPRE_struct_mv.a

clean:
	@rm -f *.o *.dxx

veryclean: clean
	@rm -f libHYPRE_*.a

##################################################################
# Rules
##################################################################

libHYPRE_struct_mv.a: ${OBJS}
	@echo  "Building $@ ... "
	ar -ru $@ ${OBJS}
	${RANLIB} $@

${OBJS}: ${HEADERS}

##################################################################
# Generic rules
##################################################################

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

