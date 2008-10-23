Summary: Sample Applications For Use In OpenSpeedShop-Live
Name: toyprograms
Version: 0.0.1
Release: 1
License: GPL
Group: Development/Libraries
URL: http://www.openspeedshop.org/
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Prefix: /opt/apps

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
tar -xf smg2000.tar
cd smg2000;make;cd ..

mpicc -g main.c -o nbody-mpi
mpicc -g mm-mpi.c -o mm-mpi

gcc -g forever.c -o forever
gcc -g mutatee.c -o mutatee
g++ -g -lpthread threads.cxx -o threads

%install
mkdir -p $RPM_BUILD_ROOT%{prefix}/smg2000/{krylov,struct_ls,struct_mv,test,utilities}
cd smg2000
install -D -m ugo+rw HYPRE_config.h $RPM_BUILD_ROOT%{prefix}/smg2000/
install -D -m ugo+rw krylov/*.c $RPM_BUILD_ROOT%{prefix}/smg2000/krylov/
install -D -m ugo+rw krylov/*.h $RPM_BUILD_ROOT%{prefix}/smg2000/krylov/
install -D -m ugo+rw krylov/*.a $RPM_BUILD_ROOT%{prefix}/smg2000/krylov/

install -D -m ugo+rw struct_ls/*.c $RPM_BUILD_ROOT%{prefix}/smg2000/struct_ls/
install -D -m ugo+rw struct_ls/*.h $RPM_BUILD_ROOT%{prefix}/smg2000/struct_ls/
install -D -m ugo+rw struct_ls/*.a $RPM_BUILD_ROOT%{prefix}/smg2000/struct_ls/

install -D -m ugo+rw struct_mv/*.c $RPM_BUILD_ROOT%{prefix}/smg2000/struct_mv/
install -D -m ugo+rw struct_mv/*.h $RPM_BUILD_ROOT%{prefix}/smg2000/struct_mv/
install -D -m ugo+rw struct_mv/*.a $RPM_BUILD_ROOT%{prefix}/smg2000/struct_mv/

install -D -m ugo+rw utilities/*.c $RPM_BUILD_ROOT%{prefix}/smg2000/utilities/
install -D -m ugo+rw utilities/*.h $RPM_BUILD_ROOT%{prefix}/smg2000/utilities/
install -D -m ugo+rw utilities/*.a $RPM_BUILD_ROOT%{prefix}/smg2000/utilities/


install -D -m ugo+rw test/*.c $RPM_BUILD_ROOT%{prefix}/smg2000/test/
install -D -m ugo+rwx test/smg2000 $RPM_BUILD_ROOT%{prefix}/smg2000/test/

cd ..

install -D -m ugo+rw main.c $RPM_BUILD_ROOT%{prefix}
install -D -m ugo+rx nbody-mpi $RPM_BUILD_ROOT%{prefix}

install -D -m ugo+rw forever.c $RPM_BUILD_ROOT%{prefix}
install -D -m ugo+rx forever $RPM_BUILD_ROOT%{prefix}

install -D -m ugo+rw mm-mpi.c $RPM_BUILD_ROOT%{prefix}
install -D -m ugo+rx mm-mpi $RPM_BUILD_ROOT%{prefix}

install -D -m ugo+rw mutatee.c $RPM_BUILD_ROOT%{prefix}
install -D -m ugo+rx mutatee $RPM_BUILD_ROOT%{prefix}

install -D -m ugo+rw threads.cxx $RPM_BUILD_ROOT%{prefix}
install -D -m ugo+rx threads $RPM_BUILD_ROOT%{prefix}

%clean
if test x"$RPM_BUILD_ROOT" != x"/"; then
    rm -rf $RPM_BUILD_ROOT
fi

%files
%defattr(-,root,root,-)

%{prefix}/*

%changelog
* Fri Aug 29 2008 Samuel K. Gutierrez <samuel@lanl.gov> - 1.2.6
- Initial creation of RPM specification file.
