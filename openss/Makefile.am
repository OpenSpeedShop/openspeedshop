################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2009 Krell Institute. All Rights Reserved.
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

dist_bin_SCRIPTS =
BUILT_FILES =

if BUILD_INSTRUMENTOR
dist_bin_SCRIPTS += osslink
BUILT_FILES += osslink
if !BUILD_RUNTIME
dist_bin_SCRIPTS += ossrun ossdriver
BUILT_FILES += ossrun ossdriver
endif
endif

LDFLAGS = 

bin_PROGRAMS =

if !BUILD_RUNTIME
bin_PROGRAMS += openss 


openss_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework \
	-DLIBRARY_DIR=\"$(libdir)\" \
	-DPLUGIN_DIR=\"$(pkglibdir)\" \
	-DLIB_DIR_NAME=\"$(abi_libdir_name)\" \
	$(LTDLINCL)

openss_LDFLAGS = \
	@OPENMP_LDFLAGS@

openss_LDADD = \
	$(LIBLTDL) \
	@OPENMP_LIBS@ \
	-lpthread

openss_SOURCES = \
	openss.cxx
endif

if BUILD_INSTRUMENTOR
if !BUILD_RUNTIME
bin_PROGRAMS += ossutil

ossutil_CXXFLAGS = \
        @BINUTILS_CPPFLAGS@ \
        -I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-queries \
        -I$(top_srcdir)/libopenss-runtime \
        -DLIBRARY_DIR=\"$(libdir)\" \
        -DPLUGIN_DIR=\"$(pkglibdir)\" \
        -DLIB_DIR_NAME=\"$(abi_libdir_name)\" \
        $(LTDLINCL)

ossutil_LDFLAGS = \
        @BINUTILS_LDFLAGS@ \
        -L$(top_srcdir)/libopenss-framework \
	-L$(top_srcdir)/libopenss-queries \
        -L$(top_srcdir)/libopenss-runtime

ossutil_LDADD = \
        @BINUTILS_BFD_LIB@ \
        $(LIBLTDL) \
	-lopenss-framework \
	-lopenss-framework-binutils \
        -lpthread

ossutil_SOURCES = \
        ossutil.cxx
endif
endif


CLEANFILES = $(BUILT_FILES)

# OFFLINE and libmonitor support.
# This is done here to stay in the GNU coding standards
# pkglibdir can be modified at make time, so can't use
# variable substitution at configure time
# TODO: need to configure location of libmonitor.so
# via "-e 's,@@libmonitordir@@,$(monitorlibdir),' \"

if BUILD_TARGETED
monitor_dir=$(TARGET_LIBMONITOR_DIR)
unwind_dir=$(TARGET_LIBUNWIND_DIR)
papi_dir=$(TARGET_PAPI_DIR)
else
monitor_dir=$(LIBMONITOR_DIR)
unwind_dir=$(LIBUNWIND_DIR)
papi_dir=$(PAPI_DIR)
endif

if BUILD_INSTRUMENTOR
osslink: osslink.in
	@rm -f osslink
	@sed \
		-e 's,@@default_mpi_impl@@,$(DEFAULT_MPI_IMPL),' \
		-e 's,@@ossinstalldir@@,$(prefix),' \
		-e 's,@@libmonitordir@@,$(monitor_dir),' \
		-e 's,@@libunwinddir@@,$(unwind_dir),' \
		-e 's,@@osstargetos@@,$(TARGET_OS),' \
		-e 's,@@papidir@@,$(papi_dir),' \
			$(srcdir)/osslink.in > osslink
	@chmod 755 osslink
endif

if BUILD_INSTRUMENTOR
if !BUILD_RUNTIME
ossrun: ossrun.in
	@rm -f ossrun
	@sed \
		-e 's,@@machine_dependent_libdir@@,$(abi_libdir_name),' \
		-e 's,@@alt_machine_dependent_libdir@@,$(alt_abi_libdir_name),' \
		-e 's,@@ossinstalldir@@,$(prefix),' \
		-e 's,@@libmonitordir@@,$(LIBMONITOR_DIR),' \
		-e 's,@@lamdir@@,$(LAM_DIR),' \
		-e 's,@@lampidir@@,$(LAMPI_DIR),' \
		-e 's,@@mpichdir@@,$(MPICH_DIR),' \
		-e 's,@@mpich2dir@@,$(MPICH2_DIR),' \
		-e 's,@@mvapichdir@@,$(MVAPICH_DIR),' \
		-e 's,@@mvapich2dir@@,$(MVAPICH2_DIR),' \
		-e 's,@@mptdir@@,$(MPT_DIR),' \
		-e 's,@@openmpidir@@,$(OPENMPI_DIR),' \
		-e 's,@@ossdefaultmpi@@,$(DEFAULT_MPI_IMPL),' \
			$(srcdir)/ossrun.in > ossrun
	@chmod 755 ossrun

ossdriver: ossdriver.in
	@rm -f ossdriver
	cp $(srcdir)/ossdriver.in ossdriver
	chmod 755 ossdriver

install-exec-hook: ossdriver
# osspcsamp
	@rm -f $(DESTDIR)$(bindir)/osspcsamp
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/osspcsamp
	chmod 755 $(DESTDIR)$(bindir)/osspcsamp
# ossusertime
	@rm -f $(DESTDIR)$(bindir)/ossusertime
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/ossusertime
	chmod 755 $(DESTDIR)$(bindir)/ossusertime
# osshwc
	@rm -f $(DESTDIR)$(bindir)/osshwc
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/osshwc
	chmod 755 $(DESTDIR)$(bindir)/osshwc
# osshwcsamp
	@rm -f $(DESTDIR)$(bindir)/osshwcsamp
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/osshwcsamp
	chmod 755 $(DESTDIR)$(bindir)/osshwcsamp
# osshwctime
	@rm -f $(DESTDIR)$(bindir)/osshwctime
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/osshwctime
	chmod 755 $(DESTDIR)$(bindir)/osshwctime
# ossio
	@rm -f $(DESTDIR)$(bindir)/ossio
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/ossio
	chmod 755 $(DESTDIR)$(bindir)/ossio
# ossiot
	@rm -f $(DESTDIR)$(bindir)/ossiot
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/ossiot
	chmod 755 $(DESTDIR)$(bindir)/ossiot
# ossmpi
	@rm -f $(DESTDIR)$(bindir)/ossmpi
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/ossmpi
	chmod 755 $(DESTDIR)$(bindir)/ossmpi
# ossmpit
	@rm -f $(DESTDIR)$(bindir)/ossmpit
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/ossmpit
	chmod 755 $(DESTDIR)$(bindir)/ossmpit
# ossmpiotf
	@rm -f $(DESTDIR)$(bindir)/ossmpiotf
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/ossmpiotf
	chmod 755 $(DESTDIR)$(bindir)/ossmpiotf
# ossfpe
	@rm -f $(DESTDIR)$(bindir)/ossfpe
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/ossfpe
	chmod 755 $(DESTDIR)$(bindir)/ossfpe
# osscompare
	@rm -f $(DESTDIR)$(bindir)/osscompare
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/osscompare
	chmod 755 $(DESTDIR)$(bindir)/osscompare
endif
endif

EXTRA_DIST = ossrun.in ossdriver.in osslink.in
