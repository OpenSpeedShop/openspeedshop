Summary: Sample Applications For Use In OpenSpeedShop-Live
Name: toyprograms
Version: 0.0.1
Release: 1
License: GPL
Group: Development/Libraries
URL: http://www.openspeedshop.org/
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Prefix: /opt/tempapps

Requires(pre):  coreutils
Requires(preun):  coreutils
Requires(post): coreutils
Requires(postun): coreutils

%description
Sample Applications For Use In OpenSpeedShop-Live

# Do not strip symbols
%define __os_install_post %{nil}
%define __arch_install_post %{nil}
%define debug_package %{nil}
%define __strip /bin/true

%prep
if [ -d %{name}-%{version} ]; then
    rm -fr %{name}-%{version}
fi

%setup -q -n %{name}-%{version}

%build

%install
mkdir -p $RPM_BUILD_ROOT%{prefix}/{mpi,sequential}/smg2000/{krylov,struct_ls,struct_mv,test,utilities}

cd sequential/smg2000
#sequential
install -D -m ugo+rw HYPRE_config.h $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/
install -D -m ugo+rw krylov/*.c $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/krylov/
install -D -m ugo+rw krylov/*.h $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/krylov/
install -D -m ugo+rw krylov/*.a $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/krylov/

install -D -m ugo+rw struct_ls/*.c $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/struct_ls/
install -D -m ugo+rw struct_ls/*.h $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/struct_ls/
install -D -m ugo+rw struct_ls/*.a $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/struct_ls/

install -D -m ugo+rw struct_mv/*.c $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/struct_mv/
install -D -m ugo+rw struct_mv/*.h $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/struct_mv/
install -D -m ugo+rw struct_mv/*.a $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/struct_mv/

install -D -m ugo+rw utilities/*.c $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/utilities/
install -D -m ugo+rw utilities/*.h $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/utilities/
install -D -m ugo+rw utilities/*.a $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/utilities/

install -D -m ugo+rw test/*.c $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/test/
install -D -m ugo+rwx test/smg2000 $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/test/

cd ../..

cd sequential

install -D -m ugo+rw forever.c $RPM_BUILD_ROOT%{prefix}/sequential/forever.c
install -D -m ugo+rwx forever $RPM_BUILD_ROOT%{prefix}/sequential/forever

cd ..

cd mpi/smg2000
#mpi
install -D -m ugo+rw HYPRE_config.h $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/
install -D -m ugo+rw krylov/*.c $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/krylov/
install -D -m ugo+rw krylov/*.h $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/krylov/
install -D -m ugo+rw krylov/*.a $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/krylov/

install -D -m ugo+rw struct_ls/*.c $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/struct_ls/
install -D -m ugo+rw struct_ls/*.h $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/struct_ls/
install -D -m ugo+rw struct_ls/*.a $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/struct_ls/

install -D -m ugo+rw struct_mv/*.c $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/struct_mv/
install -D -m ugo+rw struct_mv/*.h $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/struct_mv/
install -D -m ugo+rw struct_mv/*.a $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/struct_mv/

install -D -m ugo+rw utilities/*.c $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/utilities/
install -D -m ugo+rw utilities/*.h $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/utilities/
install -D -m ugo+rw utilities/*.a $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/utilities/

install -D -m ugo+rw test/*.c $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/test/
install -D -m ugo+rwx test/smg2000 $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/test/

cd ../..

cd mpi/smg2000

%clean
if test x"$RPM_BUILD_ROOT" != x"/"; then
    rm -rf $RPM_BUILD_ROOT
fi

%files
%defattr(-,root,root,-)

%{prefix}/*

%changelog
* Fri Aug 29 2008 Samuel K. Gutierrez <samuel@lanl.gov> - 0.0.1
- Initial creation of RPM specification file.
