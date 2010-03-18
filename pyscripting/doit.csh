#!/bin/csh

# Set the stage

# This is just a quick and dirty way of setting things
# up. Before releasing to the public this needs to be
# done in the configure stage.
rm -f openss/*.py openss/*.pyc
rm -rf doc
rm -rf test/openss
mkdir doc

# Copy over raw python sources to package directory
cp sources/*py openss
chmod 644 openss/*.py

# Make link to cli dso into openss directory
cd openss

set mymach=`uname -m`

if ($mymach == "x86_64") then
  ln -s $OPENSS_PREFIX/lib64/libopenss-cli.so.0.0.0 PYopenss.so
else
  ln -s $OPENSS_PREFIX/lib/libopenss-cli.so.0.0.0 PYopenss.so
endif

cd ../.

# Generate expdoc documentation
#epydoc -o doc -t __init__.py -n openss __init__.py openss_*.py 
#epydoc -o doc -t openss/__init__.py -n openss openss/openss*.py openss/__init__.py
epydoc -o doc -n openss openss/openss*.py openss/__init__.py

# Preprocess out epytext and copy to package directory
#sed -f epytext.sed < sources/__init__.py > openss/__init__.py
#sed -f epytext.sed < sources/openss_classes.py > openss/openss_classes.py
#sed -f epytext.sed < sources/openss_commands.py > openss/openss_commands.py
# Create a link in the test directory to our package

cd test

ln -s ../openss .

cd ../.
