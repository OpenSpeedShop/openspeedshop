rm -f oss/*.py oss/*.pyc
rm -rf oss/doc
mkdir oss/doc
cd oss
# Copy over raw python sources to package directory
cp ../sources/*py .
# Generate expdoc documentation
epydoc -o doc -t __init__.py -n oss __init__.py oss_*.py 
#epydoc -o doc -t oss/__init__.py -n oss oss/oss*.py oss/__init__.py
# Preprocess out epytext and copy to package directory
sed -f ../epytext.sed < ../sources/__init__.py > __init__.py
sed -f ../epytext.sed < ../sources/oss_classes.py > oss_classes.py
sed -f ../epytext.sed < ../sources/oss_commands.py > oss_commands.py
sed -f ../epytext.sed < ../sources/oss_parse.py > oss_parse.py
cd ..
