################################################################################
# Copyright (c) 2013 Krell Institute All Rights Reserved.
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

noinst_PROGRAMS = stress_omp

FFLAGS = -O1 -g -gdwarf-2
#gfortran -D__GFORTRAN -g -O -fopenmp -ostress_omp stress_omp.f -lm -lgomp


stress_omp_FFLAGS = \
	-lm -D__GFORTRAN -fopenmp 

stress_omp_LDADD = \
	-ldl

stress_omp_SOURCES = \
	stress_omp.f
