#!/bin/sh
rm -f input.script
../../../../../executables/forever_cxx/forever >> file &
ls -l file
cat file
read i < file
rm file
echo expCreate  -p $i hwctime>> input.script
echo expgo -x 1 >> input.script 
echo exppause -x 1 >> input.script
echo expstatus -x 1 >> input.script
echo exit >> input.script
openss -batch < input.script
kill $i
