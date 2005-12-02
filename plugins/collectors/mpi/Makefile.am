################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

MPIWRAPPERS = \
	MPI_Allgather.c \
	MPI_Allgatherv.c \
	MPI_Allreduce.c \
	MPI_Alltoall.c \
	MPI_Alltoallv.c \
	MPI_Barrier.c \
	MPI_Bcast.c \
	MPI_Bsend.c \
	MPI_Cancel.c \
	MPI_Gather.c \
	MPI_Gatherv.c \
	MPI_Get_count.c \
	MPI_Ibsend.c \
	MPI_Irecv.c \
	MPI_Irsend.c \
	MPI_Isend.c \
	MPI_Issend.c \
	MPI_Pack.c \
	MPI_Probe.c \
	MPI_Recv.c \
	MPI_Reduce.c \
	MPI_Reduce_scatter.c \
	MPI_Request_free.c \
	MPI_Rsend.c \
	MPI_Scan.c \
	MPI_Scatter.c \
	MPI_Scatterv.c \
	MPI_Send.c \
	MPI_Sendrecv.c \
	MPI_Sendrecv_replace.c \
	MPI_Ssend.c \
	MPI_Test.c \
	MPI_Testall.c \
	MPI_Testany.c \
	MPI_Testsome.c \
	MPI_Unpack.c \
	MPI_Wait.c \
	MPI_Waitall.c \
	MPI_Waitany.c \
	MPI_Waitsome.c \
        $(NULL)

BUILT_SOURCES = \
	blobs.h blobs.c

pkglib_LTLIBRARIES = mpi.la mpi-rt.la

mpi_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

mpi_la_LDFLAGS = \
	-no-undefined -module -avoid-version

mpi_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	MPICollector.hxx MPICollector.cxx

mpi_rt_la_CFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_CPPFLAGS@ \
	@MPI_CPPFLAGS@

mpi_rt_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_LDFLAGS@ \
	@MPI_LDFLAGS@ \
	-no-undefined -module -avoid-version

mpi_rt_la_LIBADD = \
	-lopenss-runtime \
	@LIBUNWIND_LIBS@ \
	@MPI_LIBS@ \
	-lpthread -lrt

mpi_rt_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) $(MPIWRAPPERS) \
	wrappers.c \
	mkwrapper.c runtime.h runtime.c

EXTRA_DIST = mkwrapper.template

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES) $(MPIWRAPPERS) mkwrapper

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

$(MPIWRAPPERS) : mkwrapper mkwrapper.template
	./mkwrapper \
		` echo $@ | sed -e 's/\.c//' ` \
		@MPI_HEADER@ \
		mkwrapper.template \
		> $@

mkwrapper : mkwrapper.c
	$(CC) -o $@ $<
