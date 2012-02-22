#!/bin/sh
################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2010 Krell Institute All Rights Reserved.
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
  exit
fi
if test -d plugins/panels
then
./configure --prefix=/home/dew/oss --libdir=/home/dew/oss/lib64 --with-instrumentor=offline --with-tls=implicit --with-libdwarf=/home/dew/oss --with-binutils=/usr --with-libunwind=/home/dew/oss --with-papi=/home/dew/oss --with-sqlite=/home/dew/oss --with-qtlib=/usr/lib64/qt-3.3 --with-dyninst=/home/dew/oss --with-dyninst-version=7.0 --with-lampi= --with-openmpi=/opt/openmpi-1.4.3 --with-mpt= --with-mpich= --with-mpich2= --with-mpich2-driver= --with-mvapich= --with-mvapich2= --with-mvapich2-ofed= --with-lam= --with-mpich-driver= --with-otf=/home/dew/oss --with-vt=/home/dew/oss --with-libmonitor=/home/dew/oss --with-mrnet=/home/dew/oss --with-mrnet-version=3.0.1 --with-python=/usr; make; make install

#./configure --prefix=$OPENSS_PREFIX --libdir=$OPENSS_PREFIX/lib64 --with-instrumentor=mrnet --with-tls=implicit --with-libdwarf=$OPENSS_PREFIX --with-binutils=/usr --with-libunwind=$OPENSS_PREFIX --with-papi=$OPENSS_PREFIX --with-sqlite=$OPENSS_PREFIX --with-qtlib=/usr/lib64/qt-3.3 --with-dyninst=$OPENSS_PREFIX --with-dyninst-version=6.1 --with-lampi= --with-openmpi=$OPENSS_MPI_OPENMPI --with-mpt= --with-mpich= --with-mpich2=$OPENSS_MPI_MPICH2 --with-mpich2-driver= --with-mvapich=$OPENSS_MPI_MVAPICH --with-mvapich2=$OPENSS_MPI_MVAPICH2 --with-mvapich2-ofed= --with-lam= --with-mpich-driver= --with-otf=$OPENSS_PREFIX --with-vt=$OPENSS_PREFIX --with-libmonitor=$OPENSS_PREFIX --with-mrnet=$OPENSS_PREFIX --with-mrnet-version=3.0.1 --with-python=/usr; make clean; make; make install

else
  echo NOTE: This must be run from the 'current' directory...
fi
