Summary: A cross platform open source performance tool.
Name: openspeedshop
Version: 0.875
Release: 1
License: GPL
# %{name}-%{version}.tar.gz is found in RPM/SOURCES.
Source:%{name}-%{version}.tar.gz
Obsoletes: openspeedshop openspeedshop-devel
Group: Development/Tools

# build relative to our topdir rather than tmp.
# _topdir is defined in $HOME/.rpmmacros to be
# /data/clink/tools/builds/oss_dev/RPM
# %{_topdir}/../rpminstall ==  $WORKAREA/rpminstall.
BuildRoot:%{_topdir}/../rpminstall

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
# show the current build environment for rpmbuild.
# show the current build environment for rpmbuild.
%ifarch x86_64 x86-64
    %define LIBDIR /usr/local/lib64
%else
    %define LIBDIR /usr/local/lib
%endif
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

rm -rf $RPM_BUILD_ROOT/usr/local/doc
mkdir $RPM_BUILD_ROOT/usr/local/doc
rm -rf $RPM_BUILD_ROOT/usr/local/doc/openspeedshop
mkdir $RPM_BUILD_ROOT/usr/local/doc/openspeedshop
rm -rf $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/users_guide
mkdir $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/users_guide
rm -rf $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide
mkdir $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide

cd doc/users_guide
cp -rf * $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/users_guide/.
rm -f $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/users_guide/Makefile
rm -f $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/users_guide/Makefile.am
rm -f $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/users_guide/Makefile.in

cd ../../doc/users_guide/TroubleShootingGuide
cp -rf * $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide/.
rm -f $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide/Makefile
rm -f $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide/Makefile.am
rm -f $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide/Makefile.in

rm -rf $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/cli_doc
mkdir $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/cli_doc
cd ../../cli_doc
cp -rf * $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/cli_doc/.
rm -f $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/cli_doc/Makefile
rm -f $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/cli_doc/Makefile.am
rm -f $RPM_BUILD_ROOT/usr/local/doc/openspeedshop/cli_doc/Makefile.in
cd ../..

%clean
# comment out for now.
#rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

# The %docdir directive is used to add a directory
# to the list of directories that will contain documentation.
# RPM includes the directories /usr/doc, /usr/info, and /usr/man
# in the %docdir list by default.
%docdir /usr/local/doc/openspeedshop

# installation is in --prefix dir. ($RPM_BUILD_ROOT/usr/local)
%files
/usr/local/doc/openspeedshop/users_guide/BuildingInstallingDPCL
/usr/local/doc/openspeedshop/users_guide/BuildingInstallingDPCL.html
/usr/local/doc/openspeedshop/users_guide/FileMenu.png
/usr/local/doc/openspeedshop/users_guide/HelpMenu.png
/usr/local/doc/openspeedshop/users_guide/index.html
/usr/local/doc/openspeedshop/users_guide/InitialPanel.png
/usr/local/doc/openspeedshop/users_guide/IntroWizardRightMouseDown.png
/usr/local/doc/openspeedshop/users_guide/OpenSpeedShop_Logo.jpg
/usr/local/doc/openspeedshop/users_guide/OpenSpeedShopPackageNotes.html
/usr/local/doc/openspeedshop/users_guide/PC_SampleExperiment-2.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleExperiment-3-RESULTS.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleExperiment-4-Split.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleExperiment-5-f3click.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleExperiment-non-wizard-1.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleExperiment-non-wizard-2.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleExperiment-non-wizard-3.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleExperiment-non-wizard-4.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleExperiment-non-wizard-5.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleExperiment-non-wizard-6.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleExperiment-non-wizard-7.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleWizard-AttachToRunningProcess-fred1.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleWizard-AttachToRunningProcess-fred.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleWizard-AttachToRunningProcess-GUI-nonwizard1.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleWizard-AttachToRunningProcess-GUI-nonwizard.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleWizard-AttachToRunningProcess-MPIJOB1.png
/usr/local/doc/openspeedshop/users_guide/PC_SampleWizard-AttachToRunningProcess.png
/usr/local/doc/openspeedshop/users_guide/PCSampling-StatsPanel.png
/usr/local/doc/openspeedshop/users_guide/PCSampling-StatsPanel-Split.png
/usr/local/doc/openspeedshop/users_guide/PC_SamplingWizard-2.png
/usr/local/doc/openspeedshop/users_guide/PC_SamplingWizard-3A.png
/usr/local/doc/openspeedshop/users_guide/PC_SamplingWizard-3.png
/usr/local/doc/openspeedshop/users_guide/PC_SamplingWizard-4.png
/usr/local/doc/openspeedshop/users_guide/PC_SamplingWizard-5.png
/usr/local/doc/openspeedshop/users_guide/PC_SamplingWizard-6_OR_PC_SampleExperiment-1.png
/usr/local/doc/openspeedshop/users_guide/Preferences-General.png
/usr/local/doc/openspeedshop/users_guide/Preferences-Source.png
/usr/local/doc/openspeedshop/users_guide/Preferences-Stats.png
/usr/local/doc/openspeedshop/users_guide/rpmQuery.asc
/usr/local/doc/openspeedshop/users_guide/snapshot_ExperimentsMenuItems.png
/usr/local/doc/openspeedshop/users_guide/snapshot_WizardsMenuItems.png
/usr/local/doc/openspeedshop/users_guide/SourcePanel.png
/usr/local/doc/openspeedshop/users_guide/StatsPanel.png
/usr/local/doc/openspeedshop/users_guide/ToolsMenu.png
/usr/local/doc/openspeedshop/users_guide/valid-html401.png
/usr/local/doc/openspeedshop/cli_doc/OpenSpeedShop_Logo.jpg
/usr/local/doc/openspeedshop/cli_doc/README
/usr/local/doc/openspeedshop/cli_doc/definition.html
/usr/local/doc/openspeedshop/cli_doc/fields.html
/usr/local/doc/openspeedshop/cli_doc/index.html
/usr/local/doc/openspeedshop/cli_doc/invocation.html
/usr/local/doc/openspeedshop/cli_doc/overview.html
/usr/local/doc/openspeedshop/cli_doc/s1.html
/usr/local/doc/openspeedshop/cli_doc/s2.html
/usr/local/doc/openspeedshop/cli_doc/s3.html
/usr/local/doc/openspeedshop/cli_doc/summary.html
/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide/doxygen.css
/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide/doxygen.png
/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide/files.html
/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide/globals.html
/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide/globals_func.html
/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide/globals_vars.html
/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide/index.html
/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide/mainpage_8hxx-source.html
/usr/local/doc/openspeedshop/users_guide/TroubleShootingGuide/mainpage_8hxx.html

# installation attributes for files installed from the rpm packages.
%defattr(-,root,root)
# The following will pickup everything in --prefix/bin
%{_bindir}/*
# The following will pickup everything in --prefix/lib including
# the openspeedshop plugin dir
%{LIBDIR}/*

%changelog
* Sat Mar 25 2006  Jim Galarowicz
- Change version to 0.875 to match the milestone 7 release
* Tue Feb 14 2006  Jim Galarowicz
- Change version to 0.75 to match the milestone 6 release
* Fri Nov 11 2005  Don Maghrak
- libdir is lib64 for Operton and EMT64...
* Thu Nov 10 2005  Jim Galarowicz
- Change version to 0.5 for the developers beta
* Mon Mar 28 2005  Don Maghrak
- Changes for rpmbuild to build from tarball in ${_topdir}/SOURCES and
- package binary and source rpms.
* Mon Feb 21 2005  Jim Galarowicz
- Initial spec file
