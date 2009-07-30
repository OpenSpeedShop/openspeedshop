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

CFLAGS = \
 -I..\
 -I../utilities\
 -I../krylov\
 -I../struct_mv\
 ${INCLUDE_CFLAGS}

RANLIB = ranlib
 
HEADERS =\
 headers.h\
 smg.h\
 struct_ls.h\
 HYPRE_struct_ls.h

FILES =\
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

OBJS = ${FILES:.c=.o}

##################################################################
# Targets
##################################################################

all: libHYPRE_struct_ls.a 

clean:
	@rm -f *.o *.dxx 

veryclean: clean
	@rm -f libHYPRE_*.a


##################################################################
# Rules
##################################################################

libHYPRE_struct_ls.a: ${OBJS}
	@echo  "Building $@ ... "
	ar -ru $@ ${OBJS}
	${RANLIB} $@

${OBJS}: ${HEADERS}



##################################################################
# Generic rules
##################################################################

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

