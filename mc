################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

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
