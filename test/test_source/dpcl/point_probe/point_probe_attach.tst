#! /bin/tcsh
#./mutator 301 localhost $cwd/mutatee 301
$cwd/mutatee 301 > LOG &
sleep 10
set pid = `ps -ef | grep "\.\/mutatee" | cut -c 10-14`
$cwd/mutator 301 "$HOSTNAME" $pid

