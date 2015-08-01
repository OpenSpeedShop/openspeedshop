################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2014 Krell Institute. All Rights Reserved.
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

dist_bin_SCRIPTS = ossdbtool ossanon
BUILT_FILES =
nodist_bin_SCRIPTS =

if BUILD_INSTRUMENTOR
if !BUILD_CBTF
nodist_bin_SCRIPTS += osslink
BUILT_FILES += osslink
endif
if !BUILD_RUNTIME_ONLY
nodist_bin_SCRIPTS += ossdriver ossdriver_compare
BUILT_FILES += ossdriver ossdriver_compare
# endif for !BUILD_RUNTIME_ONLY
endif
if !BUILD_CBTF
if HAVE_TARGET_SHARED
nodist_bin_SCRIPTS += ossrun
BUILT_FILES += ossrun
else
nodist_bin_SCRIPTS += ossrun
BUILT_FILES += ossrun
# endif for !BUILD_CBTF
endif

endif
# endif for BUILD_INSTRUMENTOR
endif

AM_LDFLAGS = 

bin_PROGRAMS =

#
# building target shared and not building the runtime are mutually exclusive
# therefore no else is used
#
if !BUILD_RUNTIME_ONLY
bin_PROGRAMS += openss 
if BUILD_CBTF
bin_PROGRAMS += osscollect 
xmldir = $(datadir)/KrellInstitute/xml
endif
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

osscollect_CXXFLAGS = \
	-DXMLDIR="\"$(xmldir)\"" \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-framework/cbtf \
	@BOOST_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
	@CBTF_KRELL_CPPFLAGS@ \
	@DYNINST_CPPFLAGS@ \
	$(LTDLINCL)

osscollect_LDFLAGS = \
        -L$(top_srcdir)/libopenss-framework \
        @DYNINST_LDFLAGS@ \
        @LIBELF_LDFLAGS@ \
        @LIBDWARF_LDFLAGS@ \
	@LIBXERCES_C_LDFLAGS@ \
	@CBTF_KRELL_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
        @BOOST_LDFLAGS@

osscollect_LDADD = \
	$(LIBLTDL) \
        @DYNINST_SYMTABAPI_LIBS@ \
	@CBTF_KRELL_CORE_SYMTABAPI_LIBS@ \
	@CBTF_KRELL_CORE_MRNET_LIBS@ \
        @LIBELF_LIBS@ \
        @LIBDWARF_LIBS@ \
	@LIBXERCES_C@ \
        @BOOST_PROGRAM_OPTIONS_LIB@ \
        @BOOST_FILESYSTEM_LIB@ \
        @BOOST_SYSTEM_LIB@ \
        @BOOST_THREAD_LIB@ \
	@MRNET_LIBS@ \
	-lopenss-framework \
	-lpthread

osscollect_SOURCES = \
	osscollect.cxx

#if BUILD_INSTRUMENTOR

if !BUILD_RUNTIME_ONLY
if !BUILD_CBTF
bin_PROGRAMS += ossutil

if OPENSS_USE_SYMTABAPI
ossutil_CXXFLAGS = \
        @SYMTABAPI_CPPFLAGS@ \
        @LIBDWARF_CPPFLAGS@ \
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

# endif for NOT CBTF
endif
# endif for NOT RUNTIME ONLY
endif

# endif for BUILD_INSTRUMENTOR
#endif

CLEANFILES = $(BUILT_FILES)

# OFFLINE and libmonitor support.
# This is done here to stay in the GNU coding standards
# pkglibdir can be modified at make time, so can't use
# variable substitution at configure time
# TODO: need to configure location of libmonitor.so
# via "-e 's,@libmonitordir@,$(monitorlibdir),' \"

if HAVE_RUNTIME_DIR
oss_location=$(RUNTIME_DIR)
else
oss_location=$(prefix)
endif

if BUILD_TARGETED
monitor_dir=$(TARGET_LIBMONITOR_DIR)
monitor_libdir=$(TARGET_LIBMONITOR_LIBDIR)
unwind_dir=$(TARGET_LIBUNWIND_DIR)
papi_dir=$(TARGET_PAPI_DIR)
mpich2_dir=$(TARGET_MPICH2_DIR)
mpich2_libdir=$(TARGET_MPICH2_LIBDIR)
else

monitor_dir=$(LIBMONITOR_DIR)
monitor_libdir=$(LIBMONITOR_LIBDIR)
unwind_dir=$(LIBUNWIND_DIR)
unwind_libdir=$(LIBUNWIND_LIBDIR)
papi_dir=$(PAPI_DIR)
papi_libdir=$(PAPI_LIBDIR)
mpich2_dir=$(MPICH2_DIR)
mpich2_libdir=$(MPICH2_LIBDIR)

endif

if BUILD_INSTRUMENTOR
osslink: osslink.in
	@rm -f osslink
	@sed \
		-e 's,@default_mpi_impl@,$(DEFAULT_MPI_IMPL),' \
		-e 's,@ossinstalldir@,$(oss_location),' \
		-e 's,@libmonitordir@,$(monitor_dir),' \
		-e 's,@libmonitorlibdir@,$(monitor_libdir),' \
		-e 's,@libunwinddir@,$(unwind_dir),' \
		-e 's,@libunwindlibdir@,$(unwind_libdir),' \
		-e 's,@osstargetos@,$(TARGET_OS),' \
		-e 's,@papidir@,$(papi_dir),' \
		-e 's,@papilibdir@,$(papi_libdir),' \
			$(srcdir)/osslink.in > osslink
	@chmod 755 osslink


if HAVE_DYNINST
if !BUILD_CBTF
bin_PROGRAMS += osswrapper 

osswrapper_CXXFLAGS = \
        @DYNINST_CPPFLAGS@ \
        @BOOST_CPPFLAGS@ \
        @LIBELF_CPPFLAGS@ \
        @LIBDWARF_CPPFLAGS@ 

osswrapper_LDFLAGS = \
        @DYNINST_LDFLAGS@ \
        @LIBELF_LDFLAGS@ \
        @LIBDWARF_LDFLAGS@ 

osswrapper_LDADD = \
	$(LIBLTDL) \
        @DYNINST_LIBS@ \
        @LIBELF_LIBS@ \
        @LIBDWARF_LIBS@ \
         -lpthread

osswrapper_SOURCES = \
	osswrapper.cxx

endif
endif
endif

if BUILD_INSTRUMENTOR

if !BUILD_CBTF
ossrun: ossrun.in
endif
	@rm -f ossrun
	@sed \
		-e 's,@machine_dependent_libdir@,$(abi_libdir_name),' \
		-e 's,@alt_machine_dependent_libdir@,$(alt_abi_libdir_name),' \
		-e 's,@ossinstalldir@,$(oss_location),' \
		-e 's,@libmonitordir@,$(monitor_dir),' \
		-e 's,@libmonitorlibdir@,$(monitor_libdir),' \
		-e 's,@lamdir@,$(LAM_DIR),' \
		-e 's,@lamlibdir@,$(LAM_LIBDIR),' \
		-e 's,@lampidir@,$(LAMPI_DIR),' \
		-e 's,@lampilibdir@,$(LAMPI_LIBDIR),' \
		-e 's,@mpichdir@,$(MPICH_DIR),' \
		-e 's,@mpichlibdir@,$(MPICH_LIBDIR),' \
		-e 's,@mpich2dir@,$(mpich2_dir),' \
		-e 's,@mpich2libdir@,$(mpich2_libdir),' \
		-e 's,@mvapichdir@,$(MVAPICH_DIR),' \
		-e 's,@mvapichlibdir@,$(MVAPICH_LIBDIR),' \
		-e 's,@mvapich2dir@,$(MVAPICH2_DIR),' \
		-e 's,@mvapich2libdir@,$(MVAPICH2_LIBDIR),' \
		-e 's,@mptdir@,$(MPT_DIR),' \
		-e 's,@mptlibdir@,$(MPT_LIBDIR),' \
		-e 's,@openmpidir@,$(OPENMPI_DIR),' \
		-e 's,@openmpilibdir@,$(OPENMPI_LIBDIR),' \
		-e 's,@ossdefaultmpi@,$(DEFAULT_MPI_IMPL),' \
			$(srcdir)/ossrun.in > ossrun
	@chmod 755 ossrun

if !BUILD_TARGETED
if !BUILD_RUNTIME_ONLY
ossdriver: ossdriver.in
	@rm -f ossdriver
	@sed \
		-e 's,@oss_instrumentor@,$(INSTRUMENTOR),' \
		-e 's,@oss_targetos@,$(TARGET_OS),' \
			$(srcdir)/ossdriver.in > ossdriver
	@chmod 755 ossdriver

ossdriver_compare: ossdriver_compare.in
	@rm -f ossdriver_compare
	cp $(srcdir)/ossdriver_compare.in ossdriver_compare
	chmod 755 ossdriver_compare

install-exec-hook: ossdriver ossdriver_compare
if !BUILD_MRNET
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
if BUILD_CBTF
# ossiop
	@rm -f $(DESTDIR)$(bindir)/ossiop
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/ossiop
	chmod 755 $(DESTDIR)$(bindir)/ossiop
# ossmem
	@rm -f $(DESTDIR)$(bindir)/ossmem
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/ossmem
	chmod 755 $(DESTDIR)$(bindir)/ossmem
# osspthreads
	@rm -f $(DESTDIR)$(bindir)/osspthreads
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/osspthreads
	chmod 755 $(DESTDIR)$(bindir)/osspthreads
# osscuda
	@rm -f $(DESTDIR)$(bindir)/osscuda
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/osscuda
	chmod 755 $(DESTDIR)$(bindir)/osscuda
# osscudaio
	@rm -f $(DESTDIR)$(bindir)/osscudaio
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/osscudaio
	chmod 755 $(DESTDIR)$(bindir)/osscudaio
# ossmpip
	@rm -f $(DESTDIR)$(bindir)/ossmpip
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver  $(DESTDIR)$(bindir)/ossmpip
	chmod 755 $(DESTDIR)$(bindir)/ossmpip
# endif for BUILD_CBTF
endif
# endif for !BUILD_MRNET
endif

# osscompare
	@rm -f $(DESTDIR)$(bindir)/osscompare
	$(LN_S) $(DESTDIR)$(bindir)/ossdriver_compare  $(DESTDIR)$(bindir)/osscompare
	chmod 755 $(DESTDIR)$(bindir)/osscompare

# endif for !BUILD_RUNTIME_ONLY
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
	@rm -f $(DESTDIR)$(bindir)/ossiop
	@rm -f $(DESTDIR)$(bindir)/ossmem
	@rm -f $(DESTDIR)$(bindir)/osspthreads
	@rm -f $(DESTDIR)$(bindir)/osscuda
	@rm -f $(DESTDIR)$(bindir)/osscudaio
	@rm -f $(DESTDIR)$(bindir)/ossmpip

# endif for BUILD_INSTRUMENTOR
endif

EXTRA_DIST = ossrun.in ossdriver.in ossdriver_compare.in osslink.in
