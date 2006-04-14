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

export MPI_TYPE_DEPTH=16
export MPI_TYPE_MAX=65536

mpirun -np 2 ../../../../executables/mpi/nbody/nbody < /dev/null > /dev/null &

mpipid=mpipid.$$
rm -rf $mpipid
ps r -C mpirun -o pid --no-heading > $mpipid
ls -1 $mpipid
cat $mpipid
read i < $mpipid
rm -rf $mpipid

rm -rf input.script
echo expcreate -v mpi -p $i mpi >> input.script
echo expgo >> input.script
echo ! sleep 180 >> input.script
echo expview -v nowait >>input.script
openss -batch < input.script
kill -9 $i

