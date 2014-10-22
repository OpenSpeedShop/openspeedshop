#!/bin/bash
echo "..........................................."
echo "Running setup_and_build_rpms.sh...."
echo "..........................................."
################################################################################

MPIWRAPPER=$(which mpicc)
echo "MPIWRAPPER=${MPIWRAPPER}"
if [[ "${MPIWRAPPER}" == "" ]] ; then
  echo "..........................................."
  echo "Do you have the MPI module loaded that you want to build the MPI test programs with? ... "
  echo "..........................................."
  echo "Please load the module and restart (nothing has been copied, so no cvs update is required - answer with a 'y' or 'Y' to exit and restart."
  read answer
#
  if [ "$answer" = Y -o "$answer" = y ]; then
     echo "..........................................."
     echo "Exiting the build ....."
     echo "..........................................."
     exit
  else
     echo "..........................................."
     echo "Exiting the build ....."
     echo "..........................................."
     exit
  fi
fi

rm -rf SPECS SOURCES RPMS toyprograms-0.0.1 webtutorial-0.0.1

echo "..........................................."
echo "Copying specs to SPECS....."
echo "..........................................."
mkdir SPECS
cp -r specs/* SPECS/.
echo "..........................................."
echo "Moving toyprograms to toyprograms-0.0.1....."
echo "..........................................."
mv toyprograms toyprograms-0.0.1
chmod -R 755 toyprograms-0.0.1
echo "..........................................."
echo "Moving web-tutorials to webtutorial-0.0.1....."
echo "..........................................."
mv web-tutorials webtutorial-0.0.1
chmod -R 755 webtutorial-0.0.1
echo "..........................................."
echo "mkdir SOURCES....."
echo "..........................................."
mkdir SOURCES
echo "..........................................."
echo "Building executables in toyprograms-0.0.1....."
echo "..........................................."
cd toyprograms-0.0.1
cd sequential/forever
chmod 755 buildit
echo "..........................................."
echo "Building forever....."
echo "..........................................."
./buildit
cd ../mutatee
chmod 755 buildit
echo "..........................................."
echo "Building mutatee....."
echo "..........................................."
./buildit
cd ../threads
chmod 755 buildit
echo "..........................................."
echo "Building threads....."
echo "..........................................."
./buildit
cd ../matmul/orig
chmod 755 buildit
echo "..........................................."
echo "Building matmul/orig....."
echo "..........................................."
./buildit
cd ../modified
chmod 755 buildit
echo "..........................................."
echo "Building matmul/modified....."
echo "..........................................."
./buildit
cd ../../smg2000
echo "..........................................."
echo "Building sequential version of smg2000....."
echo "..........................................."
make
echo "..........................................."
echo "Building mpi version of smg2000....."
echo "..........................................."
#
#
cd ../../mpi/smg2000
make
cd ../nbody
chmod 755 buildit
echo "..........................................."
echo "Building mpi version of nbody....."
echo "..........................................."
./buildit

cd ../../multi/openmp_stress
chmod 755 buildit
echo "..........................................."
echo "Building openmp_stress....."
echo "..........................................."
./buildit

cd ../../..

#cd ../../databases
#touch empty.openss
#
#echo "..........................................."
#echo "Do you want to add more database files to the toyprograms-0.0.1/databases directory ... "
#echo "..........................................."
#echo "Please place the file(s) into the toyprograms-0.0.1/databases  and answer with a 'y' or 'Y' to continue."
#read answer
#
#if [ "$answer" = Y -o "$answer" = y ]; then
#   echo "..........................................."
#   echo "Continuing the build ....."
#   echo "..........................................."
#fi
#
#cd ../..
#
pwd

echo "..........................................."
echo "Creating tarball for toyprograms-0.0.1....."
echo "..........................................."
chmod -R 755 toyprograms-0.0.1
tar -cvf toyprograms-0.0.1.tar toyprograms-0.0.1/*
gzip toyprograms-0.0.1.tar
mv toyprograms-0.0.1.tar.gz SOURCES/.
echo "..........................................."
echo "Creating tarball for webtutorial-0.0.1....."
echo "..........................................."
#
echo "..........................................."
echo "Do you want to add more files to the webtutorial-0.0.1 sub-directories? ... "
echo "..........................................."
echo "Please place the file(s) into the webtutorial-0.0.1/...  and answer with a 'y' or 'Y' to continue."
read answer

if [ "$answer" = Y -o "$answer" = y ]; then
   echo "..........................................."
   echo "Continuing the build ....."
   echo "..........................................."
fi
#
chmod -R 755 webtutorial-0.0.1
tar -cvf webtutorial-0.0.1.tar webtutorial-0.0.1/*
gzip webtutorial-0.0.1.tar
mv webtutorial-0.0.1.tar.gz SOURCES/.

echo ".........................................................."
echo "Using install.sh to reset .rpmmacros to this location....."
echo ".........................................................."
./install.sh

echo "..........................................."
echo "Building RPM for webtutorial-0.0.1....."
echo "..........................................."
./Build-RPM webtutorial-0.0.1
echo "..........................................."
echo "Listing the location RPM for webtutorial-0.0.1....."
echo "..........................................."
if [ "$(uname -i)" = "i386" ]; then
  ls -lastr RPMS/localhost.localdomain/webtutorial.OSS.i686.rpm
elif [ "$(uname -i)" = "x86_64" ]; then
  ls -lastr RPMS/localhost.localdomain/webtutorial.OSS.x86_64.rpm
fi


echo "..........................................."
echo "Building RPM for toyprograms-0.0.1....."
echo "..........................................."
./Build-RPM toyprograms-0.0.1
echo "..........................................."
echo "Listing the location RPM for toyprograms-0.0.1....."
echo "..........................................."
if [ "$(uname -i)" = "i386" ]; then
  ls -lastr RPMS/localhost.localdomain/toyprograms.OSS.i686.rpm
elif [ "$(uname -i)" = "x86_64" ]; then
  ls -lastr RPMS/localhost.localdomain/toyprograms.OSS.x86_64.rpm
fi

#echo "..........................................."
#echo "Checking for SOURCES/openmpi-1.8.2.tar.gz....."
#echo "..........................................."
#
#if [ -f SOURCES/openmpi-1.8.2.tar.gz ]; then
#     echo "..........................................."
#     echo "Building RPM for openmpi-1.8.2....."
#     echo "..........................................."
#     ./Build-RPM openmpi-1.8.2
#else
#     echo "No SOURCES/openmpi-1.8.2.tar.gz file was found."
#     echo "Please place the file into SOURCES/openmpi-1.8.2.tar.gz and answer with a 'y' or 'Y' to continue."
#     read answer
#
#     if [ "$answer" = Y -o "$answer" = y ]; then
#        echo "..........................................."
#        echo "Building RPM for openmpi-1.8.2....."
#        echo "..........................................."
#        ./Build-RPM openmpi-1.8.2
#     else
#        echo "..........................................."
#        echo "Skipping the build of the RPM for openmpi-1.8.2....."
#        echo "..........................................."
#     fi
#fi

#echo "..........................................."
#echo "Listing the location RPM for openmpi-1.n.m....."
#echo "..........................................."
#
## only works on x86, x86_64
#if [ "$(uname -i)" = "i386" ]; then
# ls -lastr RPMS/localhost.localdomain/openmpi.OSS.i686.rpm
#elif [ "$(uname -i)" = "x86_64" ]; then
#  ls -lastr RPMS/localhost.localdomain/openmpi.OSS.x86_64.rpm
#fi

echo "............................................."
echo "PLEASE MANUALLY COPY THE RPMS OVER TO /var/www/html/yum/base as super user."
echo "............................................."




