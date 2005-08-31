#!/bin/sh
rm -f input.script
../../../../executables/forever_cxx/forever >> file &
ls -l file
cat file
read i < file
rm file
echo expCreate  -p $i  pcsamp >> input.script
echo expCreate -f ../../../../executables/mutatee/cplus_version/mutatee usertime >>input.script
echo expgo -x 1 >> input.script
echo expgo -x 2 >> input.script
#echo expview stats5 -m usertime::inclusive_time >> input.script
echo exit >> input.script
openss -batch < input.script
kill $i

