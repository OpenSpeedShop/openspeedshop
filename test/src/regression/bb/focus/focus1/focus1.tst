#!/bin/sh
#
#Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
#
rm -f input.script
../../../../../executables/sequential/forever_cxx/forever >> file &
ls -l file
cat file
read i < file
rm file
../../../../../executables/sequential/forever_cxx/forever >> file &
ls -l file
cat file
read j < file
rm file
echo expCreate  -p $i pcsamp>> input.script
echo expCreate -p $j pcsamp >> input.script
echo expfocus  >> input.script
echo expfocus -x 1 >> input.script
echo expfocus >> input.script
echo exit >> input.script
openss -batch < input.script
kill -n 9 $i $j
