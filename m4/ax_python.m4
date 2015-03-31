################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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

################################################################################
# Check for PYTHON libraries
# Some of this code was found by an internet search for python configuration/
# checking, so credit to those who originally created parts or all
# of the code below.
################################################################################
dnl @synopsis AX_PYTHON_DEVEL([version])
dnl
dnl Checks for Python and tries to get the include path to 'Python.h'.
dnl It provides the $(PYTHON_CPPFLAGS) and $(PYTHON_LDFLAGS) output
dnl variables. Also exports $(PYTHON_EXTRA_LIBS) and
dnl $(PYTHON_EXTRA_LDFLAGS) for embedding Python in your code.
dnl
dnl You can search for some particular version of Python by passing a
dnl parameter to this macro, for example ">= '2.3.1'", or "== '2.4'".
dnl Please note that you *have* to pass also an operator along with the
dnl version to match, and pay special attention to the single quotes
dnl surrounding the version number.
dnl
dnl If the user wants to employ a particular version of Python, she can
dnl now pass to configure the PYTHON_VERSION environment variable. This
dnl is only limited by the macro parameter set by the packager.
dnl
dnl This macro should work for all versions of Python >= 2.1.0. You can
dnl disable the check for the python version by setting the
dnl PYTHON_NOVERSIONCHECK environment variable to something else than
dnl the empty string.
dnl
dnl If you need to use this macro for an older Python version, please
dnl contact the authors. We're always open for feedback.
dnl
dnl @category InstalledPackages
dnl @author Sebastian Huber <sebastian-huber@web.de>
dnl @author Alan W. Irwin <irwin@beluga.phys.uvic.ca>
dnl @author Rafael Laboissiere <laboissiere@psy.mpg.de>
dnl @author Andrew Collier <colliera@nu.ac.za>
dnl @author Matteo Settenvini <matteo@member.fsf.org>
dnl @version 2006-02-05
dnl @license GPLWithACException

AC_DEFUN([AX_PYTHON_DEVEL],[
        #
        # Allow the use of a (user set) custom python version
        #
        AC_ARG_VAR([PYTHON_VERSION],[The installed Python
                version to use, for example '2.3'. This string
                will be appended to the Python interpreter
                canonical name.])

dnl Support the user enabled --with-python option.
    AC_MSG_CHECKING(for --with-python)
    AC_ARG_WITH(python,
                AC_HELP_STRING([--with-python=PYTHON],
                               [path name of Python libraries]),
                [ if test ! -d "$withval" 
                  then
                    python_dir="/usr"
                    AC_MSG_RESULT("/usr")
                  else
                    PYTHON="$with_python/bin/python"
                    python_dir="$with_python"
                    AC_MSG_RESULT($with_python)
                  fi
                ],
                [ AC_MSG_RESULT(no)
                ])

        AC_PATH_PROG([PYTHON],[python[$PYTHON_VERSION]])
        if test -z "$PYTHON"; then
           AC_MSG_ERROR([Cannot find python$PYTHON_VERSION in your system path])
        fi

    # jeg added this - if using an alternative python installation, we need to point at the libraries
    python_saved_LDFLAGS=$LDFLAGS
    LDFLAGS="$LDFLAGS -L$python_dir/$abi_libdir -L$python_dir/$alt_abi_libdir"

    #
    # if the macro parameter ``version'' is set, honour it
    #
    if test -n "$1"; then
            AC_MSG_CHECKING([for a version of Python $1])
            ac_supports_python_ver=`$PYTHON -c "import sys, string; \
                    ver = string.split(sys.version)[[0]]; \
                    print ver $1"`
            if test "$ac_supports_python_ver" = "True"; then
               AC_MSG_RESULT([yes])
            else
                    AC_MSG_RESULT([no])
                    AC_MSG_ERROR([this package requires Python $1. 
If you have it installed, but it isn't the default Python 
interpreter in your system path, please pass the PYTHON_VERSION 
variable to configure. See ``configure --help'' for reference. ])
            fi
    fi

    #
    # Check if you have distutils, else fail
    #
    AC_MSG_CHECKING([for the distutils Python package])
    ac_distutils_result=`$PYTHON -c "import distutils" 2>&1`
    if test -z "$ac_distutils_result"; then
            AC_MSG_RESULT([yes])
    else
            AC_MSG_RESULT([no])
            AC_MSG_ERROR([cannot import Python module "distutils".
Please check your Python installation. The error was:
$ac_distutils_result])
    fi

    #
    # Check for Python include path
    #
    AC_MSG_CHECKING([for Python include path])
    if test -z "$PYTHON_CPPFLAGS"; then
            python_path=`$PYTHON -c "import distutils.sysconfig; \
                    print distutils.sysconfig.get_python_inc();"`
            if test -n "${python_path}"; then
                    python_path="-I$python_path"
            fi
            PYTHON_CPPFLAGS=$python_path
    fi
    AC_MSG_RESULT([$PYTHON_CPPFLAGS])
    AC_SUBST([PYTHON_CPPFLAGS])

    #
    # Check for Python library path
    #
    AC_MSG_CHECKING([for Python library path])
    if test -z "$PYTHON_LDFLAGS"; then
            # (makes two attempts to ensure we've got a version number
            # from the interpreter)
            py_version=`$PYTHON -c "from distutils.sysconfig import *; \
                    from string import join; \
                    print join(get_config_vars('VERSION'))"`
            if test "$py_version" == "[None]"; then
                    if test -n "$PYTHON_VERSION"; then
                            py_version=$PYTHON_VERSION
                    else
                            py_version=`$PYTHON -c "import sys; \
                                    print sys.version[[:3]]"`
                    fi
            fi

            python_libpath_tmp=`$PYTHON -c "from distutils.sysconfig import *; \
                    from string import join; \
                    print get_python_lib(0,1)"`;

            libpyvers="python$py_version";
            PYTHON_LIBPATH=`echo $python_libpath_tmp | sed "s?$libpyvers??"` ;

            so_designator=".so"
            libpython_designator="libpython"
            if test -f $PYTHON_LIBPATH/$libpython_designator$py_version$so_designator; then
              PYTHON_LDFLAGS="-L$PYTHON_LIBPATH"
              PYTHON_LIBS="-lpython$py_version"
            elif test -f $PYTHON_LIBPATH/x86_64-linux-gnu/$libpython_designator$py_version$so_designator; then
              PYTHON_LDFLAGS="-L$PYTHON_LIBPATH/x86_64-linux-gnu"
              PYTHON_LIBS="-lpython$py_version"
            else
              PYTHON_LDFLAGS="-L$PYTHON_LIBPATH"
              PYTHON_LIBS="-lpython$py_version"
            fi

            #PYTHON_LDFLAGS_TMP=`$PYTHON -c "from distutils.sysconfig import *; \
            #        from string import join; \
            #        print '-L' + get_python_lib(0,1), \
            #        '-lpython';"`$py_version

            PYTHON_LIBRARY_NAME="libpython$py_version$so_designator"

            if test -f $PYTHON_LIBPATH/$libpython_designator$py_version$so_designator; then
              python_full_path_tmp1=$PYTHON_LIBPATH/$libpython_designator$py_version
            elif test -f $PYTHON_LIBPATH/x86_64-linux-gnu/$libpython_designator$py_version$so_designator; then
              python_full_path_tmp1=$PYTHON_LIBPATH/x86_64-linux-gnu/$libpython_designator$py_version
            else
              python_full_path_tmp1=$PYTHON_LIBPATH/$libpython_designator$py_version
            fi
            #python_full_path_tmp1=`$PYTHON -c "from distutils.sysconfig import *; \
            #        from string import join; \
            #        print get_python_lib(0,1), " | sed 's?python2?libpython2?'`;
            AC_SUBST([python_full_path_tmp1])
            PYTHON_FULL_PATH_LIBRARY_NAME="${python_full_path_tmp1}$so_designator"

    fi

    if test "x${python_dir}" == "x" || test "x${python_dir}" == "x/usr" ; then
      PYTHON_LDFLAGS=""
    fi


    AC_MSG_RESULT([$PYTHON_LDFLAGS])
    AC_SUBST([PYTHON_LDFLAGS_TMP])
    AC_SUBST([PYTHON_LDFLAGS])
    AC_SUBST([PYTHON_LIBS])
    AC_SUBST([PYTHON_LIBPATH])
    AC_DEFINE_UNQUOTED(PYTHON_FP_LIB_NAME, "$PYTHON_FULL_PATH_LIBRARY_NAME",
                           [Name of full path python dynamic library])
    AC_DEFINE_UNQUOTED(PYTHON_LIB_NAME, "$PYTHON_LIBRARY_NAME",
                           [Name of python dynamic library])

    AC_SUBST([PYTHON_LIBRARY_NAME])
    AC_SUBST([PYTHON_FULL_PATH_LIBRARY_NAME])

    #
    # Check for site packages
    #
    AC_MSG_CHECKING([for Python site-packages path])
    if test -z "$PYTHON_SITE_PKG"; then
            PYTHON_SITE_PKG=`$PYTHON -c "import distutils.sysconfig; \
                    print distutils.sysconfig.get_python_lib(0,0);"`
    fi
    AC_MSG_RESULT([$PYTHON_SITE_PKG])
    AC_SUBST([PYTHON_SITE_PKG])

    #
    # libraries which must be linked in when embedding
    #
    AC_MSG_CHECKING(python extra libraries)
    if test -z "$PYTHON_EXTRA_LIBS"; then
       PYTHON_EXTRA_LIBS=`$PYTHON -c "import distutils.sysconfig; \
            conf = distutils.sysconfig.get_config_var; \
            print conf('LOCALMODLIBS'), conf('LIBS')"`
    fi
    AC_MSG_RESULT([$PYTHON_EXTRA_LIBS])
    AC_SUBST(PYTHON_EXTRA_LIBS)

    #
    # linking flags needed when embedding
    #
    AC_MSG_CHECKING(python extra linking flags)
    if test -z "$PYTHON_EXTRA_LDFLAGS"; then
            PYTHON_EXTRA_LDFLAGS=`$PYTHON -c "import distutils.sysconfig; \
                    conf = distutils.sysconfig.get_config_var; \
                    print conf('LINKFORSHARED')"`
    fi
    AC_MSG_RESULT([$PYTHON_EXTRA_LDFLAGS])
    AC_SUBST(PYTHON_EXTRA_LDFLAGS)
    LDFLAGS=$python_saved_LDFLAGS
    CPPFLAGS=$python_saved_CPPFLAGS

    python_saved_CPPFLAGS=$CPPFLAGS
    python_saved_LDFLAGS=$LDFLAGS
    python_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $PYTHON_CPPFLAGS"
    LDFLAGS="$LDFLAGS $PYTHON_LDFLAGS"
    LIBS="$PYTHON_LIBS"

    #
    # Check for a version of Python >= 2.1.0
    #
    AC_MSG_CHECKING([for a version of Python >= '2.1.0'])
    ac_supports_python_ver=`$PYTHON -c "import sys, string; \
            ver = string.split(sys.version)[[0]]; \
            print ver >= '2.1.0'"`

    if test "$ac_supports_python_ver" != "True"; then
                    AC_MSG_FAILURE([
This version of the PYTHON_DEVEL macro
doesn't work properly with versions of Python before
2.1.0. You may need to install a newer version of Python.
])
    else
            AC_MSG_RESULT([yes])
    fi

    CPPFLAGS=$python_saved_CPPFLAGS
    LDFLAGS=$python_saved_LDFLAGS
    LIBS=$python_saved_LIBS

])
