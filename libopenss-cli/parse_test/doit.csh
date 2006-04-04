#!/bin/csh

#Then loop through all the input files creating
#output files. Here is how it is done in tcsh:

echo "GENERATE OUTPUT"
echo ""
    foreach file (`cat file.list`)
#    	echo ""
#    	echo $file
    	./parser $file.input > $file.input.out
    end

#To compare the results against checkedin results:

echo "GENERATE DIFFS"
echo ""

    foreach file (`cat file.list`)
#    	echo ""
#    	echo $file
    	diff -bB $file.input.out $file.output > $file.diff
    end

#All the diff files should be empty.

echo "wc *.diff"
echo ""

wc *.diff

#You should be able to:
#    grep -i syntax
#or 
#    grep -i error

#all the *.input.out files without having a hit.


