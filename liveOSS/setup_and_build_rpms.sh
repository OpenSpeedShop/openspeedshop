#!/bin/bash
echo "..........................................."
echo "Running setup_and_build_rpms.sh...."
echo "..........................................."
################################################################################

echo "..........................................."
echo "Copying specs to SPECS....."
echo "..........................................."
cp -r specs SPECS
echo "..........................................."
echo "Moving toyprograms to toyprograms-0.0.1....."
echo "..........................................."
mv toyprograms toyprograms-0.0.1
echo "..........................................."
echo "Moving web-tutorials to webtutorial-0.0.1....."
echo "..........................................."
mv web-tutorials webtutorial-0.0.1
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
cd ../../mpi/smg2000
echo "..........................................."
echo "Building mpi version of smg2000....."
echo "..........................................."
make
cd ../nbody
chmod 755 buildit
echo "..........................................."
echo "Building mpi version of nbody....."
echo "..........................................."
./buildit

cd ../../..

pwd

echo "..........................................."
echo "Creating tarball for toyprograms-0.0.1....."
echo "..........................................."
tar -cvf toyprograms-0.0.1.tar toyprograms-0.0.1/*
gzip toyprograms-0.0.1.tar
mv toyprograms-0.0.1.tar.gz SOURCES/.
echo "..........................................."
echo "Creating tarball for webtutorial-0.0.1....."
echo "..........................................."
tar -cvf webtutorial-0.0.1.tar webtutorial-0.0.1/*
gzip webtutorial-0.0.1.tar
mv webtutorial-0.0.1.tar.gz SOURCES/.

echo ".........................................................."
echo "Using install.sh to reset .rpmmacros to this location....."
echo ".........................................................."
./install.sh
0


echo "..........................................."
echo "Building RPM for webtutorial-0.0.1....."
echo "..........................................."
./Build-RPM webtutorial-0.0.1
echo "..........................................."
echo "Listing the location RPM for webtutorial-0.0.1....."
echo "..........................................."
ls -lastr RPMS/localhost.localdomain/webtutorial.OSS.i386.rpm

echo "..........................................."
echo "Building RPM for toyprograms-0.0.1....."
echo "..........................................."
./Build-RPM toyprograms-0.0.1
echo "..........................................."
echo "Listing the location RPM for toyprograms-0.0.1....."
echo "..........................................."
ls -lastr RPMS/localhost.localdomain/toyprograms.OSS.i386.rpm

echo "..........................................."
echo "Checking for SOURCES/openmpi-1.2.8.tar.gz....."
echo "..........................................."

if [ -f SOURCES/openmpi-1.2.8.tar.gz ]; then
     echo "..........................................."
     echo "Building RPM for openmpi-1.2.8....."
     echo "..........................................."
     ./Build-RPM openmpi-1.2.8
else
     echo "No SOURCES/openmpi-1.2.8.tar.gz file was found."
     echo "Please place the file into SOURCES/openmpi-1.2.8.tar.gz and answer with a 'y' or 'Y' to continue."
     read answer

     if [ "$answer" = Y -o "$answer" = y ]; then
        echo "..........................................."
        echo "Building RPM for openmpi-1.2.8....."
        echo "..........................................."
        ./Build-RPM openmpi-1.2.8
     else
        echo "..........................................."
        echo "Skipping the build of the RPM for openmpi-1.2.8....."
        echo "..........................................."
     fi
fi

echo "..........................................."
echo "Listing the location RPM for openmpi-1.n.m....."
echo "..........................................."
ls -lastr RPMS/localhost.localdomain/openmpi.OSS.i386.rpm

echo "............................................."
echo "PLEASE MANUALLY COPY THE RPMS OVER TO /var/www/html/yum/base as super user."
echo "............................................."




