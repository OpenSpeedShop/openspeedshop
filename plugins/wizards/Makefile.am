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

if HAVE_QTLIB
SUBDIRS = \
	IntroWizardPanel \
	CompareWizardPanel

if BUILD_INSTRUMENTOR
SUBDIRS += \
	pcSampleWizardPanel \
	UserTimeWizardPanel \
	FPE_TracingWizardPanel \
	IOWizardPanel \
	MPIWizardPanel
endif

if HAVE_PAPI
if BUILD_INSTRUMENTOR
SUBDIRS += HW_CounterWizardPanel
SUBDIRS += HW_CounterSampWizardPanel
endif
endif
 
DIST_SUBDIRS = $(SUBDIRS)
else

SUBDIRS =
DIST_SUBDIRS =

endif

