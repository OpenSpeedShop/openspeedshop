#!/bin/bash
################################################################################
echo "Build-RPM command-line argument #2 = $2"
echo "Build-RPM command-line argument #3 = $3"


echo "moving specs to SPECS....."
mv specs SPECS
echo "moving toyprograms to toyprograms-0.0.1....."
mv toyprograms toyprograms-0.0.1
echo "moving web-tutorials to webtutorial-0.0.1....."
mv web-tutorials webtutorial-0.0.1
echo "mkdir SOURCES....."
mkdir SOURCES
echo "building executables in toyprograms-0.0.1....."
cd toyprograms-0.0.1
cd sequential/forever
chmod 755 buildit
echo "building forever....."
./buildit
cd ../mutatee
chmod 755 buildit
echo "building mutatee....."
./buildit
cd ../threads
chmod 755 buildit
echo "building threads....."
./buildit
cd ../matmul/orig
chmod 755 buildit
echo "building matmul/orig....."
./buildit
cd ../modified
chmod 755 buildit
echo "building matmul/modified....."
./buildit
cd ../../smg2000
echo "building sequential version of smg2000....."
make
cd ../../mpi/smg2000
echo "building mpi version of smg2000....."
make
cd ../nbody
chmod 755 buildit
echo "building mpi version of nbody....."
./buildit

cd ../../
echo "creating tarball for toyprograms-0.0.1....."
tar -cvf toyprograms-0.0.1.tar toyprograms-0.0.1/*
gzip toyprograms-0.0.1.tar
echo "creating tarball for webtutorial-0.0.1....."
tar -cvf webtutorial-0.0.1.tar webtutorial-0.0.1/*
gzip webtutorial-0.0.1.tar

