#! /bin/tcsh  -vx

#
#  This script is focused on running on LANL systems
#

if($1 == "--help" || $1 == "help") then
  echo ""
  echo "Usage: This script automatically compiles smg2000 and sweep3d"
  echo "       using the Intel, PGI, and GNU compilers and the default MPI implementation."
  echo ""
  echo "       It then runs OpenSpeedShop on each of the smg2000 and sweep3d versions using:"
  echo "       the pcsamp, usertime, hwc, hwctime, io, iot, mpi, and mpit experiments."
  echo ""
  echo "       It verifies the results only by searching for a common expected value.  At this time"
  echo "       no performance ratios are tested for."
  echo ""
  exit
endif

#
#
set debug_flag = 1

##set -x

set basedir = `pwd`

#source ${MODULESHOME}/init/csh
source /usr/share/modules/init/tcsh
setenv MODULEPATH /users/jegsgi/privatemodules:$MODULEPATH

module purge
echo "NOTE: load openss-mrnet module"
module load openss-mrnet
#echo "NOTE: load openmpi-gcc/1.2.4 module"
#module load openmpi-gcc/1.2.4
echo "NOTE: start MODULE LIST - TOP OF SCRIPT"
module list
echo "NOTE: end   MODULE LIST - TOP OF SCRIPT"

set NEWNAME = ""

#for testname in `ls *.tar.gz`
foreach testname ( `ls *.tar.gz` )
#do
   echo "NOTE: in foreach testname"
   if ( $debug_flag == 1 ) then
      echo "1st loop, testname=" 
      echo "1st loop, testname=" $testname
   endif

   echo "before NEWNAME=" 
#   set NEWNAME = (echo $testname | sed -e 's/.tar.gz//')
   set NEWNAME = `echo "$testname" | sed -e "s/.tar.gz//"`
   echo "after NEWNAME=" 

   if ( $debug_flag == 1 ) then
     echo "1st loop, NEWNAME=" $NEWNAME
   endif

   cd $basedir

   if ( $debug_flag == 1 ) then
     echo "1st loop, basedir=" $basedir
   endif

#thiscompiler="intel"
#  for thiscompiler in intel pgi gnu

#  loop here through a compiler list
#for thiscompiler in gnu intel pgi pathscale
#do
#foreach thiscompiler ( intel )
foreach thiscompiler ( gnu intel pgi pathscale )
 if ( $debug_flag == 1 ) then
   echo "thiscompiler=" $thiscompiler  
#   echo "testsuffix=" $testsuffix
#   echo "testCC=" $testCC
 endif

 module purge
 module load modules openss-mrnet

 if ( $thiscompiler == "gnu" ) then
    module load openmpi-gcc/1.2.4
 else if  ( $thiscompiler == "pgi" )then
    module load pgi/7.2-5
    module load openmpi-pgi/1.2.4
 else if  ( $thiscompiler == "intel" )then
#    module load intel-c/10.0.023
#    module load intel-f/10.0.023
    module load intel-c/9.1.043
    module load intel-f/9.1.037
    module load openmpi-intel/1.2.4
 else if  ( $thiscompiler == "pathscale" )then
    module load pathscale/3.0
    module load openmpi-pathscale/1.2.4
 endif

echo "NOTE: start MODULE LIST - AFTER COMPILER SELECTION"
module list
echo "NOTE: end   MODULE LIST - AFTER COMPILER SELECTION"

set hostabi = `uname -m`
`ulimit -c unlimited`
#
# setup the path and information with regards to building the MPI applicaton you will be running
#
set testexe = $NEWNAME
echo "test independent, before testexe.tar.gz unpacking, NEWNAME=" $NEWNAME

gunzip $testexe.tar.gz
tar -xvf $testexe.tar
gzip $testexe.tar

set currentdir = `pwd`
echo "test independent, first currentdir=" $currentdir
set testpathbase = $currentdir
echo "test independent, testpathbase=" $testpathbase

# keep two copies, the previous and the current
# if previous already exists remove it.
echo "test independent, prev test prev_${testexe}_${thiscompiler}=" prev_${testexe}_${thiscompiler}
if ( -e prev_${testexe}_${thiscompiler} ) then
   rm -rf prev_${testexe}_${thiscompiler}
endif

mv -f  ${testexe}_${thiscompiler} prev_${testexe}_${thiscompiler}
mv ${testexe} ${testexe}_${thiscompiler}
cd ${testexe}_${thiscompiler}
echo "test independent, first ${testexe}_${thiscompiler}=" ${testexe}_${thiscompiler}

set currentdir = `pwd`
#echo $currentdir
echo "test independent, second currentdir=" $currentdir

set thismpicc = `which mpicc`
echo "which mpicc=" $thismpicc

if ( "$testexe" == "smg2000" ) then
#  mv -f newMakefile.include Makefile.include
  make
else if  ( "$testexe" == "sweep3d" ) then
#  mv -f new_makefile makefile
  make -f makefile.lanl clean
  make -f makefile.lanl mpi
endif

if ( "$testexe" == "smg2000" ) then
   set testpath = $testpathbase/${testexe}_${thiscompiler}/test
   set testexepath = $testpath
   set testexeargs = '-n 35 35 35'
   set testprogram = $testexepath/$testexe
else if ( "$testexe" == "sweep3d" ) then
   set testpath = $testpathbase/${testexe}_${thiscompiler}
   set testexepath = $testpath
   set testexeargs = ''
   set testprogram = $testexepath/$testexe.mpi
else
   set testpath = $testpathbase/${testexe}_${thiscompiler}
   set testexepath = $testpath
   set testexeargs = ''
   set testprogram = $testexepath/$testexe
endif

#
# setup the OpenSpeedShop experiment type
#
set experiment = usertime
#experiment=pcsamp

sleep 2

#setenv OPENSS_DEBUG_OPENSS 1
#setenv OPENSS_DEBUG_MPIJOB 1
#setenv OPENSS_DEBUG_PROCESS 1
#setenv DYNINST_DEBUG_STARTUP 1
#setenv DYNINST_DEBUG_SYMTAB 1

echo "testpath=" $testpath
cd $testpath

#
# setup the path to OpenSpeedShop
#
set theopenss = `which openss`
#theopenss=/g/g24/jeg/chaos_4_x86_64_ib/opt/OSS-mrnet/bin/openss

echo "Using openss=" $theopenss

# 
# Create the default test configuration file if it does not exist
#
# CHANGE THIS FILE CAT TO ADD YOUR EMAIL and NODE/RANK COUNTS
# For sweep3d.mpi to execute properly please keep the rank count a power of two.
# Also, the compiler setting "all" is ignored right now.
# The default_test_config file is defined as:
# Number of nodes
# Number of ranked processes
# Compiler setting: all, pgi, intel, gnu, pathscale  (currently ignored)
# Email address where the test reports are sent
#

if ( -e default_test_config ) then
  echo "NOTE: Using default_test_config file which already exists"
else
  echo "NOTE: Creating default_test_config file"
cat > default_test_config << EOF
4
32
all
jegkas@gmail.com
EOF
endif

# define the NodeCount for the launch script (msub) so it knows how many nodes to request
# they should be the same as the default_test_config file above
set NodeCount = 4


cat > common_commands << EOF
wait
expstatus
expview
EOF


if ( "$testexe" == "smg2000" ) then

echo "NOTE: Creating  smg2000_${thiscompiler}_openmpi_script.tcsh"

cat > smg2000_${thiscompiler}_openmpi_script.tcsh << EOF
#!/bin/tcsh
set debug_flag = 1
#source ${MODULESHOME}/init/csh
source /usr/share/modules/init/tcsh
setenv MODULEPATH /users/jegsgi/privatemodules:$MODULEPATH

echo "NOTE: smg2000 test, thiscompiler=" $thiscompiler
module purge
module load modules openss-mrnet
echo "NOTE: smg2000 test, start MODULE LIST after loading openss-mrnet="
module list
echo "NOTE: smg2000 test, after MODULE LIST after loading openss-mrnet="

if ( $thiscompiler == "gnu" ) then
    module load openmpi-gcc/1.2.4
else if  ( $thiscompiler == "pgi" ) then
    module load pgi/7.2-5
    module load openmpi-pgi/1.2.4
else if  ( $thiscompiler == "intel" ) then
#    module load intel-c/10.0.023
    module load intel-c/9.1.043
    module load intel-f/9.1.037
    module load openmpi-intel/1.2.4
else if  ( $thiscompiler == "pathscale" ) then
    module load pathscale/3.0
    module load openmpi-pathscale/1.2.4
endif

echo "NOTE: smg2000 test, start MODULE LIST="
module list
echo "NOTE: smg2000 test, end MODULE LIST="
echo "NOTE: smg2000 test, PATH=" $PATH

# setup separate unique raw data directories for each compiler/test combination
rm -rf /scratch3/jegsgi/smg2000_${thiscompiler}_RAW
mkdir /scratch3/jegsgi/smg2000_${thiscompiler}_RAW
setenv OPENSS_RAWDATA_DIR /scratch3/jegsgi/smg2000_${thiscompiler}_RAW

which openss
setenv PATH .:$PATH
echo "NOTE: smg2000 test, after ., PATH=" $PATH
#set executable = `which smg2000`
set currentDir = `pwd`
set executable = \$currentDir/smg2000
#echo "NOTE: smg2000 test, current directory is:" `pwd`
echo "NOTE: smg2000 test, executable path directory is:" \$executable
set mpicommand = `which mpirun`
echo "smg2000 test, mpirun command path directory is:" \$mpicommand
#
# Read in test parameters
#

set testCompiler = "${thiscompiler}"

#echo "ls of default_test_config=`ls -lastr default_test_config`"

if ( -e  \${currentDir}/default_test_config ) then
  setenv IFS "\n"
  set cnt = 0
  foreach i (\`cat \${currentDir}/default_test_config\`)
    echo \$i
    echo "Im here 0"
    @ cnt = \$cnt + 1
    echo "Im here 1"
    if ( \$cnt == 1 ) then
      echo "Im here 2"
      set NodeCount = \`echo \$i\`
      echo "Im here 3"
    else if ( \$cnt == 2 ) then
      echo "Im here 4"
      set RankCount = \`echo \$i\`
      echo "Im here 5"
    else if ( \$cnt == 3 ) then
      echo "Im here 6"
      set CompilerType = \`echo \$i\`
      echo "Im here 6"
    else if ( \$cnt == 4 ) then
      echo "Im here 8"
      set EmailAddress = \`echo \$i\`
      echo "Im here 9"
    endif
  end
else
  set NodeCount = 4
  set RankCount = 32
  set EmailAddress = jegkas@gmail.com
  set CompilerType = all
endif

echo "Im here 10"
#
if ( \$debug_flag == 1 ) then
  echo "DEBUG: NodeCount: \$NodeCount"
  echo "DEBUG: RankCount: \$RankCount"
  echo "DEBUG: CompilerType: \$CompilerType"
  echo "DEBUG: EmailAddress: \$EmailAddress"
endif

#
#
# Run all experiments
#

echo "NOTE: Using openss=" $theopenss
echo "NOTE: smg2000 testpath=" $testpath
echo "NOTE: smg2000 mpicommand=" \$mpicommand
echo "NOTE: smg2000 RankCount=" \$RankCount
echo "NOTE: smg2000 executable=" \$executable
echo "NOTE: smg2000 start pwd=`pwd`"
echo "NOTE: end pwd"

cd $testpath

echo "NOTE: after cd testpath, smg2000 start pwd=`pwd`"
echo "NOTE: end pwd"

#
# Run the executable by itself first
#
\$mpicommand -np \$RankCount \$executable -n 40 40 40

#
# Run pcsamp and analyze the results
#
openss -offline -f "\$mpicommand -np \$RankCount \$executable -n 40 40 40" pcsamp
ls *.openss | grep "smg2000-pcsamp\." > smg2000_${thiscompiler}_openmpi_create_pcsamp
echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 pcsamp experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_openmpi_create_pcsamp > new_input.script
echo "log -f smg2000_${thiscompiler}_openmpi_pcsamp_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "hypre_CyclicReduction" smg2000_${thiscompiler}_openmpi_pcsamp_results.log | cat > smg2000_${thiscompiler}_openmpi_pcsamp_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 pcsamp experiment"
echo "-------------------------------------"

#
# Run usertime and analyze the results
#
openss -offline -f "\$mpicommand -np \$RankCount \$executable -n 40 40 40" usertime
ls *.openss | grep "smg2000-usertime\." > smg2000_${thiscompiler}_openmpi_create_usertime

echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 usertime experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_openmpi_create_usertime > new_input.script
echo "log -f smg2000_${thiscompiler}_openmpi_usertime_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "hypre_CyclicReduction" smg2000_${thiscompiler}_openmpi_usertime_results.log | cat > smg2000_${thiscompiler}_openmpi_usertime_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 usertime experiment"
echo "-------------------------------------"
echo ""



openss -offline -f "\$mpicommand -np \$RankCount \$executable -n 40 40 40" hwc
ls *.openss | grep "smg2000-hwc\." > smg2000_${thiscompiler}_openmpi_create_hwc

echo ""
echo "BEGIN Analyzing smg2000 hwc experiment"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_openmpi_create_hwc > new_input.script
echo "log -f smg2000_${thiscompiler}_openmpi_hwc_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "hypre_CyclicReduction" smg2000_${thiscompiler}_openmpi_hwc_results.log | cat > smg2000_${thiscompiler}_openmpi_hwc_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 hwc experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount \$executable -n 40 40 40" hwctime
ls *.openss | grep "smg2000-hwctime\." > smg2000_${thiscompiler}_openmpi_create_hwctime

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 hwctime experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_openmpi_create_hwctime > new_input.script
echo "log -f smg2000_${thiscompiler}_openmpi_hwctime_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "hypre_CyclicReduction" smg2000_${thiscompiler}_openmpi_hwctime_results.log | cat > smg2000_${thiscompiler}_openmpi_hwctime_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 hwctime experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount \$executable -n 40 40 40" io
ls *.openss | grep "smg2000-io\." > smg2000_${thiscompiler}_openmpi_create_io

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 io experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_openmpi_create_io > new_input.script
echo "log -f smg2000_${thiscompiler}_openmpi_io_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "libc_open" smg2000_${thiscompiler}_openmpi_io_results.log | cat > smg2000_${thiscompiler}_openmpi_io_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 io experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount \$executable -n 40 40 40" iot
ls *.openss | grep "smg2000-iot\." > smg2000_${thiscompiler}_openmpi_create_iot

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 iot experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_openmpi_create_iot > new_input.script
echo "log -f smg2000_${thiscompiler}_openmpi_iot_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "libc_open" smg2000_${thiscompiler}_openmpi_iot_results.log | cat > smg2000_${thiscompiler}_openmpi_iot_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 iot experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount \$executable -n 5 5 5" mpi
ls *.openss | grep "smg2000-mpi\-" > smg2000_${thiscompiler}_openmpi_create_mpi

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 mpi experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_openmpi_create_mpi > new_input.script
echo "log -f smg2000_${thiscompiler}_openmpi_mpi_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "\$RankCount *MPI_Finalize" smg2000_${thiscompiler}_openmpi_mpi_results.log | cat > smg2000_${thiscompiler}_openmpi_mpi_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 mpi experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount \$executable -n 5 5 5" mpit

#
# Find corresponding experiment database files and create files that can be used to restore the databases
#
ls *.openss | grep "smg2000-mpit\-" > smg2000_${thiscompiler}_openmpi_create_mpit

#
# Use the corresponding experiment database file names to restore the database and print out the status and results for the experiments
#
echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing smg2000 mpit experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' smg2000_${thiscompiler}_openmpi_create_mpit > new_input.script
echo "log -f smg2000_${thiscompiler}_openmpi_mpit_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "\$RankCount *MPI_Finalize" smg2000_${thiscompiler}_openmpi_mpit_results.log | cat > smg2000_${thiscompiler}_openmpi_mpit_results.status

echo "-------------------------------------"
echo "END Analyzing smg2000 mpit experiment"
echo "-------------------------------------"
echo ""

# script to send test results email
# email subject
set SUBJECT="OpenSpeedShop MPI smg2000 executable test results"
# Email To ?
set EMAIL = \$EmailAddress
#
# Email text/message
set EMAILMESSAGE="/tmp/openss_test_message.txt"
echo " OpenSpeedShop Performance Tool SMG2000 Test Results" >\$EMAILMESSAGE
echo "" >>\$EMAILMESSAGE
echo " \${testCompiler} Compiler \$NodeCount Nodes \$RankCount ranks">> \$EMAILMESSAGE
set thisuname="\`uname -srnmo\`"
echo " System uname: " \$thisuname>> \$EMAILMESSAGE
set thisdate="\`date\`"
echo " Date/time of run: " \$thisdate>> \$EMAILMESSAGE
set thisdir="\`pwd\`"
echo " Directory where tests were run: " \$thisdir>> \$EMAILMESSAGE
echo "" >>\$EMAILMESSAGE
#
set testval=`ls *.status | wc -l`
echo " Expected tests: 8">> \$EMAILMESSAGE
echo " Completed tests: " \$testval>> \$EMAILMESSAGE
#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e smg2000_${thiscompiler}_openmpi_pcsamp_results.status ) then
  if ( -s smg2000_${thiscompiler}_openmpi_pcsamp_results.status ) then
    echo " PCSAMP EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " PCSAMP EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " PCSAMP EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e smg2000_${thiscompiler}_openmpi_usertime_results.status ) then
  if ( -s smg2000_${thiscompiler}_openmpi_usertime_results.status ) then
    echo " USERTIME EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " USERTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " USERTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e smg2000_${thiscompiler}_openmpi_hwc_results.status ) then
  if ( -s smg2000_${thiscompiler}_openmpi_usertime_results.status ) then
    echo " HWC EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " HWC EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " HWC EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e smg2000_${thiscompiler}_openmpi_hwctime_results.status ) then
  if ( -s smg2000_${thiscompiler}_openmpi_hwctime_results.status ) then
    echo " HWCTIME EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " HWCTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " HWCTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e smg2000_${thiscompiler}_openmpi_io_results.status ) then
  if ( -s smg2000_${thiscompiler}_openmpi_io_results.status ) then
    echo " I/O EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " I/O EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " I/O EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e smg2000_${thiscompiler}_openmpi_iot_results.status ) then
  if ( -s smg2000_${thiscompiler}_openmpi_iot_results.status ) then
    echo " I/O TRACE EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " I/O TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " I/O TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e smg2000_${thiscompiler}_openmpi_mpi_results.status ) then
  if ( -s smg2000_${thiscompiler}_openmpi_mpi_results.status ) then
    echo " MPI EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " MPI EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " MPI EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e smg2000_${thiscompiler}_openmpi_mpit_results.status ) then
  if ( -s smg2000_${thiscompiler}_openmpi_mpit_results.status ) then
    echo " MPI TRACE EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " MPI TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " MPI TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
endif
#echo "This is an email message test">> \$EMAILMESSAGE
#echo "This is email text" >>\$EMAILMESSAGE
# send an email using /bin/mail
/bin/mail -s "\$SUBJECT" "-c \$EMAIL" "\$EMAIL" < \$EMAILMESSAGE

#
# End of nested script
#
exit
EOF

echo "pwd=`pwd`"

set RUN_DIR=$testpath
echo "RUN_DIR=$RUN_DIR"
cd $RUN_DIR
#set REQ_WALLTIME = 1:00
#set REQ_NNODES = $NodeCount
#echo "REQ_NNODES=$REQ_NNODES"
#set REQ_SCRIPT = "$RUN_DIR/smg2000_${thiscompiler}_openmpi_script.tcsh"

echo "NOTE: BEFORE Creating  moab_smg2000_${thiscompiler}_openmpi_script.tcsh"

cat > moab_smg2000_${thiscompiler}_openmpi_script.tcsh << EOF
#!/bin/tcsh
#MSUB -o $testpath/smg2000_${thiscompiler}_openmpi_output.txt
#MSUB -j oe
#MSUB -l nodes="$NodeCount":ppn=8
#MSUB -l walltime=2400
#MSUB -N  $testpath/smg2000_${thiscompiler}_openmpi_script.tcsh

echo Shell is $SHELL
echo Program is $0

echo "BEFORE: Kick off $testpath/smg2000_${thiscompiler}_openmpi_script.tcsh"
tcsh  $testpath/smg2000_${thiscompiler}_openmpi_script.tcsh
echo "AFTER: Kick off $testpath/smg2000_${thiscompiler}_openmpi_script.tcsh"

EOF

echo "NOTE: AFTER Creating  moab_smg2000_${thiscompiler}_openmpi_script.tcsh"

/opt/MOAB/bin/msub moab_smg2000_${thiscompiler}_openmpi_script.tcsh

echo "NOTE: AFTER msub moab_smg2000_${thiscompiler}_openmpi_script.tcsh"

else if ( "$testexe" == "sweep3d" ) then

echo "NOTE: BEFORE SWEEP3D: Creating  sweep3d_${thiscompiler}_openmpi_script.tcsh"

cat > sweep3d_${thiscompiler}_openmpi_script.tcsh << EOF
#!/bin/tcsh
#
set debug_flag = 1
#
#source ${MODULESHOME}/init/tcsh
source /usr/share/modules/init/tcsh
setenv MODULEPATH /users/jegsgi/privatemodules:$MODULEPATH
#
module purge
module load modules openss-mrnet
#
if ( $thiscompiler == "gnu" ) then
    module load openmpi-gcc/1.2.4
else if  ( $thiscompiler == "pgi" ) then
    module load pgi/7.2-5
    module load openmpi-pgi/1.2.4
else if  ( $thiscompiler == "intel" ) then
#    module load intel-c/10.0.023
#    module load intel-f/10.0.023
    module load intel-c/9.1.043
    module load intel-f/9.1.037
    module load openmpi-intel/1.2.4
else if  ( $thiscompiler == "pathscale" ) then
    module load pathscale/3.0
    module load openmpi-pathscale/1.2.4
endif
#
echo "NOTE: sweep3d test, start MODULE LIST="
module list
echo "NOTE: sweep3d test, end MODULE LIST="
#
# setup separate unique raw data directories for each compiler/test combination
rm -rf /scratch3/jegsgi/sweep3d_${thiscompiler}_RAW
mkdir /scratch3/jegsgi/sweep3d_${thiscompiler}_RAW
setenv OPENSS_RAWDATA_DIR /scratch3/jegsgi/sweep3d_${thiscompiler}_RAW
#
which openss
#
setenv PATH .:$PATH
echo "Current directory is:" `pwd`
set currentDir = `pwd`
set executable = \$currentDir/sweep3d.mpi
echo "sweep3d.mpi executable path directory is:" \$executable
set mpicommand = `which mpirun`
echo "mpirun command path directory is:" \$mpicommand
#
# Read in test parameters
#
#

set testCompiler = "${thiscompiler}"

if ( -e  \${currentDir}/default_test_config ) then
  setenv IFS "\n"
  set cnt = 0
  foreach i (\`cat \${currentDir}/default_test_config\`)
    echo \$i
    echo "Im here 0"
    @ cnt = \$cnt + 1
    echo "Im here 1"
    if ( \$cnt == 1 ) then
      echo "Im here 2"
      set NodeCount = \`echo \$i\`
      echo "Im here 3"
    else if ( \$cnt == 2 ) then
      echo "Im here 4"
      set RankCount = \`echo \$i\`
      echo "Im here 5"
    else if ( \$cnt == 3 ) then
      echo "Im here 6"
      set CompilerType = \`echo \$i\`
      echo "Im here 6"
    else if ( \$cnt == 4 ) then
      echo "Im here 8"
      set EmailAddress = \`echo \$i\`
      echo "Im here 9"
    endif
  end
else
  set NodeCount = 4
  set RankCount = 32
  set EmailAddress = jegkas@gmail.com
  set CompilerType = all
endif

echo "Im here 10"

#
if ( \$debug_flag == 1 ) then
  echo "DEBUG: NodeCount: \$NodeCount"
  echo "DEBUG: RankCount: \$RankCount"
  echo "DEBUG: CompilerType: \$CompilerType"
  echo "DEBUG: EmailAddress: \$EmailAddress"
endif
#
# Run all experiments
#

# Check the rank count and use an appropriate input file.  The supported sweep3d.mpi input files
# are 8, 16, 32, 64, 128, or 256 ranks.   We move the input.8 or input.16 etc into the default
# input file named "input"
if ( \$RankCount == 8 ) then
  echo "Moving the input.8 specific input file to the default input file"
  cp -f \${currentDir}/input.8 \${currentDir}/input
else if  ( \$RankCount == 16 ) then
  echo "Moving the input.16 specific input file to the default input file"
  cp -f \${currentDir}/input.16 \${currentDir}/input
else if ( \$RankCount == 32 ) then
  echo "Moving the input.32 specific input file to the default input file"
  cp -f \${currentDir}/input.32 \${currentDir}/input
else if ( \$RankCount == 64 ) then
  echo "Moving the input.64 specific input file to the default input file"
  cp -f \${currentDir}/input.64 \${currentDir}/input
else if ( \$RankCount == 128 ) then
  echo "Moving the input.128 specific input file to the default input file"
  cp -f \${currentDir}/input.128 \${currentDir}/input
else if ( \$RankCount == 256 ) then
  echo "Moving the input.256 specific input file to the default input file"
  cp -f \${currentDir}/input.256 \${currentDir}/input
endif


echo "NOTE: Using openss=" $theopenss
echo "NOTE: sweep3d testpath=" $testpath
echo "NOTE: sweep3d mpicommand=" \$mpicommand
echo "NOTE: sweep3d RankCount=" \$RankCount
echo "NOTE: sweep3d executable=" \$executable
echo "NOTE: sweep3d start pwd=`pwd`"
echo "NOTE: end pwd"
#
cd $testpath

echo "NOTE: after cd testpath, sweep3d start pwd=`pwd`"
echo "NOTE: end pwd"
#
# Run the executable by itself first
#
\$mpicommand -np \$RankCount \$executable -n 40 40 40
#
#
# Run pcsamp and analyze the results
#
openss -offline -f "\$mpicommand -np \$RankCount \$executable" pcsamp
#openss -offline -f "\$mpicommand -np 32 \$executable" pcsamp
ls *.openss | grep "sweep3d.mpi-pcsamp\." > sweep3d_${thiscompiler}_openmpi_create_pcsamp
echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi pcsamp experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_openmpi_create_pcsamp > new_input.script
echo "log -f sweep3d_${thiscompiler}_openmpi_pcsamp_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script
#
grep "source_" sweep3d_${thiscompiler}_openmpi_pcsamp_results.log | cat > sweep3d_${thiscompiler}_openmpi_pcsamp_results.status
#
echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi pcsamp experiment"
echo "-------------------------------------"
#
#
# Run usertime and analyze the results
#
openss -offline -f "\$mpicommand -np \$RankCount \$executable" usertime
ls *.openss | grep "sweep3d.mpi-usertime\." > sweep3d_${thiscompiler}_openmpi_create_usertime
#
echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi usertime experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_openmpi_create_usertime > new_input.script
echo "log -f sweep3d_${thiscompiler}_openmpi_usertime_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script
#
grep "source_" sweep3d_${thiscompiler}_openmpi_usertime_results.log | cat > sweep3d_${thiscompiler}_openmpi_usertime_results.status
#
echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi usertime experiment"
echo "-------------------------------------"
echo ""
#
openss -offline -f "\$mpicommand -np \$RankCount \$executable" hwc
ls *.openss | grep "sweep3d.mpi-hwc\." > sweep3d_${thiscompiler}_openmpi_create_hwc
#
echo ""
echo "BEGIN Analyzing sweep3d.mpi hwc experiment"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_openmpi_create_hwc > new_input.script
echo "log -f sweep3d_${thiscompiler}_openmpi_hwc_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "source_" sweep3d_${thiscompiler}_openmpi_hwc_results.log | cat > sweep3d_${thiscompiler}_openmpi_hwc_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi hwc experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount \$executable" hwctime
ls *.openss | grep "sweep3d.mpi-hwctime\." > sweep3d_${thiscompiler}_openmpi_create_hwctime

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi hwctime experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_openmpi_create_hwctime > new_input.script
echo "log -f sweep3d_${thiscompiler}_openmpi_hwctime_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "source_" sweep3d_${thiscompiler}_openmpi_hwctime_results.log | cat > sweep3d_${thiscompiler}_openmpi_hwctime_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi hwctime experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount \$executable" io
ls *.openss | grep "sweep3d.mpi-io\." > sweep3d_${thiscompiler}_openmpi_create_io

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi io experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_openmpi_create_io > new_input.script
echo "log -f sweep3d_${thiscompiler}_openmpi_io_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "libc_read" sweep3d_${thiscompiler}_openmpi_io_results.log | cat > sweep3d_${thiscompiler}_openmpi_io_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi io experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount \$executable" iot
ls *.openss | grep "sweep3d.mpi-iot\." > sweep3d_${thiscompiler}_openmpi_create_iot

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi iot experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_openmpi_create_iot > new_input.script
echo "log -f sweep3d_${thiscompiler}_openmpi_iot_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "libc_read" sweep3d_${thiscompiler}_openmpi_iot_results.log | cat > sweep3d_${thiscompiler}_openmpi_iot_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi iot experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount \$executable" mpi
ls *.openss | grep "sweep3d.mpi-mpi\-" > sweep3d_${thiscompiler}_openmpi_create_mpi

echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi mpi experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_openmpi_create_mpi > new_input.script
echo "log -f sweep3d_${thiscompiler}_openmpi_mpi_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "\$RankCount *MPI_Finalize" sweep3d_${thiscompiler}_openmpi_mpi_results.log | cat > sweep3d_${thiscompiler}_openmpi_mpi_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi mpi experiment"
echo "-------------------------------------"
echo ""


openss -offline -f "\$mpicommand -np \$RankCount \$executable" mpit

#
# Find corresponding experiment database files and create files that can be used to restore the databases
#
ls *.openss | grep "sweep3d.mpi-mpit\-" > sweep3d_${thiscompiler}_openmpi_create_mpit

#
# Use the corresponding experiment database file names to restore the database and print out the status and results for the experiments
#
echo ""
echo "-------------------------------------"
echo "BEGIN Analyzing sweep3d.mpi mpit experiment"
echo "-------------------------------------"
sed 's/^/exprestore -f /' sweep3d_${thiscompiler}_openmpi_create_mpit > new_input.script
echo "log -f sweep3d_${thiscompiler}_openmpi_mpit_results.log" >> new_input.script
cat common_commands >> new_input.script
openss -batch < new_input.script

grep "\$RankCount *MPI_Finalize" sweep3d_${thiscompiler}_openmpi_mpit_results.log | cat > sweep3d_${thiscompiler}_openmpi_mpit_results.status

echo "-------------------------------------"
echo "END Analyzing sweep3d.mpi mpit experiment"
echo "-------------------------------------"
echo ""

# script to send test results email
# email subject
set SUBJECT = "OpenSpeedShop MPI sweep3d.mpi executable test results"
# Email To ?
set EMAIL = \$EmailAddress
# Email text/message
set EMAILMESSAGE = "/tmp/openss_test_message.txt"
echo " OpenSpeedShop Performance Tool SWEEP3D.MPI Test Results" >\$EMAILMESSAGE
echo "" >>\$EMAILMESSAGE
#echo " Intel compiler, \$NodeCount Nodes \$RankCount ranks">> \$EMAILMESSAGE
echo " \${testCompiler} Compiler \$NodeCount Nodes \$RankCount ranks">> \$EMAILMESSAGE
set thisuname = "\`uname -srnmo\`"
echo " System uname: " \$thisuname>> \$EMAILMESSAGE
set thisdate = "\`date\`"
echo " Date/time of run: " \$thisdate>> \$EMAILMESSAGE
set thisdir = "\`pwd\`"
echo " Directory where tests were run: " \$thisdir>> \$EMAILMESSAGE
echo "" >>\$EMAILMESSAGE
#
set testval = `ls *.status | wc -l`
echo " Expected tests: 8">> \$EMAILMESSAGE
echo " Completed tests: " \$testval>> \$EMAILMESSAGE
#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e sweep3d_${thiscompiler}_openmpi_pcsamp_results.status ) then
  if ( -s sweep3d_${thiscompiler}_openmpi_pcsamp_results.status ) then
    echo " PCSAMP EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " PCSAMP EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " PCSAMP EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e sweep3d_${thiscompiler}_openmpi_usertime_results.status ) then
  if ( -s sweep3d_${thiscompiler}_openmpi_usertime_results.status ) then
    echo " USERTIME EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " USERTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " USERTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e sweep3d_${thiscompiler}_openmpi_hwc_results.status ) then
  if ( -s sweep3d_${thiscompiler}_openmpi_usertime_results.status ) then
    echo " HWC EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " HWC EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " HWC EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e sweep3d_${thiscompiler}_openmpi_hwctime_results.status ) then
  if ( -s sweep3d_${thiscompiler}_openmpi_hwctime_results.status ) then
    echo " HWCTIME EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " HWCTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " HWCTIME EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e sweep3d_${thiscompiler}_openmpi_io_results.status ) then
  if ( -s sweep3d_${thiscompiler}_openmpi_io_results.status ) then
    echo " I/O EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " I/O EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " I/O EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e sweep3d_${thiscompiler}_openmpi_iot_results.status ) then
  if ( -s sweep3d_${thiscompiler}_openmpi_iot_results.status ) then
    echo " I/O TRACE EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " I/O TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " I/O TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e sweep3d_${thiscompiler}_openmpi_mpi_results.status ) then
  if ( -s sweep3d_${thiscompiler}_openmpi_mpi_results.status ) then
    echo " MPI EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " MPI EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " MPI EXPERIMENT FAILED">> \$EMAILMESSAGE
endif

#
# if the status file exists and has data then the test succeeded
# if the status file does not exist then the test did not complete and consider it failed
#
if ( -e sweep3d_${thiscompiler}_openmpi_mpit_results.status ) then
  if ( -s sweep3d_${thiscompiler}_openmpi_mpit_results.status ) then
    echo " MPI TRACE EXPERIMENT PASSED">> \$EMAILMESSAGE
  else
    echo " MPI TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
  endif
else
  echo " MPI TRACE EXPERIMENT FAILED">> \$EMAILMESSAGE
endif
#echo "This is an email message test">> \$EMAILMESSAGE
#echo "This is email text" >>\$EMAILMESSAGE
# send an email using /bin/mail
/bin/mail -s "\$SUBJECT" "-c \$EMAIL" "\$EMAIL" < \$EMAILMESSAGE

#
# End of nested script
#
exit
EOF

echo "NOTE: AFTER Creating  sweep3d_${thiscompiler}_openmpi_script.tcsh"

echo "pwd=`pwd`"

set RUN_DIR = $testpath
echo "RUN_DIR=$RUN_DIR"
cd $RUN_DIR
set REQ_WALLTIME = 1:00
set REQ_NNODES = $NodeCount
echo "REQ_NNODES = $REQ_NNODES"
set REQ_SCRIPT = "$RUN_DIR/sweep3d_${thiscompiler}_openmpi_script.tcsh"

echo "NOTE: BEFORE Creating  moab_sweep3d_${thiscompiler}_openmpi_script.tcsh"

cat > moab_sweep3d_${thiscompiler}_openmpi_script.tcsh << EOF
#!/bin/tcsh
#MSUB -o $testpath/sweep3d_${thiscompiler}_openmpi_output.txt
#MSUB -j oe
#MSUB -l nodes="$NodeCount":ppn=8
#MSUB -l walltime=2400
#MSUB -N  $testpath/sweep3d_${thiscompiler}_openmpi_script.tcsh

echo Shell is $SHELL
echo Program is $0

echo "BEFORE: Kick off $testpath/sweep3d_${thiscompiler}_openmpi_script.tcsh"
tcsh  $testpath/sweep3d_${thiscompiler}_openmpi_script.tcsh
echo "AFTER: Kick off $testpath/sweep3d_${thiscompiler}_openmpi_script.tcsh"
EOF

echo "NOTE: AFTER Creating  moab_sweep3d_${thiscompiler}_openmpi_script.tcsh"

/opt/MOAB/bin/msub moab_sweep3d_${thiscompiler}_openmpi_script.tcsh

endif

cd $testpathbase
echo "bottom of the compiler loop, testpathbase=" $testpathbase

# end loop through the compiler list
end

# end loop through the test list
end

exit

cd $testpathbase 

