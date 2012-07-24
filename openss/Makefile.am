################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2012 Krell Institute. All Rights Reserved.
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

dist_bin_SCRIPTS = ossdbtool
BUILT_FILES =

if BUILD_INSTRUMENTOR
if !BUILD_CBTF
dist_bin_SCRIPTS += osslink
BUILT_FILES += osslink
endif
if !BUILD_RUNTIME_ONLY
dist_bin_SCRIPTS += ossdriver
BUILT_FILES += ossdriver
if !BUILD_CBTF
dist_bin_SCRIPTS += ossrun
BUILT_FILES += ossrun
endif
endif
if HAVE_TARGET_SHARED
dist_bin_SCRIPTS += ossrun
BUILT_FILES += ossrun
endif
endif

LDFLAGS = 

bin_PROGRAMS =

#
# building target shared and not building the runtime are mutually exclusive
# therefore no else is used
#
if !BUILD_RUNTIME_ONLY
bin_PROGRAMS += openss 
endif

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

if BUILD_INSTRUMENTOR

if !BUILD_RUNTIME_ONLY
if !BUILD_CBTF
bin_PROGRAMS += ossutil

if OPENSS_USE_SYMTABAPI
ossutil_CXXFLAGS = \
        @SYMTABAPI_CPPFLAGS@ \
        -I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-queries \
        -I$(top_srcdir)/libopenss-runtime \
        -DLIBRARY_DIR=\"$(libdir)\" \
        -DPLUGIN_DIR=\"$(pkglibdir)\" \
        -DLIB_DIR_NAME=\"$(abi_libdir_name)\" \
        $(LTDLINCL)

ossutil_LDFLAGS = \
        @SYMTABAPI_LDFLAGS@ \
        -L$(top_srcdir)/libopenss-framework \
	-L$(top_srcdir)/libopenss-queries \
        -L$(top_srcdir)/libopenss-runtime

ossutil_LDADD = \
        @SYMTABAPI_LIBS@ \
        $(LIBLTDL) \
	-lopenss-framework \
	-lopenss-framework-symtabapi \
        -lpthread
else
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
endif

ossutil_SOURCES = \
        ossutil.cxx

endif
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
mpich2_dir=$(TARGET_MPICH2_DIR)
else
monitor_dir=$(LIBMONITOR_DIR)
unwind_dir=$(LIBUNWIND_DIR)
papi_dir=$(PAPI_DIR)
mpich2_dir=$(MPICH2_DIR)
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


if HAVE_DYNINST
if !BUILD_CBTF
bin_PROGRAMS += osswrapper 

osswrapper_CXXFLAGS = \
        @DYNINST_CPPFLAGS@ \
        @LIBDWARF_CPPFLAGS@ 

osswrapper_LDFLAGS = \
        @DYNINST_LDFLAGS@ \
        @LIBDWARF_LDFLAGS@ 

osswrapper_LDADD = \
	$(LIBLTDL) \
        @DYNINST_LIBS@ \
        @LIBDWARF_LIBS@ \
         -lpthread

osswrapper_SOURCES = \
	osswrapper.cxx

endif
endif
endif

if BUILD_INSTRUMENTOR

if !BUILD_RUNTIME_ONLY
if !BUILD_CBTF
ossrun: ossrun.in
endif
endif
	@rm -f ossrun
	@sed \
		-e 's,@@machine_dependent_libdir@@,$(abi_libdir_name),' \
		-e 's,@@alt_machine_dependent_libdir@@,$(alt_abi_libdir_name),' \
		-e 's,@@ossinstalldir@@,$(prefix),' \
		-e 's,@@libmonitordir@@,$(monitor_dir),' \
		-e 's,@@lamdir@@,$(LAM_DIR),' \
		-e 's,@@lampidir@@,$(LAMPI_DIR),' \
		-e 's,@@mpichdir@@,$(MPICH_DIR),' \
		-e 's,@@mpich2dir@@,$(mpich2_dir),' \
		-e 's,@@mvapichdir@@,$(MVAPICH_DIR),' \
		-e 's,@@mvapich2dir@@,$(MVAPICH2_DIR),' \
		-e 's,@@mptdir@@,$(MPT_DIR),' \
		-e 's,@@openmpidir@@,$(OPENMPI_DIR),' \
		-e 's,@@ossdefaultmpi@@,$(DEFAULT_MPI_IMPL),' \
			$(srcdir)/ossrun.in > ossrun
	@chmod 755 ossrun

if !BUILD_TARGETED 
ossdriver: ossdriver.in
	@rm -f ossdriver
	cp $(srcdir)/ossdriver.in ossdriver
	chmod 755 ossdriver

install-exec-hook: ossdriver
if !BUILD_CBTF
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

# endif for !BUILD_TARGETED
endif


uninstall-hook:
	@rm -f $(DESTDIR)$(bindir)/osscompare
	@rm -f $(DESTDIR)$(bindir)/ossfpe
	@rm -f $(DESTDIR)$(bindir)/osshwctime
	@rm -f $(DESTDIR)$(bindir)/osshwcsamp
	@rm -f $(DESTDIR)$(bindir)/osshwc
	@rm -f $(DESTDIR)$(bindir)/osspcsamp
	@rm -f $(DESTDIR)$(bindir)/ossusertime
	@rm -f $(DESTDIR)$(bindir)/ossmpiotf
	@rm -f $(DESTDIR)$(bindir)/ossmpit
	@rm -f $(DESTDIR)$(bindir)/ossmpi
	@rm -f $(DESTDIR)$(bindir)/ossio
	@rm -f $(DESTDIR)$(bindir)/ossiot

# endif for BUILD_INSTRUMENTOR
endif

EXTRA_DIST = ossrun.in ossdriver.in osslink.in
