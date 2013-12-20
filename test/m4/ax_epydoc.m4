################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
# Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.
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

################################################################################
# Check for EPYDOC (http://epydoc.sourceforge.net/)   
################################################################################

AC_DEFUN([AX_EPYDOC], [

	AC_MSG_CHECKING([for epydoc binary])
	if epydoc --version >/dev/null 2>/dev/null ; then
      		AC_MSG_CHECKING([found epydoc binary])
		      AC_MSG_RESULT(yes)
		      AM_CONDITIONAL(HAVE_EPYDOC, true)
		      AC_DEFINE(HAVE_EPYDOC, 1, [Define to 1 if you have EPYDOC.])
	else
                AM_CONDITIONAL(HAVE_EPYDOC, false)
	fi
])
