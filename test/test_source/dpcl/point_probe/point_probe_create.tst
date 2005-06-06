#! /bin/tcsh
#echo $HOSTNAME
./mutator 301 "$HOSTNAME" $cwd/mutatee 301
pkill -9 mutatee
