#!/bin/sh
if [ "$OPENSPEEDSHOP_INSTALL_DIR" = "" ]
then
  echo "OPENSPEEDSHOP_INSTALL_DIR not set."
  echo "For oss developers that's usually .../GUI/plugin/lib/openspeedshop"
  echo "- or -"
  echo "export OPENSPEEDSHOP_INSTALL_DIR=$OPENSPEEDSHOP_PLUGIN_PATH/../.."
  if [ "$OPENSPEEDSHOP_PLUGIN_PATH" != "" ]
  then
    export OPENSPEEDSHOP_INSTALL_DIR=$OPENSPEEDSHOP_PLUGIN_PATH/../..;
    echo "WARNING: Defaulting to developers location..."
  fi 
#  exit
fi
if test -d Panels
then
  bootstrap --clean;bootstrap;configure --prefix=$OPENSPEEDSHOP_INSTALL_DIR;make uninstall;make install;
else
  echo NOTE: This must be run from the GUI directory...
fi
