#! /bin/tcsh
./mutator 30 localhost $cwd/mutatee 30
pkill -9 mutatee

