#! /bin/bash

#
#  This script is focused on running on the lobo LANL system at the moment.  It should be able to be tweaked
#  to run on TLCC machines using SLURM.  It is very similar to the glory/Sandia script.
#
if [ "$1" == "help" -o "$1" = "--help" ]
then
  echo ""
  echo "Usage: This script automatically compiles smg2000 and sweep3d"
  echo "       using the Intel, PGI, and GNU compilers and the default OpenMPI MPI implementation."
  echo ""
  echo "       It then runs OpenSpeedShop on each of the smg2000 and sweep3d versions using:"
  echo "       the pcsamp, usertime, hwc, hwctime, io, iot, mpi, and mpit experiments."
  echo ""
  echo "       It verifies the results only by searching for a common expected value.  At this time"
  echo "       no performance ratios are tested for."
  echo ""
  exit
fi

#
#
debug_flag=1

##set -x
basedir=`pwd`

# Try looking for SLURM run environment
found_slurm=0
sruncommand=`which srun`

if [ $sruncommand ]; then
  echo "Found SLURM environment: ", $sruncommand
  found_slurm=1
  runcommand=`which srun`
  echo "NOTE: CLEAR ALL MODULE SETTINGS"
  . /usr/share/modules/init/sh
  export MODULEPATH=/users/jegsgi/privatemodules:$MODULEPATH
  module purge
#  module load openss_run_openmpi
#  echo "NOTE: load openss_run_openmpi module"
#  module load mpi/openmpi-1.2.7_ofed_pgi-7.2-3
#  echo "NOTE: load mpi/openmpi-1.2.7_ofed_pgi-7.2-3 module"
else
  echo "Did not find SLURM environment: "
  found_slurm=0
fi

if [ $found_slurm == 0 ]
then
  echo "Did not find SLURM environment: Are we running this script on the correct system?"
  exit
fi

for testname in `ls *.tar.gz`
do
   if [ $debug_flag == 1 ]
   then
      echo "1st loop, testname=" $testname
   fi

   NEWNAME=$(echo "$testname" | sed -e 's/.tar.gz//')

   if [ $debug_flag == 1 ]
   then
     echo "1st loop, NEWNAME=" $NEWNAME
   fi

   cd $basedir

   if [ $debug_flag == 1 ]
   then
     echo "1st loop, basedir=" $basedir
   fi


  #for thisMPI in openmpi mvapich
  for thisMPI in openmpi
  do

#  loop here through a compiler list
#  for thiscompiler in intel pgi gnu
#   for thiscompiler in gnu intel pgi pathscale
   for thiscompiler in gnu 
   do

     if [ "$thiscompiler" == "gnu" ] 
     then
         if [ "$thisMPI" == "openmpi" ]
         then
           module purge
           module load openss-mrnet-openmpi
           module load openmpi-gcc/1.2.8
         elif  [ "$thisMPI" == "mvapich" ] 
         then
           module purge
           module load openss-mrnet-mvapich
           module load mvapich-gcc/1.1
         fi
     elif  [ "$thiscompiler" == "pgi" ] 
     then
         if [ "$thisMPI" == "openmpi" ]
         then
           module purge
           module load pgi-7.2-5
           module load openmpi-pgi/1.2.8
           module load openss-mrnet-openmpi
         elif  [ "$thisMPI" == "mvapich" ] 
         then
           module purge
           module load openss-mrnet-mvapich
           module load pgi-7.2-5
           module load mvapich-pgi/1.1
         fi
     elif  [ "$thiscompiler" == "intel" ] 
     then
         if [ "$thisMPI" == "openmpi" ]
         then
           module purge
           module load openss-mrnet-openmpi
           module load intel-f/10.0.023 intel-c/10.0.023
           module load openmpi-intel/1.2.8
         elif  [ "$thisMPI" == "mvapich" ] 
         then
           module purge
           module load openss-mrnet-mvapich
           module load intel-f/10.0.023 intel-c/10.0.023
           module load mvapich-intel/1.1
         fi
     elif  [ "$thiscompiler" == "pathscale" ] 
     then
         if [ "$thisMPI" == "openmpi" ]
         then
           module purge
           module load openss-mrnet-openmpi
           module load pathscale-3.1
           module load openmpi-pathscale/1.2.8
         elif  [ "$thisMPI" == "mvapich" ] 
         then
           module purge
           module load openss-mrnet-mvapich
           module load pathscale-3.1
           module load mvapich-pathscale/1.1
         fi
     fi

     if [ $debug_flag == 1 ]
     then
       echo "thiscompiler=" $thiscompiler  
       echo "thisMPI=" $thisMPI  
       echo "testsuffix=" $testsuffix
       echo "testCC=" $testCC
     fi

print_compiler=$thiscompiler
echo "print_compiler="$print_compiler


hostabi=`uname -m`
ulimit -c unlimited
#
# setup the path and information with regards to building the MPI applicaton you will be running
#
testexe=$NEWNAME

currentdir=`pwd`
testpathbase=$currentdir

echo $currentdir

gunzip $testexe.tar.gz
tar -xvf $testexe.tar
gzip $testexe.tar

#
# only keep the previous copy, if one already exists delete it
#
if [ -a prev_${testexe}_${thiscompiler}_${thisMPI} ]
then
   rm -rf prev_${testexe}_${thiscompiler}_${thisMPI}
fi
#
# keep the previous run as a previous copy
#
if [ -a ${testexe}_${thiscompiler}_${thisMPI} ]
then
   mv -f ${testexe}_${thiscompiler}_${thisMPI} prev_${testexe}_${thiscompiler}_${thisMPI}
fi
mv $testexe ${testexe}_${thiscompiler}_${thisMPI}

cd ${testexe}_${thiscompiler}_${thisMPI}

thismpicc=`which mpicc`
echo "which mpicc=" $thismpicc
thismpif77=`which mpif77`
echo "which mpif77=" $thismpf77


if [ "$testexe" == "smg2000" ]
then
#  mv -f newMakefile.include Makefile.include
  make
elif  [ "$testexe" == "sweep3d" ]
then
  if [ -z "$MPIHOME" ];
  then
    make mpi
  else
      make -f makefile.lanl clean
      make -f makefile.lanl mpi
  fi
fi

if [ "$testexe" == "smg2000" ]
then
   testpath=$testpathbase/${testexe}_${thiscompiler}_${thisMPI}/test
   testexepath=$testpath
   testexeargs='-n 35 35 35'
   testprogram=$testexepath/$testexe
elif [ "$testexe" == "sweep3d" ]
then
   testpath=$testpathbase/${testexe}_${thiscompiler}_${thisMPI}
   testexepath=$testpath
   testexeargs=''
   testprogram=$testexepath/$testexe.mpi
else
   testpath=$testpathbase/${testexe}_${thiscompiler}_${thisMPI}
   testexepath=$testpath
   testexeargs=''
   testprogram=$testexepath/$testexe
fi

#
# setup the OpenSpeedShop experiment type
#
experiment=usertime
#experiment=pcsamp

sleep 2

#export OPENSS_DEBUG_OPENSS=1
#export OPENSS_DEBUG_MPIJOB=1
#export OPENSS_DEBUG_PROCESS=1
#export DYNINST_DEBUG_STARTUP=1
#export DYNINST_DEBUG_SYMTAB=1

cd $testpath

#
# setup the path to OpenSpeedShop
#
theopenss=`which openss`

echo "Using openss=" $theopenss
echo "Using the raw data directory storage environment variable: OPENSS_RAWDATA_DIR=" $OPENSS_RAWDATA_DIR

#
# Create the default test configuration file if it does not exist
#
if [ -a default_test_config ]
then
  echo "NOTE: Using default_test_config file which already exists"
else
  echo "NOTE: Creating default_test_config file"
cat > default_test_config << EOF
2
16
pgi
jegkas@gmail.com
EOF

fi


cat > common_commands << EOF
wait
expstatus
expview
EOF


if [ "$testexe" == "smg2000" ]
then
cat > smg2000_${thiscompiler}_${thisMPI}_script.sh << EOF
#!/bin/bash
#
. /usr/share/modules/init/sh
export MODULEPATH=/users/jegsgi/privatemodules:$MODULEPATH


     if [ "$thiscompiler" == "gnu" ] 
     then
         if [ "$thisMPI" == "openmpi" ]
         then
           module purge
           module load openss-mrnet-openmpi
           module load openmpi-gcc/1.2.8
         elif  [ "$thisMPI" == "mvapich" ] 
         then
           module purge
           module load openss-mrnet-mvapich
           module load mvapich-gcc/1.1
         fi
     elif  [ "$thiscompiler" == "pgi" ] 
     then
         if [ "$thisMPI" == "openmpi" ]
         then
           module purge
           module load pgi-7.2-5
           module load openmpi-pgi/1.2.8
           module load openss-mrnet-openmpi
         elif  [ "$thisMPI" == "mvapich" ] 
         then
           module purge
           module load openss-mrnet-mvapich
           module load pgi-7.2-5
           module load mvapich-pgi/1.1
         fi
     elif  [ "$thiscompiler" == "intel" ] 
     then
         if [ "$thisMPI" == "openmpi" ]
         then
           module purge
           module load openss-mrnet-openmpi
           module load intel-f/10.0.023 intel-c/10.0.023
           module load openmpi-intel/1.2.8
         elif  [ "$thisMPI" == "mvapich" ] 
         then
           module purge
           module load openss-mrnet-mvapich
           module load intel-f/10.0.023 intel-c/10.0.023
           module load mvapich-intel/1.1
         fi
     elif  [ "$thiscompiler" == "pathscale" ] 
     then
         if [ "$thisMPI" == "openmpi" ]
         then
           module purge
           module load openss-mrnet-openmpi
           module load pathscale-3.1
           module load openmpi-pathscale/1.2.8
         elif  [ "$thisMPI" == "mvapich" ] 
         then
           module purge
           module load openss-mrnet-mvapich
           module load pathscale-3.1
           module load mvapich-pathscale/1.1
         fi
     fi

#
module list
which openss
executable=\`which smg2000\`
local_testpath=\$testpath
currentDir=`pwd`
echo "Current directory (PWD) is:"`pwd`
echo "Current directory (currentDir) is:" \$currentDir
echo "Local testpath directory is:" \$local_testpath
echo "smg2000 executable path directory is:" \$executable

# setup separate unique raw data directories for each compiler/test combination
rm -rf \$currnetDir/smg2000_${thiscompiler}_${thisMPI}_RAW
mkdir \$currentDir/smg2000_${thiscompiler}_${thisMPI}_RAW
export OPENSS_RAWDATA_DIR=\$currentDir/smg2000_${thiscompiler}_${thisMPI}_RAW
echo "NOTE: smg2000 test, after setting OPENSS_RAWDATA_DIR, OPENSS_RAWDATA_DIR=" $OPENSS_RAWDATA_DIR

which openss
export PATH=.:$PATH
echo "NOTE: smg2000 test, after ., PATH=" $PATH
#set executable = `which smg2000`
currentDir=`pwd`
executable=\$currentDir/smg2000
#echo "NOTE: smg2000 test, current directory is:" `pwd`
echo "NOTE: smg2000 test, executable path directory is:" \$executable
mpicommand=`which mpirun`
echo "smg2000 test, mpirun command path directory is:" \$mpicommand

#
# Read in test parameters
#
# script defaults
NodeCount=2
RankCount=16
CompilerType="gnu"
EmailAddress="jegkas@gmail.com"

if [ -a default_test_config ] 
then
  echo "NOTE: Using default_test_config file which already exists"
else
  echo "NOTE: Reading the default_test_config file"
  BAKIFS=\$IFS
  IFS=\$(echo -en "\n\b")
  exec 3<&0
  exec 0<default_test_config

  read curline
  echo "NodeCount: \$curline"
  NodeCount=\$curline
#
  read curline
  echo "RankCount: \$curline"
  RankCount=\$curline
#
  read curline
  echo "CompilerType: \$curline"
  CompilerType=\$curline
#
  read curline
  echo "EmailAddress: \$curline"
  EmailAddress=\$curline

  exec 0<&3

# restore IFS which was used to determine what the field separators are
  IFS=\$BAKIFS

fi

#
if [ $debug_flag == 1 ]
then
  echo "NodeCount: \$NodeCount"
  echo "RankCount: \$RankCount"
  echo "CompilerType: \$CompilerType"
  echo "EmailAddress: \$EmailAddress"
fi

#
#
# Run all experiments
#

echo "smg2000 test, before openss -offline: mpirun command path directory is:" \$mpicommand
echo "smg2000 test, before openss -offline: RankCount is:" \$RankCount
echo "smg2000 test, before openss -offline: openss is:" `which openss`
#
# Run pcsamp and analyze the results
#
openss -offline -f "\$mpicommand -np \$RankCount ./smg2000 -n 25 25 25" pcsamp
ls *.openss | grep "smg2000-pcsamp\." > smg2000_${thiscompiler}_${thisMPI}_create_pcsamp
echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 pcsamp experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_${thisMPI}_create_pcsamp > new_input.script
echo "log -f smg2000_${thiscompiler}_${thisMPI}_pcsamp_results.log" >> new_input.script
cat common_commands >> new_input.script

if [ $debug_flag == 1 ]
then
  echo "CATTING new_input.script"
  cat new_input.script
  echo "BEFORE CALLING openss -batch < new_input.script"
fi

which openss
openss -batch < new_input.script

if [ $debug_flag == 1 ]
then
  echo "AFTER CALLING openss -batch < new_input.script"
fi

grep "hypre_CyclicReduction" smg2000_${thiscompiler}_${thisMPI}_pcsamp_results.log | cat > smg2000_${thiscompiler}_${thisMPI}_pcsamp_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 pcsamp experiment"
echo "-------------------------------------"

#
# Run usertime and analyze the results
#
openss -offline -f "\$mpicommand -np \$RankCount ./smg2000 -n 25 25 25" usertime
ls *.openss | grep "smg2000-usertime\." > smg2000_${thiscompiler}_${thisMPI}_create_usertime

echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 usertime experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_${thisMPI}_create_usertime > new_input.script
echo "log -f smg2000_${thiscompiler}_${thisMPI}_usertime_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "hypre_CyclicReduction" smg2000_${thiscompiler}_${thisMPI}_usertime_results.log | cat > smg2000_${thiscompiler}_${thisMPI}_usertime_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 usertime experiment"
echo "-------------------------------------"
echo ""



openss -offline -f "\$mpicommand -np \$RankCount ./smg2000 -n 25 25 25" hwc
ls *.openss | grep "smg2000-hwc\." > smg2000_${thiscompiler}_${thisMPI}_create_hwc

echo ""
echo "BEGIN Analyzing smg2000 hwc experiment"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_${thisMPI}_create_hwc > new_input.script
echo "log -f smg2000_${thiscompiler}_${thisMPI}_hwc_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "hypre_CyclicReduction" smg2000_${thiscompiler}_${thisMPI}_hwc_results.log | cat > smg2000_${thiscompiler}_${thisMPI}_hwc_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 hwc experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount ./smg2000 -n 25 25 25" hwctime
ls *.openss | grep "smg2000-hwctime\." > smg2000_${thiscompiler}_${thisMPI}_create_hwctime

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 hwctime experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_${thisMPI}_create_hwctime > new_input.script
echo "log -f smg2000_${thiscompiler}_${thisMPI}_hwctime_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "hypre_CyclicReduction" smg2000_${thiscompiler}_${thisMPI}_hwctime_results.log | cat > smg2000_${thiscompiler}_${thisMPI}_hwctime_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 hwctime experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount ./smg2000 -n 25 25 25" io
ls *.openss | grep "smg2000-io\." > smg2000_${thiscompiler}_${thisMPI}_create_io

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 io experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_${thisMPI}_create_io > new_input.script
echo "log -f smg2000_${thiscompiler}_${thisMPI}_io_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "libc_open" smg2000_${thiscompiler}_${thisMPI}_io_results.log | cat > smg2000_${thiscompiler}_${thisMPI}_io_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 io experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount ./smg2000 -n 25 25 25" iot
ls *.openss | grep "smg2000-iot\." > smg2000_${thiscompiler}_${thisMPI}_create_iot

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 iot experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_${thisMPI}_create_iot > new_input.script
echo "log -f smg2000_${thiscompiler}_${thisMPI}_iot_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "libc_open" smg2000_${thiscompiler}_${thisMPI}_iot_results.log | cat > smg2000_${thiscompiler}_${thisMPI}_iot_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 iot experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount ./smg2000 -n 5 5 5" mpi
ls *.openss | grep "smg2000-mpi\-" > smg2000_${thiscompiler}_${thisMPI}_create_mpi

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 mpi experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_${thisMPI}_create_mpi > new_input.script
echo "log -f smg2000_${thiscompiler}_${thisMPI}_mpi_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "\$RankCount  *MPI_Finalize" smg2000_${thiscompiler}_${thisMPI}_mpi_results.log | cat > smg2000_${thiscompiler}_${thisMPI}_mpi_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 mpi experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount ./smg2000 -n 5 5 5" mpit

#
# Find corresponding experiment database files and create files that can be used to restore the databases
#
ls *.openss | grep "smg2000-mpit\-" > smg2000_${thiscompiler}_${thisMPI}_create_mpit

#
# Use the corresponding experiment database file names to restore the database and print out the status and results for the experiments
#
echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 mpit experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_${thisMPI}_create_mpit > new_input.script
echo "log -f smg2000_${thiscompiler}_${thisMPI}_mpit_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "\$RankCount  *MPI_Finalize" smg2000_${thiscompiler}_${thisMPI}_mpit_results.log | cat > smg2000_${thiscompiler}_${thisMPI}_mpit_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 mpit experiment"
echo "-------------------------------------"
echo ""

# script to send test results email
# email subject
SUBJECT="\$thisuname OpenSpeedShop MPI smg2000 executable test results"
# Email To ?
EMAIL="jegkas@gmail.com"
# Email text/message
EMAILMESSAGE="\$currentDir/openss_test_message.txt"
touch \$EMAILMESSAGE
#EMAILMESSAGE="\${local_testpath}/openss_test_message.txt"
echo " OpenSpeedShop Performance Tool SMG2000 Test Results" >\$EMAILMESSAGE
echo "" >>\$EMAILMESSAGE
echo " ${thiscompiler} compiler, $NodeCount Nodes \$RankCount ranks">> \$EMAILMESSAGE
thisuname="\`uname -srnmo\`"
echo " System uname: " \$thisuname>> \$EMAILMESSAGE
thisdate="\`date\`"
echo " Date/time of run: " \$thisdate>> \$EMAILMESSAGE
thisdir="\`pwd\`"
echo " Directory where tests were run: " \$thisdir>> \$EMAILMESSAGE
echo "" >>\$EMAILMESSAGE
#
testval=\`ls *.status | wc -l\`
echo " Expected tests: 8">> \$EMAILMESSAGE
echo " Completed tests: " \$testval>> \$EMAILMESSAGE
#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a smg2000_${thiscompiler}_${thisMPI}_pcsamp_results.status ]
then
  if [ -s smg2000_${thiscompiler}_${thisMPI}_pcsamp_results.status ]
  then
    echo " PCSAMP EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " PCSAMP EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " PCSAMP EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a smg2000_${thiscompiler}_${thisMPI}_usertime_results.status ]
then
  if [ -s smg2000_${thiscompiler}_${thisMPI}_usertime_results.status ]
  then
    echo " USERTIME EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " USERTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " USERTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a smg2000_${thiscompiler}_${thisMPI}_hwc_results.status ]
then
  if [ -s smg2000_${thiscompiler}_${thisMPI}_usertime_results.status ]
  then
    echo " HWC EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " HWC EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " HWC EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a smg2000_${thiscompiler}_${thisMPI}_hwctime_results.status ]
then
  if [ -s smg2000_${thiscompiler}_${thisMPI}_hwctime_results.status ]
  then
    echo " HWCTIME EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " HWCTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " HWCTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a smg2000_${thiscompiler}_${thisMPI}_io_results.status ]
then
  if [ -s smg2000_${thiscompiler}_${thisMPI}_io_results.status ]
  then
    echo " I/O EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " I/O EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " I/O EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a smg2000_${thiscompiler}_${thisMPI}_iot_results.status ]
then
  if [ -s smg2000_${thiscompiler}_${thisMPI}_iot_results.status ]
  then
    echo " I/O TRACE EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " I/O TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " I/O TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a smg2000_${thiscompiler}_${thisMPI}_mpi_results.status ]
then
  if [ -s smg2000_${thiscompiler}_${thisMPI}_mpi_results.status ]
  then
    echo " MPI EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " MPI EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " MPI EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a smg2000_${thiscompiler}_${thisMPI}_mpit_results.status ]
then
  if [ -s smg2000_${thiscompiler}_${thisMPI}_mpit_results.status ]
  then
    echo " MPI TRACE EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " MPI TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " MPI TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
fi
#echo "This is an email message test">> \$EMAILMESSAGE
#echo "This is email text" >>\$EMAILMESSAGE
# send an email using /bin/mail
rsh \$HOSTNAME /bin/mail -s \"\$SUBJECT\" \"-c \$EMAIL\"  \"\$EMAIL\" < \$EMAILMESSAGE

#
# End of nested script
#
exit
EOF

echo "pwd=`pwd`"
echo "testpathbase=$testpathbase"
echo "testpathbase/testexe=$testpathbase/${testexe}"

#RUN_DIR=$testpath
#RUN_DIR=$testpathbase/${testexe}/test
RUN_DIR=$testpathbase/${testexe}_${thiscompiler}_${thisMPI}/test
echo "RUN_DIR=$RUN_DIR"
cd $RUN_DIR
REQ_WALLTIME=1:00
REQ_NNODES=2
REQ_SCRIPT="$RUN_DIR/smg2000_${thiscompiler}_${thisMPI}_script.sh"
sbatch --account=FY093085 --time=${REQ_WALLTIME}:00 -N ${REQ_NNODES} ${REQ_SCRIPT}

elif [ "$testexe" == "sweep3d" ]
then

cat > sweep3d_${thiscompiler}_${thisMPI}_script.sh << EOF
#!/bin/bash
. /usr/share/modules/init/sh
export MODULEPATH=/users/jegsgi/privatemodules:$MODULEPATH

     if [ "$thiscompiler" == "gnu" ] 
     then
         if [ "$thisMPI" == "openmpi" ]
         then
           module purge
           module load openss-mrnet-openmpi
           module load openmpi-gcc/1.2.8
         elif  [ "$thisMPI" == "mvapich" ] 
         then
           module purge
           module load openss-mrnet-mvapich
           module load mvapich-gcc/1.1
         fi
     elif  [ "$thiscompiler" == "pgi" ] 
     then
         if [ "$thisMPI" == "openmpi" ]
         then
           module purge
           module load pgi-7.2-5
           module load openmpi-pgi/1.2.8
           module load openss-mrnet-openmpi
         elif  [ "$thisMPI" == "mvapich" ] 
         then
           module purge
           module load openss-mrnet-mvapich
           module load pgi-7.2-5
           module load mvapich-pgi/1.1
         fi
     elif  [ "$thiscompiler" == "intel" ] 
     then
         if [ "$thisMPI" == "openmpi" ]
         then
           module purge
           module load openss-mrnet-openmpi
           module load intel-f/10.0.023 intel-c/10.0.023
           module load openmpi-intel/1.2.8
         elif  [ "$thisMPI" == "mvapich" ] 
         then
           module purge
           module load openss-mrnet-mvapich
           module load intel-f/10.0.023 intel-c/10.0.023
           module load mvapich-intel/1.1
         fi
     elif  [ "$thiscompiler" == "pathscale" ] 
     then
         if [ "$thisMPI" == "openmpi" ]
         then
           module purge
           module load openss-mrnet-openmpi
           module load pathscale-3.1
           module load openmpi-pathscale/1.2.8
         elif  [ "$thisMPI" == "mvapich" ] 
         then
           module purge
           module load openss-mrnet-mvapich
           module load pathscale-3.1
           module load mvapich-pathscale/1.1
         fi
     fi


#
module list
which openss
#
executable=\`which sweep3d.mpi\`
echo "Current directory is:" \`\$pwd\`
currentDir=`pwd`
echo "Current directory (pwd) is:" `pwd`
echo "Current directory (currentDir) is:" \$currentDir
local_testpath=\$testpath
echo "Local testpath directory is:" \$local_testpath
echo "sweep3d.mpi executable path directory is:" \$executable
mpicommand=`which mpirun`
echo "sweep3d.mpi test, mpirun command path directory is:" \$mpicommand

# setup separate unique raw data directories for each compiler/test combination
rm -rf \$currnetDir/sweep3d_${thiscompiler}_RAW
mkdir \$currentDir/sweep3d_${thiscompiler}_RAW
export OPENSS_RAWDATA_DIR=\$currentDir/sweep3d_${thiscompiler}_RAW
echo "NOTE: sweep3d test, after setting OPENSS_RAWDATA_DIR, OPENSS_RAWDATA_DIR=" $OPENSS_RAWDATA_DIR

#
# Read in test parameters
#
# script defaults
NodeCount=2
RankCount=16
CompilerType="gnu"
EmailAddress="jegkas@gmail.com"

if [ -a default_test_config ] 
then
  echo "NOTE: Using default_test_config file which already exists"
else
  echo "NOTE: Reading the default_test_config file"
  BAKIFS=\$IFS
  IFS=\$(echo -en "\n\b")
  exec 3<&0
  exec 0<default_test_config

  read curline
  echo "NodeCount: \$curline"
  NodeCount=\$curline
#
  read curline
  echo "RankCount: \$curline"
  RankCount=\$curline
#
  read curline
  echo "CompilerType: \$curline"
  CompilerType=\$curline
#
  read curline
  echo "EmailAddress: \$curline"
  EmailAddress=\$curline

  exec 0<&3

# restore IFS which was used to determine what the field separators are
  IFS=\$BAKIFS

fi

#
if [ $debug_flag == 1 ]
then
  echo "NodeCount: \$NodeCount"
  echo "RankCount: \$RankCount"
  echo "CompilerType: \$CompilerType"
  echo "EmailAddress: \$EmailAddress"
fi

#
# Run all experiments
#
# Check the rank count and use an appropriate input file.  The supported sweep3d.mpi input files
# are 8, 16, 32, 64, 128, or 256 ranks.   We move the input.8 or input.16 etc into the default
# input file named "input"
if [ \$RankCount == 8 ] 
then
  echo "Moving the input.8 specific input file to the default input file"
  cp -f \${currentDir}/input.8 \${currentDir}/input
elif  [ \$RankCount == 16 ] 
then
  echo "Moving the input.16 specific input file to the default input file"
  cp -f \${currentDir}/input.16 \${currentDir}/input
elif [ \$RankCount == 32 ] 
then
  echo "Moving the input.32 specific input file to the default input file"
  cp -f \${currentDir}/input.32 \${currentDir}/input
elif [ \$RankCount == 64 ] 
then
  echo "Moving the input.64 specific input file to the default input file"
  cp -f \${currentDir}/input.64 \${currentDir}/input
elif [ \$RankCount == 128 ] 
then
  echo "Moving the input.128 specific input file to the default input file"
  cp -f \${currentDir}/input.128 \${currentDir}/input
elif [ \$RankCount == 256 ] 
then
  echo "Moving the input.256 specific input file to the default input file"
  cp -f \${currentDir}/input.256 \${currentDir}/input
elif [ \$RankCount == 512 ] 
then
  echo "Moving the input.512 specific input file to the default input file"
  cp -f \${currentDir}/input.512 \${currentDir}/input
elif [ \$RankCount == 1024 ] 
then
  echo "Moving the input.1024 specific input file to the default input file"
  cp -f \${currentDir}/input.1024 \${currentDir}/input
elif [ \$RankCount == 2048 ] 
then
  echo "Moving the input.2048 specific input file to the default input file"
  cp -f \${currentDir}/input.2048 \${currentDir}/input
fi

echo "sweep3d.mpi test, before openss -offline: mpirun command path directory is:" \$mpicommand
echo "sweep3d.mpi test, before openss -offline: RankCount is:" \$RankCount
echo "sweep3d.mpi test, before openss -offline: openss is:" `which openss`

#
# Run pcsamp and analyze the results
#
openss -offline -f "\$mpicommand -np \$RankCount ./sweep3d.mpi" pcsamp
ls *.openss | grep "sweep3d.mpi-pcsamp\." > sweep3d_${thiscompiler}_${thisMPI}_create_pcsamp
echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi pcsamp experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_${thisMPI}_create_pcsamp > new_input.script
echo "log -f sweep3d_${thiscompiler}_${thisMPI}_pcsamp_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "source_" sweep3d_${thiscompiler}_${thisMPI}_pcsamp_results.log | cat > sweep3d_${thiscompiler}_${thisMPI}_pcsamp_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi pcsamp experiment"
echo "-------------------------------------"

#
# Run usertime and analyze the results
#
openss -offline -f "\$mpicommand -np \$RankCount ./sweep3d.mpi" usertime
ls *.openss | grep "sweep3d.mpi-usertime\." > sweep3d_${thiscompiler}_${thisMPI}_create_usertime

echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi usertime experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_${thisMPI}_create_usertime > new_input.script
echo "log -f sweep3d_${thiscompiler}_${thisMPI}_usertime_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "source_" sweep3d_${thiscompiler}_${thisMPI}_usertime_results.log | cat > sweep3d_${thiscompiler}_${thisMPI}_usertime_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi usertime experiment"
echo "-------------------------------------"
echo ""



openss -offline -f "\$mpicommand -np \$RankCount ./sweep3d.mpi" hwc
ls *.openss | grep "sweep3d.mpi-hwc\." > sweep3d_${thiscompiler}_${thisMPI}_create_hwc

echo ""
echo "BEGIN Analyzing sweep3d.mpi hwc experiment"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_${thisMPI}_create_hwc > new_input.script
echo "log -f sweep3d_${thiscompiler}_${thisMPI}_hwc_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "source_" sweep3d_${thiscompiler}_${thisMPI}_hwc_results.log | cat > sweep3d_${thiscompiler}_${thisMPI}_hwc_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi hwc experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount ./sweep3d.mpi" hwctime
ls *.openss | grep "sweep3d.mpi-hwctime\." > sweep3d_${thiscompiler}_${thisMPI}_create_hwctime

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi hwctime experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_${thisMPI}_create_hwctime > new_input.script
echo "log -f sweep3d_${thiscompiler}_${thisMPI}_hwctime_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "source_" sweep3d_${thiscompiler}_${thisMPI}_hwctime_results.log | cat > sweep3d_${thiscompiler}_${thisMPI}_hwctime_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi hwctime experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount ./sweep3d.mpi" io
ls *.openss | grep "sweep3d.mpi-io\." > sweep3d_${thiscompiler}_${thisMPI}_create_io

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi io experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_${thisMPI}_create_io > new_input.script
echo "log -f sweep3d_${thiscompiler}_${thisMPI}_io_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "libc_read" sweep3d_${thiscompiler}_${thisMPI}_io_results.log | cat > sweep3d_${thiscompiler}_${thisMPI}_io_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi io experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount ./sweep3d.mpi" iot
ls *.openss | grep "sweep3d.mpi-iot\." > sweep3d_${thiscompiler}_${thisMPI}_create_iot

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi iot experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_${thisMPI}_create_iot > new_input.script
echo "log -f sweep3d_${thiscompiler}_${thisMPI}_iot_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "libc_read" sweep3d_${thiscompiler}_${thisMPI}_iot_results.log | cat > sweep3d_${thiscompiler}_${thisMPI}_iot_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi iot experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount ./sweep3d.mpi" mpi
ls *.openss | grep "sweep3d.mpi-mpi\-" > sweep3d_${thiscompiler}_${thisMPI}_create_mpi

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi mpi experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_${thisMPI}_create_mpi > new_input.script
echo "log -f sweep3d_${thiscompiler}_${thisMPI}_mpi_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "\$RankCount  *MPI_Finalize" sweep3d_${thiscompiler}_${thisMPI}_mpi_results.log | cat > sweep3d_${thiscompiler}_${thisMPI}_mpi_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi mpi experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount ./sweep3d.mpi" mpit

#
# Find corresponding experiment database files and create files that can be used to restore the databases
#
ls *.openss | grep "sweep3d.mpi-mpit\-" > sweep3d_${thiscompiler}_${thisMPI}_create_mpit

#
# Use the corresponding experiment database file names to restore the database and print out the status and results for the experiments
#
echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi mpit experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_${thisMPI}_create_mpit > new_input.script
echo "log -f sweep3d_${thiscompiler}_${thisMPI}_mpit_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "\$RankCount  *MPI_Finalize" sweep3d_${thiscompiler}_${thisMPI}_mpit_results.log | cat > sweep3d_${thiscompiler}_${thisMPI}_mpit_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi mpit experiment"
echo "-------------------------------------"
echo ""

# script to send test results email
# email subject
SUBJECT="\$thisuname OpenSpeedShop MPI sweep3d.mpi executable test results"
# Email To ?
EMAIL=\$EmailAddress
# Email text/message
#EMAILMESSAGE="/tmp/openss_test_message.txt"
EMAILMESSAGE="\$currentDir/openss_test_message.txt"
touch \$EMAILMESSAGE
echo " OpenSpeedShop Performance Tool SWEEP3D.MPI Test Results" >\$EMAILMESSAGE
echo "" >>\$EMAILMESSAGE
echo " ${thiscompiler} compiler, \$NodeCount Nodes \$RankCount ranks">> \$EMAILMESSAGE
thisuname="\`uname -srnmo\`"
echo " System uname: " \$thisuname>> \$EMAILMESSAGE
thisdate="\`date\`"
echo " Date/time of run: " \$thisdate>> \$EMAILMESSAGE
thisdir="\`pwd\`"
echo " Directory where tests were run: " \$thisdir>> \$EMAILMESSAGE
echo "" >>\$EMAILMESSAGE
#
testval=\`ls *.status | wc -l\`
echo " Expected tests: 8">> \$EMAILMESSAGE
echo " Completed tests: " \$testval>> \$EMAILMESSAGE
#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a sweep3d_${thiscompiler}_${thisMPI}_pcsamp_results.status ]
then
  if [ -s sweep3d_${thiscompiler}_${thisMPI}_pcsamp_results.status ]
  then
    echo " PCSAMP EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " PCSAMP EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " PCSAMP EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a sweep3d_${thiscompiler}_${thisMPI}_usertime_results.status ]
then
  if [ -s sweep3d_${thiscompiler}_${thisMPI}_usertime_results.status ]
  then
    echo " USERTIME EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " USERTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " USERTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a sweep3d_${thiscompiler}_${thisMPI}_hwc_results.status ]
then
  if [ -s sweep3d_${thiscompiler}_${thisMPI}_usertime_results.status ]
  then
    echo " HWC EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " HWC EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " HWC EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a sweep3d_${thiscompiler}_${thisMPI}_hwctime_results.status ]
then
  if [ -s sweep3d_${thiscompiler}_${thisMPI}_hwctime_results.status ]
  then
    echo " HWCTIME EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " HWCTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " HWCTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a sweep3d_${thiscompiler}_${thisMPI}_io_results.status ]
then
  if [ -s sweep3d_${thiscompiler}_${thisMPI}_io_results.status ]
  then
    echo " I/O EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " I/O EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " I/O EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a sweep3d_${thiscompiler}_${thisMPI}_iot_results.status ]
then
  if [ -s sweep3d_${thiscompiler}_${thisMPI}_iot_results.status ]
  then
    echo " I/O TRACE EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " I/O TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " I/O TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a sweep3d_${thiscompiler}_${thisMPI}_mpi_results.status ]
then
  if [ -s sweep3d_${thiscompiler}_${thisMPI}_mpi_results.status ]
  then
    echo " MPI EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " MPI EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " MPI EXPERIMENT FAILED">> \$EMAILMESSAGE
fi

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if [ -a sweep3d_${thiscompiler}_${thisMPI}_mpit_results.status ]
then
  if [ -s sweep3d_${thiscompiler}_${thisMPI}_mpit_results.status ]
  then
    echo " MPI TRACE EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " MPI TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
  fi
else
  echo " MPI TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
fi
#echo "This is an email message test">> \$EMAILMESSAGE
#echo "This is email text" >>\$EMAILMESSAGE
# send an email using /bin/mail
#/bin/mail -s "\$SUBJECT" "\$EMAIL" < \$EMAILMESSAGE
rsh \$HOSTNAME /bin/mail -s \"\$SUBJECT\" \"-c \$EMAIL\"  \"\$EMAIL\" < \$EMAILMESSAGE

#
# End of nested script
#
exit
EOF

echo "pwd=`pwd`"
echo "testpathbase=$testpathbase"
echo "testpathbase/testexe=$testpathbase/${testexe}"

#RUN_DIR=$testpath
#RUN_DIR=$testpathbase/${testexe}
RUN_DIR=$testpathbase/${testexe}_${thiscompiler}_${thisMPI}
echo "RUN_DIR=$RUN_DIR"
cd $RUN_DIR
REQ_WALLTIME=1:00
REQ_NNODES=2
REQ_SCRIPT="$RUN_DIR/sweep3d_${thiscompiler}_${thisMPI}_script.sh"
sbatch --account=FY093085 --time=${REQ_WALLTIME}:00 -N ${REQ_NNODES} ${REQ_SCRIPT}
fi


# Send out the results for sweep3d.mpi
# sweep_${thiscompiler}_${thisMPI}_email.sh
#EMAIL_REQ_SCRIPT="$RUN_DIR/sweep_${thiscompiler}_${thisMPI}_email.sh"
#sh ${EMAIL_REQ_SCRIPT}


cd $testpathbase
echo "testpathbase=$testpathbase"

##
## only keep the previous copy, if one already exists delete it
##
#if [ -a prev_${testexe}_${thiscompiler} ]
#then
#   rm -rf prev_${testexe}_${thiscompiler}
#fi
##
## keep the previous run as a previous copy
##
#if [ -a ${testexe}_${thiscompiler} ]
#then
##   mv -f ${testexe}_${thiscompiler} prev_${testexe}_${thiscompiler}
#fi
#mv $testexe ${testexe}_${thiscompiler}


# end loop through the compiler list
done

# end loop through the MPI implemementations
done

# end loop through the test list
done

exit


cd $testpathbase 

