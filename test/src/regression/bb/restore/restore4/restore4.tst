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
rm -f input*
rm -f results.data
echo expCreate  -f ../../../../../executables/sequential/mutatee/cplus_version/mutatee hwctime>> input1.script
echo expgo >> input1.script 
echo wait >> input1.script
#echo expview  >> input1.script
echo expSave -f results.data >> input1.script
echo exit >> input1.script
openss -batch < input1.script
echo exprestore -f results.data >> input2.script
#echo expgo >> input2.script
echo expview stats -m inclusive_overflows >> input2.script
echo exit >> input2.script
openss -batch < input2.script
