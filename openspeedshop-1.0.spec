Summary: A cross platform open source performance tool.
Name: openspeedshop
Version: 1.0
Release: 1
Copyright: GPL
# %{name}-%{version}.tar.gz is found in RPM/SOURCES.
Source:%{name}-%{version}.tar.gz
Obsoletes: openspeedshop openspeedshop-devel
Group: Development/Tools

# build relative to our topdir rather than tmp.
# _topdir is defined in $HOME/.rpmmacros to be
# /data/clink/tools/builds/oss_dev/RPM
# %{_topdir}/../rpminstall ==  $WORKAREA/rpminstall.
BuildRoot:%{_topdir}/../rpminstall

BuildRequires: gcc >= 3.2
BuildRequires: elfutils-libelf >= 0.85
BuildRequires: binutils >= 2.14
BuildRequires: qt >= 3.3.1
# additional BuildRequires go here.
# for some reason, some of the packages fail this test.
# (sqlite, sqlite-devel, tmake, libtool, python, doxygen)
# Others that we may need: libunwind, dpcl, dyninst.
# BuildRequires is what is needed to build the tarball.
# The source rpm file is essentially a tarball within an rpm package.

%description
Open/SpeedShop is a cross platform open source
performance tool.

%package openspeedshop
Summary: A multiplatform performance analysis tool set.
Group: Development/Tools

%description openspeedshop
The openspeedshop package provides a GUI, CLI, and batch 
processing performance tool.

%prep
# show the current build environment for rpmbuild.
env

%setup -q

%build
# openspeedshop gui uses Qt. Set env up for Qt as needed.
# (how do we remove the jardcoded qt version?)
set QTDIR=$ROOT/usr/lib/qt-3.3/
export QTDIR
# show our ROOT, LD_LIBRARY_PATH, PATH for debug purposes.
echo $ROOT
echo $LD_LIBRARY_PATH
echo $PATH
# use $RPM_BUILD_ROOT/usr/local for rpmbuild install target.
# This will be used to populate the binary rpm.
sh configure --prefix=$RPM_BUILD_ROOT/usr/local
# Just build the software. (make all is default).
make

%install
# install into --prefix dir. ($RPM_BUILD_ROOT/usr/local)
make install 

%clean
# comment out for now.
#rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

# installation is in --prefix dir. ($RPM_BUILD_ROOT/usr/local)
%files
# installation attributes for files installed from the rpm packages.
%defattr(-,root,root)
# The following will pickup everything in --prefix/bin
%{_bindir}/*
# The following will pickup everything in --prefix/lib including
# the openspeedshop plugin dir
%{_libdir}/*

%changelog
* Mon Mar 28 2005  Don Maghrak
- Changes for rpmbuild to build from tarball in ${_topdir}/SOURCES and
- package binary and source rpms.
* Mon Feb 21 2005  Jim Galarowicz
- Initial spec file
