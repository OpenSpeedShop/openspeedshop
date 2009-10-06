#!/bin/sh
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

if [ "$OPENSS_PREFIX" = "" ]
then
  echo "OPENSS_PREFIX not set."
  echo "For oss developers that's usually .../GUI/plugin/lib/openspeedshop"
  echo "- or -"
  echo "export OPENSS_PREFIX=$OPENSS_PLUGIN_PATH/../.."
  if [ "$OPENSS_PLUGIN_PATH" != "" ]
  then
    export OPENSS_PREFIX=$OPENSS_PLUGIN_PATH/../..;
    echo "WARNING: Defaulting to developers location..."
  fi 
#  exit
fi
if test -d plugins/panels
then
./configure --prefix=/opt/OSSCRAY --with-instrumentor=mrnet --with-tls=implicit --with-libdwarf=/opt/OSS-mrnet --with-binutils=/usr --with-libunwind=/opt/OSS-mrnet --with-papi=/opt/OSS-mrnet --with-sqlite=/opt/OSS-mrnet --with-qtlib=/usr/lib64/qt-3.3 --with-dyninst=/opt/OSS-mrnet --with-mpich2=/opt/mpich2-1.1  --with-otf=/opt/OSS-mrnet --with-vt=/opt/OSS-mrnet --with-libmonitor=/opt/OSS-mrnet --with-mrnet=/opt/OSS-mrnet --with-openmpi=/opt/openmpi-1.2.8 ;make clean; make;make install;make dist;
#./configure --prefix=/opt/OSSCRAY --with-instrumentor=mrnet --with-tls=implicit --with-libdwarf=/opt/OSS-mrnet --with-binutils=/usr --with-libunwind=/opt/OSS-mrnet --with-papi=/opt/OSS-mrnet --with-sqlite=/opt/OSS-mrnet --with-qtlib=/usr/lib64/qt-3.3 --with-dyninst=/opt/OSS-mrnet --with-dpcl=/opt/OSS-mrnet --with-lampi=/opt/lampi-1.5.16 --with-openmpi=/opt/openmpi-1.2.6 --with-mpt= --with-mpich= --with-mpich2=/opt/mpich2-1.1 --with-mvapich2= --with-lam= --with-mpich-driver= --with-otf=/opt/OSS-mrnet --with-vt=/opt/OSS-mrnet --with-libmonitor=/opt/OSS-mrnet --with-mrnet=/opt/OSS-mrnet ;make clean; make;make install;make dist;
else
  echo NOTE: This must be run from the 'current' directory...
fi
