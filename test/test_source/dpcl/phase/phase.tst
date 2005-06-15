#! /bin/tcsh
#./mutator localhost $cwd/mutatee 30
../../../executables/Eon/eon &
set pid=$!
./mutator localhost $pid
rm /tmp/probe_module*
