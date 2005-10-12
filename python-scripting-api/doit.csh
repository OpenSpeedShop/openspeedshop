# Set the stage
# This is just a quick and dirty way of setting things
# up. Before releasing to the public this needs to be
# done in the configure stage.
rm -f oss/*.py oss/*.pyc
rm -rf doc
rm -rf test/oss
mkdir doc

# Copy over raw python sources to package directory
cp sources/*py oss
chmod 644 oss/*.py

# Make link to cli dso into oss directory
cd oss
ln -s $OPENSS_INSTALL_DIR/lib/libopenss-cli.so.0.0.0 PY_Input.so
cd ..

# Generate expdoc documentation
#epydoc -o doc -t __init__.py -n oss __init__.py oss_*.py 
epydoc -o doc -t oss/__init__.py -n oss oss/oss*.py oss/__init__.py

# Preprocess out epytext and copy to package directory
sed -f epytext.sed < sources/__init__.py > oss/__init__.py
sed -f epytext.sed < sources/oss_classes.py > oss/oss_classes.py
sed -f epytext.sed < sources/oss_commands.py > oss/oss_commands.py
sed -f epytext.sed < sources/oss_parse.py > oss/oss_parse.py

# Create a link in the test directory to our package
cd test
ln -s ../oss .
cd ..
