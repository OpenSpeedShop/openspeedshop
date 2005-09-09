#!/bin/sh
rm -f input.script
rm -f *.expected*
../../../../../executables/forever_cxx/forever >> file &
ls -l file
cat file
read i < file
echo $i >> pids2.tst.expected
rm file
echo expCreate  -p $i usertime >> input.script
echo listpids >> input.script
echo exit >> input.script
openss -batch < input.script
kill $i
