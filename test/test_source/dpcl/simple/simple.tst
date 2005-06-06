#! /bin/tcsh
./mutator $cwd/mutatee
pkill -9 mutatee
pkill -9 mutator
