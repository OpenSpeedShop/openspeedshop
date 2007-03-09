#!/bin/sh
###############################################################################
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
#export DYNINSTAPI_RT_LIB=/usr/lib/libdyninstAPI_RT.so.1
BUILDPROCESSOR=`uname -p`
if [ "$DYNINSTAPI_RT_LIB" = "" ]
then
  case $BUILDPROCESSOR in
    x86_64)
      export DYNINSTAPI_RT_LIB=/usr/lib64/libdyninstAPI_RT.so.1
      ;;
    *)
      export DYNINSTAPI_RT_LIB=/usr/lib/libdyninstAPI_RT.so.1
      ;;
  esac
fi

./mutator ./mutatee
killall mutator mutatee
