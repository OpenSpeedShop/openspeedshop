Summary: A powerful implementaion of MPI 
Name: openmpi
Version: 1.2.6
Release: 1
License: BSD
Group: Development/Libraries
URL: http://www.open-mpi.org/
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Prefix: /opt/openmpi

%description
Open MPI is a project combining technologies and resources from
several other projects (FT-MPI, LA-MPI, LAM/MPI, and PACX-MPI) in
order to build the best MPI library available.

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

./configure --prefix=%{prefix} \
--libdir=%{prefix}/%{_lib}

%build
make

%install
make DESTDIR=%{buildroot} install

%clean
if test x"$RPM_BUILD_ROOT" != x"/"; then
    rm -rf $RPM_BUILD_ROOT
fi

%files
%defattr(-,root,root,-)

%{prefix}/bin/*
%{prefix}/%{_lib}/*
%{prefix}/include/*
%{prefix}/etc/*
%{prefix}/share/*

%changelog
* Fri Aug 29 2008 Samuel K. Gutierrez <samuel@lanl.gov> - 1.2.6
- Initial creation of RPM specification file.
