rm -f oss/*.py oss/*.pyc
rm -rf doc
mkdir doc
# Copy over raw python sources to package directory
cp sources/*py oss
chmod 644 oss/*.py
# Generate expdoc documentation
#epydoc -o doc -t __init__.py -n oss __init__.py oss_*.py 
epydoc -o doc -t oss/__init__.py -n oss oss/oss*.py oss/__init__.py
# Preprocess out epytext and copy to package directory
sed -f epytext.sed < sources/__init__.py > oss/__init__.py
sed -f epytext.sed < sources/oss_classes.py > oss/oss_classes.py
sed -f epytext.sed < sources/oss_commands.py > oss/oss_commands.py
sed -f epytext.sed < sources/oss_parse.py > oss/oss_parse.py
