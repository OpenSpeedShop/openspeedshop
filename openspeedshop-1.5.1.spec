Summary: A cross platform open source performance tool.
Name: openspeedshop
Version: 1.5.1
Release: 1
License: GPL
Source:%{name}-%{version}.tar.gz
Obsoletes: openspeedshop openspeedshop-devel
Group: Development/Tools
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Prefix: /usr


# build relative to our topdir rather than tmp.
# _topdir is defined in $HOME/.rpmmacros to be
# /data/clink/tools/builds/oss_dev/RPM
# %{_topdir}/../rpminstall ==  $WORKAREA/rpminstall.
#BuildRoot:%{_topdir}/../rpminstall

#BuildRequires: gcc >= 3.2
#BuildRequires: elfutils-libelf >= 0.85
#BuildRequires: binutils >= 2.14
#BuildRequires: qt >= 3.3.1
# additional BuildRequires go here.
# for some reason, some of the packages fail this test.
# (sqlite, sqlite-devel, tmake, libtool, python, doxygen)
# Others that we may need: libunwind, dpcl, dyninst.
# BuildRequires is what is needed to build the tarball.
# The source rpm file is essentially a tarball within an rpm package.

%description
Open|SpeedShop is a cross platform open source
performance tool.

%package openspeedshop
Summary: A multiplatform performance analysis tool set.
Group: Development/Tools

%description openspeedshop
The openspeedshop package provides a GUI, CLI, and batch 
processing performance tool.

%prep

# Since %define macros are executed regardless of shell-based conditionals, we have to
# set $OPENSS_PREFIX using the shell-based conditional then %define prefix to what
# we set $OPENSS_PREFIX to be.
if [ $OPENSS_PREFIX ]; then
	echo "- \$OPENSS_PREFIX is defined: setting prefix macro to $OPENSS_PREFIX"
else
	set $OPENSS_PREFIX=/usr
	export OPENSS_PREFIX=/usr
	echo "- \$OPENSS_PREFIX is undefined: prefix macro is %{prefix}"
fi
%define prefix %(echo $OPENSS_PREFIX)

if [ $OPENSS_DEPEND ]; then
	echo "- \$OPENSS_DEPEND is defined: setting prefix macro to $OPENSS_PREFIX"
else
	set OPENSS_DEPEND=%{prefix}
	echo "- \$OPENSS_DEPEND is undefined: prefix macro is %{prefix}"
fi
%define depend_prefix %(echo $OPENSS_DEPEND)

%setup -q



%build

# openspeedshop gui uses Qt. Set env up for Qt as needed.
# (how do we remove the hardcoded qt version?)
if [ $QTDIR ]; then
	echo "- \$QTDIR is defined."
else
	echo "- \$QTDIR is undefined, setting to /usr/share/qt3"
	export QTDIR=/usr/%{_lib}/qt-3.3
fi

./bootstrap

# use $RPM_BUILD_ROOT/usr/for rpmbuild install target.
# This will be used to populate the binary rpm.
./configure --prefix=$RPM_BUILD_ROOT/%{prefix} --with-binutils=%{depend_prefix} --with-binutils-version=2.17 --with-libunwind=%{depend_prefix} --with-papi=%{depend_prefix} --with-sqlite=%{depend_prefix} --with-qtlib=$QTDIR --with-dyninst=%{depend_prefix} --with-dpcl=%{depend_prefix} --with-lampi=/opt/lampi/lampi-1.5.16/gm 

# Just build the software. (make all is default).
make



%install

# install into --prefix directory set by ./configure
make install 

rm -f $RPM_BUILD_ROOT%{prefix}/lib/libltdl*
rm -f $RPM_BUILD_ROOT%{prefix}/include/ltdl.h

%clean
# comment out for now.
#rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig
rm -rf %{prefix}/share/doc/packages/OpenSpeedShop


# installation is in --prefix dir. ($RPM_BUILD_ROOT/usr/
%files
#
# The %docdir directive is used to add a directory
# to the list of directories that will contain documentation.
#%docdir /usr/share/doc/%{name}-%{version}-%{release}
#
%docdir %{prefix}/share/doc/packages/OpenSpeedShop
%{prefix}/share/doc/packages/OpenSpeedShop

# installation attributes for files installed from the rpm packages.
%defattr(-,root,root)
# The following will pickup everything in --prefix/bin
%{prefix}/bin/*
# The following will pickup everything in --prefix/lib including
# the openspeedshop plugin dir
%{prefix}/%{_lib}/*

%changelog
* Thu Nov 29 2007 Jim Galarowicz <jeg@krellinst.org>
- Changes for 1.5.1 release and removing ltdl after make install
* Wed Nov 7 2007 Jim Galarowicz <jeg@krellinst.org>
- Changes for 1.5 release
* Thu Nov 1 2007 Jim Galarowicz <jeg@krellinst.org>
- Changes for 1.50rc1
* Tue Jun 19 2007 Matt Brock <mvbrock@gmail.com>
- Added prefix macro to the install locations.
- Added ./configure options for specifying binutils, libunwind,
- Replaced LIBDIR with _lib macro
PAPI, sqlite, qtlib, dyninst, and DPCL directories.
* Thu Nov 02 2006 Jim Galarowicz
- Initial 1.00 spec file
