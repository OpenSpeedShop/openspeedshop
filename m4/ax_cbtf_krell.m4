################################################################################
# Copyright (c) 2015 Krell Institute. All Rights Reserved.
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

################################################################################
# Check for CBTF KRELL
################################################################################

AC_DEFUN([AX_CBTF_KRELL], [

    AC_ARG_WITH(cbtf-krell,
                AC_HELP_STRING([--with-cbtf-krell=DIR],
                               [CBTF krell installation @<:@/usr@:>@]),
                cbtf_krell_dir=$withval, cbtf_krell_dir="/usr")


    CBTF_KRELL_INSTALLDIR="$cbtf_krell_dir"
    CBTF_KRELL_LIBDIR="$cbtf_krell_dir/$abi_libdir"
    CBTF_TOOLS_CBTFRUN="$cbtf_krell_dir/bin/cbtfrun"
    CBTF_TOOLS_XML="$cbtf_krell_dir/share/KrellInstitute/xml"
    CBTF_TOOLS_COLLECTORS="$cbtf_krell_dir/$abi_libdir/KrellInstitute/Collectors"
    CBTF_TOOLS_COMPONENTS="$cbtf_krell_dir/$abi_libdir/KrellInstitute/Components"

    AC_DEFINE_UNQUOTED(CBTF_KRELL_INSTALL_DIR, "$CBTF_KRELL_INSTALLDIR",
                           [Name of full path to cbtf krell installtation])
    AC_DEFINE_UNQUOTED(CBTF_KRELL_LIB_DIR, "$CBTF_KRELL_LIBDIR",
                           [Name of full path to cbtf krell lib directory])
    AC_DEFINE_UNQUOTED(CBTF_TOOLS_CBTFRUN_CMD, "$CBTF_TOOLS_CBTFRUN",
                           [Name of full path to cbtf krell cbtfrun command])
    AC_DEFINE_UNQUOTED(CBTF_TOOLS_XMLDIR, "$CBTF_TOOLS_XML",
                           [Name of full path to cbtf krell xml components])
    AC_DEFINE_UNQUOTED(CBTF_TOOLS_COLLECTORSDIR, "$CBTF_TOOLS_COLLECTORS",
                           [Name of full path to cbtf krell collectors])
    AC_DEFINE_UNQUOTED(CBTF_TOOLS_COMPONENTSDIR, "$CBTF_TOOLS_COMPONENTS",
                           [Name of full path to cbtf krell components])

    AC_SUBST([CBTF_KRELL_INSTALLDIR])
    AC_SUBST([CBTF_KRELL_LIBDIR])
    AC_SUBST([CBTF_TOOLS_XML])
    AC_SUBST([CBTF_TOOLS_COLLECTORS])
    AC_SUBST([CBTF_TOOLS_COMPONENTS])
    AC_SUBST([CBTF_TOOLS_CBTFRUN])

    CBTF_KRELL_CPPFLAGS="-I$cbtf_krell_dir/include"
    CBTF_KRELL_LDFLAGS="-L$cbtf_krell_dir/$abi_libdir"
    CBTF_KRELL_CORE_LIBS="-lcbtf-core"
    CBTF_KRELL_CORE_SYMTABAPI_LIBS="-lcbtf-core-symtabapi"
    CBTF_KRELL_CORE_MRNET_LIBS="-lcbtf-core-mrnet"
    CBTF_KRELL_CORE_BFD_LIBS="-lcbtf-core-bfd"


    CBTF_KRELL_CORE_LIBS="$CBTF_KRELL_CORE_LIBS"
    CBTF_KRELL_CORE_BFD_LIBS="$CBTF_KRELL_CORE_BFD_LIBS"
    CBTF_KRELL_CORE_MRNET_LIBS="$CBTF_KRELL_CORE_MRNET_LIBS"
    CBTF_KRELL_CORE_SYMTABAPI_LIBS="$CBTF_KRELL_CORE_SYMTABAPI_LIBS"

    CBTF_KRELL_MESSAGES_LIBS="-lcbtf-messages-base -lcbtf-messages-collector -lcbtf-messages-events -lcbtf-messages-instrumentation -lcbtf-messages-perfdata -lcbtf-messages-symtab -lcbtf-messages-thread"
    CBTF_KRELL_MESSAGES_BASE_LIBS="-lcbtf-messages-base"
    CBTF_KRELL_MESSAGES_COLLECTOR_LIBS="-lcbtf-messages-collector"
    CBTF_KRELL_MESSAGES_EVENTS_LIBS="-lcbtf-messages-events"
    CBTF_KRELL_MESSAGES_INSTRUMENTATION_LIBS="-lcbtf-messages-instrumentation"
    CBTF_KRELL_MESSAGES_PERFDATA_LIBS="-lcbtf-messages-perfdata"
    CBTF_KRELL_MESSAGES_SYMTAB_LIBS="-lcbtf-messages-symtab"
    CBTF_KRELL_MESSAGES_THREAD_LIBS="-lcbtf-messages-thread"

    CBTF_KRELL_SERVICES_BINUTILS_LIBS="-lcbtf-services-binutils"
    CBTF_KRELL_SERVICES_COMMON_LIBS="-lcbtf-services-common"
    CBTF_KRELL_SERVICES_DATA_LIBS="-lcbtf-services-data"
    CBTF_KRELL_SERVICES_FILEIO_LIBS="-lcbtf-services-fileio"
    CBTF_KRELL_SERVICES_FPE_LIBS="-lcbtf-services-fpe"
    CBTF_KRELL_SERVICES_MONITOR_LIBS="-lcbtf-services-monitor"
    CBTF_KRELL_SERVICES_MRNET_LIBS="-lcbtf-services-mrnet"
    CBTF_KRELL_SERVICES_OFFLINE_LIBS="-lcbtf-services-offline"
    CBTF_KRELL_SERVICES_PAPI_LIBS="-lcbtf-services-papi"
    CBTF_KRELL_SERVICES_SEND_LIBS="-lcbtf-services-send"
    CBTF_KRELL_SERVICES_TIMER_LIBS="-lcbtf-services-timer"
    CBTF_KRELL_SERVICES_UNWIND_LIBS="-lcbtf-services-unwind"
    CBTF_KRELL_SERVICES_LIBS="$CBTF_KRELL_SERVICES_BINUTILS_LIBS $CBTF_KRELL_SERVICES_COMMON_LIBS $CBTF_KRELL_SERVICES_DATA_LIBS $CBTF_KRELL_SERVICES_FILEIO_LIBS $CBTF_KRELL_SERVICES_FPE_LIBS $CBTF_KRELL_SERVICES_MONITOR_LIBS $CBTF_KRELL_SERVICES_MRNET_LIBS $CBTF_KRELL_SERVICES_OFFLINE_LIBS $CBTF_KRELL_SERVICES_PAPI_LIBS $CBTF_KRELL_SERVICES_SEND_LIBS $CBTF_KRELL_SERVICES_TIMER_LIBS $CBTF_KRELL_SERVICES_UNWIND_LIBS"

    cbtf_krell_saved_CPPFLAGS=$CPPFLAGS
    cbtf_krell_saved_LDFLAGS=$LDFLAGS
    cbtf_krell_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $CBTF_KRELL_CPPFLAGS"
    LDFLAGS="$LDFLAGS $CBTF_KRELL_LDFLAGS"
    LIBS="$CBTF_KRELL_CORE_LIBS $CBTF_KRELL_MESSAGES_BASE_LIBS $CBTF_KRELL_MESSAGES_PERFDATA_LIBS -lrt -lpthread"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    AC_MSG_CHECKING([for CBTF KRELL core library and headers])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <KrellInstitute/Core/AddressBuffer.hpp>
        ]], [[
        ]])], [
            AC_MSG_RESULT(yes)
            AM_CONDITIONAL(HAVE_CBTF_KRELL_CORE, true)
            AC_DEFINE(HAVE_CBTF_KRELL_CORE, 1,
                      [Define to 1 if you have CBTF Krell core])
        ], [
            AC_MSG_RESULT(no)
            AM_CONDITIONAL(HAVE_CBTF_KRELL_CORE, false)
            AC_DEFINE(HAVE_CBTF_KRELL_CORE, 0,
                      [Define to 1 if you have CBTF Krell core ])
        ])

    CPPFLAGS=$cbtf_krell_saved_CPPFLAGS
    LDFLAGS=$cbtf_krell_saved_LDFLAGS
    LIBS=$cbtf_krell_saved_LIBS

    CPPFLAGS="$CPPFLAGS $CBTF_KRELL_CPPFLAGS $SYMTABAPI_CPPFLAGS"
    LDFLAGS="$LDFLAGS $CBTF_KRELL_LDFLAGS"
    LIBS="$CBTF_KRELL_CORE_SYMTABAPI_LIBS -lrt -lpthread"
    AC_MSG_CHECKING([for CBTF KRELL core symtabapi library and headers])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
        #include <KrellInstitute/Core/SymtabAPISymbols.hpp>
        using namespace KrellInstitute::Core;

        ]], [[
        SymtabAPISymbols syms = SymtabAPISymbols();
        ]])], [
	    found_core_symtabapi=1
            AC_MSG_RESULT(yes)
            AM_CONDITIONAL(HAVE_CBTF_KRELL_CORE_SYMTABAPI, true)
            AC_DEFINE(HAVE_CBTF_KRELL_CORE_SYMTABAPI, 1,
                      [Define to 1 if you have CBTF Krell core symtabapi])
        ], [
	    found_core_symtabapi=0
            AC_MSG_RESULT(no)
            AM_CONDITIONAL(HAVE_CBTF_KRELL_CORE_SYMTABAPI, false)
            AC_DEFINE(HAVE_CBTF_KRELL_CORE_SYMTABAPI, 0,
                      [Define to 1 if you have CBTF Krell core symtabapi ])
        ])

    CPPFLAGS=$cbtf_krell_saved_CPPFLAGS
    LDFLAGS=$cbtf_krell_saved_LDFLAGS
    LIBS=$cbtf_krell_saved_LIBS

    CPPFLAGS="$CPPFLAGS $CBTF_KRELL_CPPFLAGS $MRNET_CPPFLAGS"
    LDFLAGS="$LDFLAGS $CBTF_KRELL_LDFLAGS"
    LIBS="$CBTF_KRELL_CORE_MRNET_LIBS -lrt -lpthread"

    AC_MSG_CHECKING([for CBTF KRELL core mrnet library and headers])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
        #include <iomanip>
        #include <sstream>
        #include <KrellInstitute/Core/CBTFTopology.hpp>
        using namespace KrellInstitute::Core;

        ]], [[
        CBTFTopology topology = CBTFTopology();
        ]])], [
	    found_core_mrnet=1
            AC_MSG_RESULT(yes)
            AM_CONDITIONAL(HAVE_CBTF_KRELL_CORE_MRNET, true)
            AC_DEFINE(HAVE_CBTF_KRELL_CORE_MRNET, 1,
                      [Define to 1 if you have CBTF Krell core mrnet])
        ], [
	    found_core_mrnet=1
            AC_MSG_RESULT(no)
            AM_CONDITIONAL(HAVE_CBTF_KRELL_CORE_MRNET, false)
            AC_DEFINE(HAVE_CBTF_KRELL_CORE_MRNET, 0,
                      [Define to 1 if you have CBTF Krell core mrnet ])
        ])

    CPPFLAGS=$cbtf_krell_saved_CPPFLAGS
    LDFLAGS=$cbtf_krell_saved_LDFLAGS
    LIBS=$cbtf_krell_saved_LIBS

    cbtf_krell_saved_CPPFLAGS=$CPPFLAGS
    cbtf_krell_saved_LDFLAGS=$LDFLAGS
    cbtf_krell_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $CBTF_KRELL_CPPFLAGS"
    LDFLAGS="$LDFLAGS $CBTF_KRELL_LDFLAGS"
    LIBS="$CBTF_KRELL_MESSAGES_BASE_LIBS"

    AC_MSG_CHECKING([for CBTF KRELL messages library and headers])
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <KrellInstitute/Messages/Address.h>
        ]], [[
        ]])], [
            AC_MSG_RESULT(yes)
            AM_CONDITIONAL(HAVE_CBTF_KRELL_MESSAGES, true)
            AC_DEFINE(HAVE_CBTF_KRELL_MESSAGES, 1,
                      [Define to 1 if you have CBTF Krell messages])
        ], [
            AC_MSG_RESULT(no)
            AM_CONDITIONAL(HAVE_CBTF_KRELL_MESSAGES, false)
            AC_DEFINE(HAVE_CBTF_KRELL_MESSAGES, 0,
                      [Define to 1 if you have CBTF Krell messages ])
            AC_MSG_ERROR([CBTF messages library could not be found.])
        ])

    CPPFLAGS=$cbtf_krell_saved_CPPFLAGS
    LDFLAGS=$cbtf_krellmessages_saved_LDFLAGS
    LIBS=$cbtf_krellmessages_saved_LIBS

    LDFLAGS="$LDFLAGS $CBTF_KRELL_LDFLAGS"
    LIBS="$CBTF_KRELL_SERVICES_COMMON_LIBS"

    AC_MSG_CHECKING([for CBTF KRELL services library and headers])
    AC_SEARCH_LIBS(CBTF_GetPCFromContext, cbtf-services-common[], 
        [ AC_MSG_RESULT(yes)
            AM_CONDITIONAL(HAVE_CBTF_KRELL_SERVICES, true)
            AC_DEFINE(HAVE_CBTF_KRELL_SERVICES, 1,
                      [Define to 1 if you have CBTF Krell services])

        ], [ AC_MSG_RESULT(no)
            #AC_MSG_ERROR([CBTF services library could not be found.])
            AM_CONDITIONAL(HAVE_CBTF_KRELL_SERVICES, false)
            AC_DEFINE(HAVE_CBTF_KRELL_SERVICES, 0,
                      [Define to 1 if you have CBTF Krell services ])
        ]
    )

    CPPFLAGS=$cbtf_krell_saved_CPPFLAGS
    LDFLAGS=$cbtf_krell_saved_LDFLAGS
    LIBS=$cbtf_krell_saved_LIBS

    AC_LANG_POP(C++)

    AC_SUBST(CBTF_KRELL_CPPFLAGS)
    AC_SUBST(CBTF_KRELL_LDFLAGS)
    AC_SUBST(CBTF_KRELL_CORE_LIBS)
    AC_SUBST(CBTF_KRELL_CORE_MRNET_LIBS)
    AC_SUBST(CBTF_KRELL_CORE_SYMTABAPI_LIBS)
    AC_SUBST(CBTF_KRELL_CORE_BFD_LIBS)

    AC_SUBST(CBTF_KRELL_MESSAGES_LIBS)
    AC_SUBST(CBTF_KRELL_MESSAGES_BASE_LIBS)
    AC_SUBST(CBTF_KRELL_MESSAGES_COLLECTOR_LIBS)
    AC_SUBST(CBTF_KRELL_MESSAGES_EVENTS_LIBS)
    AC_SUBST(CBTF_KRELL_MESSAGES_INSTRUMENTATION_LIBS)
    AC_SUBST(CBTF_KRELL_MESSAGES_PERFDATA_LIBS)
    AC_SUBST(CBTF_KRELL_MESSAGES_THREAD_LIBS)

    AC_SUBST(CBTF_KRELL_SERVICES_CPPFLAGS)
    AC_SUBST(CBTF_KRELL_SERVICES_LDFLAGS)
    AC_SUBST(CBTF_KRELL_SERVICES_LIBS)
    AC_SUBST(CBTF_KRELL_SERVICES_BINUTILS_LIBS)
    AC_SUBST(CBTF_KRELL_SERVICES_COMMON_LIBS)
    AC_SUBST(CBTF_KRELL_SERVICES_DATA_LIBS)
    AC_SUBST(CBTF_KRELL_SERVICES_FILEIO_LIBS)
    AC_SUBST(CBTF_KRELL_SERVICES_FPE_LIBS)
    AC_SUBST(CBTF_KRELL_SERVICES_MONITOR_LIBS)
    AC_SUBST(CBTF_KRELL_SERVICES_MRNET_LIBS)
    AC_SUBST(CBTF_KRELL_SERVICES_PAPI_LIBS)
    AC_SUBST(CBTF_KRELL_SERVICES_SEND_LIBS)
    AC_SUBST(CBTF_KRELL_SERVICES_TIMER_LIBS)
    AC_SUBST(CBTF_KRELL_SERVICES_UNWIND_LIBS)

])
