################################################################################# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
noinst_PROGRAMS = eut_diag mutatee

CFLAGS  = \
         -g -gdwarf-2

CXXFLAGS        = \
         -g -gdwarf-2

CPPFLAGS        = \
         -g -gdwarf-2

eut_diag_CXXFLAGS = \
         $(DPCL_CPPFLAGS)

eut_diag_LDFLAGS = \
        -L$(ROOT)/lib \
        $(DPCL_LDFLAGS) \
        $(DPCL_LIBS) -lelf

eut_diag_LIBS = \
        $(DPCL_LDFLAGS) \
        $(DPCL_LIBS) -lelf

eut_diag_SOURCES = \
        eut_diag.cxx

mutatee_CFLAGS = \
         $(DPCL_CPPFLAGS) \
         $(CPPFLAGS)

mutatee_LDADD = \
        -ldl

mutatee_SOURCES = \
        mutatee.c


EXTRA_DIST = \
        README \
        eut_diag.tst \
        eut_diag.tst.expected \
        runone-dpcl \
        runall-dpcl
# This software is subject to the terms of the IBM Public License            
# You must accept the terms of this license to use this software.            
#                                                                           
# Copyright (C) 2001, International Business Machines Corporation            
#                                                                            
# Further information about the IBM Public License Version 1.0 is obtained   
# from URL http://oss.software.ibm.com/developer/opensource/license10.html   
#(user,"%Z% %I% %W% %D% %T%")
.SUFFIXES:      .C 

code=eut_diag
#install = /opt/dpcl

INCDIR	= /usr/include/dpcl
INC	= -I$(INCDIR)

LIBLOC	= -L/usr/lib 

#LIB	= -ldpcl -lefence -lelf
LIB	= -ldpcl -lelf -lunwind
#CCFLAGS	= -pg -g -gdwarf_2 -D__64BIT__
CCFLAGS	= -pg -g 

all:	${code} 

.C.o:
	$(CXX) $(CCFLAGS) $(INC) -c $(<)

${code}:	${code}.o
	$(CXX) -o ${code} ${code}.o $(LIB) $(LIBLOC)

clean:
	/bin/rm -rf ${code} $(code).o

