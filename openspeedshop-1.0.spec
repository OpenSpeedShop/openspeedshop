Summary: A cross platform open source performance tool.
Name: openspeedshop
Version: 1.0
Release: 1
Copyright: GPL
Source:%{name}-%{version}.tar.gz
Obsoletes: openspeedshop openspeedshop-devel
Group: Development/Tools
BuildRoot:%{_tmppath}/%{name}-root

BuildRequires: gcc >= 3.2
BuildRequires: elfutils-libelf >= 0.85
BuildRequires: binutils >= 2.14
BuildRequires: qt >= 3.3.1
BuildRequires: sqlite >= 3.0.8
BuildRequires: sqlite-devel >= 3.0.8
BuildRequires: tmake >= 1.11
BuildRequires: libtool >= 1.5.2
BuildRequires: python >= 2.3.4
BuildRequires: doxygen >= 1.3.6
# need these in the future
#BuildRequires: libunwind >= 0.96
#BuildRequires: dpcl >= 3.3.4
#BuildRequires: dyninst >= 4.1.1

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
#set OPENSPEEDSHOP_INSTALL_DIR=/scratch/jeg/install
#set ROOT=/perftools/ROOT
#set CPPFLAGS=-I/perftools/ROOT/include
#set LD_LIBRARY_PATH=/perftools/ROOT/lib
set QTDIR=/usr/lib/qt-3.3/
export QTDIR

%setup -q

%build
bootstrap --clean
bootstrap
#sh configure --prefix=$OPENSPEEDSHOP_INSTALL_DIR
sh configure --with-dpcl=/usr --prefix=/home/jeg/install
make

%install
make install 

%clean
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/home/tulip28/jeg/bin/openss

%changelog
* Mon Feb 21 2005  Jim Galarowicz
- Initial spec file
