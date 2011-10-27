Summary: Sample Applications For Use In OpenSpeedShop-Live
Name: toyprograms
Version: 0.0.1
Release: 4
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
#mkdir -p $RPM_BUILD_ROOT%{prefix}/databases
#cd databases
#install -D -m ugo+rw *.openss $RPM_BUILD_ROOT%{prefix}/databases
#cd ..

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


install -D -m ugo+rw test/README.txt $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/test/
install -D -m ugo+rw test/*.c $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/test/
install -D -m ugo+rwx test/smg2000 $RPM_BUILD_ROOT%{prefix}/sequential/smg2000/test/

cd ../..
mkdir -p $RPM_BUILD_ROOT%{prefix}/sequential/forever
cd sequential/forever

install -D -m ugo+rw forever.c $RPM_BUILD_ROOT%{prefix}/sequential/forever/forever.c
install -D -m ugo+rwx buildit $RPM_BUILD_ROOT%{prefix}/sequential/forever/buildit
install -D -m ugo+rwx runit $RPM_BUILD_ROOT%{prefix}/sequential/forever/runit
install -D -m ugo+rwx forever $RPM_BUILD_ROOT%{prefix}/sequential/forever/forever

mkdir -p $RPM_BUILD_ROOT%{prefix}/sequential/threads
cd ../..
cd sequential/threads

install -D -m ugo+rw threads.cxx $RPM_BUILD_ROOT%{prefix}/sequential/threads/threads.cxx
install -D -m ugo+rwx buildit $RPM_BUILD_ROOT%{prefix}/sequential/threads/buildit
install -D -m ugo+rwx runit $RPM_BUILD_ROOT%{prefix}/sequential/threads/runit
install -D -m ugo+rwx threads $RPM_BUILD_ROOT%{prefix}/sequential/threads/threads


mkdir -p $RPM_BUILD_ROOT%{prefix}/sequential/mutatee
cd ../..
cd sequential/mutatee

install -D -m ugo+rw mutatee.c $RPM_BUILD_ROOT%{prefix}/sequential/mutatee/mutatee.c
install -D -m ugo+rwx buildit $RPM_BUILD_ROOT%{prefix}/sequential/mutatee/buildit
install -D -m ugo+rwx runit $RPM_BUILD_ROOT%{prefix}/sequential/mutatee/runit
install -D -m ugo+rwx mutatee $RPM_BUILD_ROOT%{prefix}/sequential/mutatee/mutatee

mkdir -p $RPM_BUILD_ROOT%{prefix}/sequential/matmul
mkdir -p $RPM_BUILD_ROOT%{prefix}/sequential/matmul/orig
mkdir -p $RPM_BUILD_ROOT%{prefix}/sequential/matmul/modified

cd ../..
cd sequential/matmul/orig

install -D -m ugo+rw matmul.f90 $RPM_BUILD_ROOT%{prefix}/sequential/matmul/orig/matmul.f90
install -D -m ugo+rw matmul_input.txt $RPM_BUILD_ROOT%{prefix}/sequential/matmul/orig/matmul_input.txt
install -D -m ugo+rwx buildit $RPM_BUILD_ROOT%{prefix}/sequential/matmul/orig/buildit
install -D -m ugo+rwx runit $RPM_BUILD_ROOT%{prefix}/sequential/matmul/orig/runit
install -D -m ugo+rwx matmul $RPM_BUILD_ROOT%{prefix}/sequential/matmul/orig/matmul

cd ../../..
cd sequential/matmul/modified

install -D -m ugo+rw matmul.f90 $RPM_BUILD_ROOT%{prefix}/sequential/matmul/modified/matmul.f90
install -D -m ugo+rw matmul_input.txt $RPM_BUILD_ROOT%{prefix}/sequential/matmul/modified/matmul_input.txt
install -D -m ugo+rwx buildit $RPM_BUILD_ROOT%{prefix}/sequential/matmul/modified/buildit
install -D -m ugo+rwx runit $RPM_BUILD_ROOT%{prefix}/sequential/matmul/modified/runit

cd ../../..
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

install -D -m ugo+rw test/README.txt $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/test/
install -D -m ugo+rw test/*.c $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/test/
install -D -m ugo+rwx test/smg2000 $RPM_BUILD_ROOT%{prefix}/mpi/smg2000/test/

cd ../..
mkdir -p $RPM_BUILD_ROOT%{prefix}/mpi/nbody
cd mpi/nbody

install -D -m ugo+rw *.c $RPM_BUILD_ROOT%{prefix}/mpi/nbody/
install -D -m ugo+rwx nbody $RPM_BUILD_ROOT%{prefix}/mpi/nbody/

cd ../..
mkdir -p $RPM_BUILD_ROOT%{prefix}/mpi/LU
cd mpi/LU

install -D -m ugo+rw README.txt $RPM_BUILD_ROOT%{prefix}/mpi/LU/
install -D -m ugo+rw *.f $RPM_BUILD_ROOT%{prefix}/mpi/LU/
install -D -m ugo+rw *.openss $RPM_BUILD_ROOT%{prefix}/mpi/LU/
install -D -m ugo+rwx Makefile $RPM_BUILD_ROOT%{prefix}/mpi/LU/
install -D -m ugo+rwx *.sample $RPM_BUILD_ROOT%{prefix}/mpi/LU/


cd ../..
mkdir -p $RPM_BUILD_ROOT%{prefix}/multi/openmp_stress
cd multi/openmp_stress

install -D -m ugo+rw *.f $RPM_BUILD_ROOT%{prefix}/multi/openmp_stress
install -D -m ugo+rwx stress_omp  $RPM_BUILD_ROOT%{prefix}/multi/openmp_stress
install -D -m ugo+rw stress.input $RPM_BUILD_ROOT%{prefix}/multi/openmp_stress
install -D -m ugo+rw README.omp_stress $RPM_BUILD_ROOT%{prefix}/multi/openmp_stress
install -D -m ugo+rwx buildit $RPM_BUILD_ROOT%{prefix}/multi/openmp_stress

cd ../..
cd hybrid
mkdir -p $RPM_BUILD_ROOT%{prefix}/hybrid
install -D -m ugo+rw README.txt $RPM_BUILD_ROOT%{prefix}/hybrid
install -D -m ugo+rw *.f $RPM_BUILD_ROOT%{prefix}/hybrid
install -D -m ugo+rw *.openss $RPM_BUILD_ROOT%{prefix}/hybrid


cd ../..

%clean
if test x"$RPM_BUILD_ROOT" != x"/"; then
    rm -rf $RPM_BUILD_ROOT
fi

%files
%defattr(-,root,root,-)

%{prefix}/*

%changelog
* Thu Oct 27 2011 Jim Galarowicz <jeg@krellinst.org> - 0.0.1
- Add more README information for users.  - 4
* Tue Oct 17 2011 Jim Galarowicz <jeg@krellinst.org> - 0.0.1
- Add hybrid directory for hybrid NPB BT  - 3
* Thu Oct 7 2010 Jim Galarowicz <jeg@krellinst.org> - 0.0.1
- Add multi directory for openmp and mpi/LU  - 2
* Fri Jul 9 2010 Jim Galarowicz <jeg@krellinst.org> - 0.0.1
- Add database directory for important database files
* Tue Aug 4 2009 Jim Galarowicz <jeg@krellinst.org> - 0.0.1
- Add more test programs
* Fri Aug 29 2008 Samuel K. Gutierrez <samuel@lanl.gov> - 0.0.1
- Initial creation of RPM specification file.
