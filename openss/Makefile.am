################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

#
# NOTE: Ideally those libraries (e.g. libopenss-queries) that use OpenMP would
#       be linked (via @OPENMP_LDFLAGS@ and @OPENMP_LIBS@) to its runtime. For
#       some unknown reason, however, that runtime cannot be dlopen()ed into a
#       process. The error "libgomp.so.1: shared object cannot be dlopen()ed"
#       is produced and the process immediately aborts. The fix is to have the
#       main "openss" executable link to the runtime, thus indirectly providing
#       the runtime for the other libraries that actually use it.
#

dist_bin_SCRIPTS = ossrun
BUILT_FILES = ossrun

bin_PROGRAMS =

bin_PROGRAMS += openss

openss_CXXFLAGS = \
        @BINUTILS_CPPFLAGS@ \
	-I$(top_srcdir)/libopenss-framework \
	-DLIBRARY_DIR=\"$(libdir)\" \
	-DPLUGIN_DIR=\"$(pkglibdir)\" \
	-DLIB_DIR_NAME=\"$(abi_libdir_name)\" \
	$(LTDLINCL)

openss_LDFLAGS = \
        @BINUTILS_LDFLAGS@ \
	@OPENMP_LDFLAGS@

openss_LDADD = \
        @BINUTILS_LIBS@ \
	$(LIBLTDL) \
	@OPENMP_LIBS@ \
	-lpthread

openss_SOURCES = \
	openss.cxx

bin_PROGRAMS += ossutil

LDFLAGS = 

ossutil_CXXFLAGS = \
        @BINUTILS_CPPFLAGS@ \
        @PAPI_CPPFLAGS@ \
        -I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-queries \
        -I$(top_srcdir)/libopenss-runtime \
        -DLIBRARY_DIR=\"$(libdir)\" \
        -DPLUGIN_DIR=\"$(pkglibdir)\" \
        -DLIB_DIR_NAME=\"$(abi_libdir_name)\" \
        $(LTDLINCL)

ossutil_LDFLAGS = \
        @PAPI_LDFLAGS@ @PAPI_LIBS@ \
        @BINUTILS_LDFLAGS@ \
        -L$(top_srcdir)/libopenss-framework \
	-L$(top_srcdir)/libopenss-queries \
        -L$(top_srcdir)/libopenss-runtime

ossutil_LDADD = \
        @BINUTILS_LIBS@ \
        $(LIBLTDL) \
	-lopenss-framework \
        -lpthread

ossutil_SOURCES = \
        ossutil.cxx


CLEANFILES = $(BUILT_FILES)

# OFFLINE and libmonitor support.
# This is done here to stay in the GNU coding standards
# pkglibdir can be modified at make time, so can't use
# variable substitution at configure time
# TODO: need to configure location of libmonitor.so
# via "-e 's,@@libmonitordir@@,$(monitorlibdir),' \"

ossrun: ossrun.in
	@rm -f ossrun
	@sed \
		-e 's,@@ossinstalldir@@,$(prefix),' \
		-e 's,@@libmonitordir@@,$(LIBMONITOR_DIR),' \
		-e 's,@@ossdefaultmpi@@,$(DEFAULT_MPI_IMPL),' \
			$(srcdir)/ossrun.in > ossrun
	@chmod 755 ossrun

EXTRA_DIST = ossrun.in
