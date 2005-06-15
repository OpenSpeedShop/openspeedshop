#! /bin/tcsh
#./mutator localhost $cwd/mutatee 30
$cwd/Eon/eon &
set pid=$!
./mutator localhost $pid

