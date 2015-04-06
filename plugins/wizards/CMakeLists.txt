################################################################################
# Copyright (c) 2014-2015 Krell Institute. All Rights Reserved.
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

if(QT_FOUND)
add_subdirectory(pcSampleWizardPanel)
add_subdirectory(IntroWizardPanel)
add_subdirectory(CompareWizardPanel)
add_subdirectory(FPE_TracingWizardPanel)

if(PAPI_FOUND)
add_subdirectory(HW_CounterSampWizardPanel)
add_subdirectory(HW_CounterWizardPanel)
endif()

add_subdirectory(IOWizardPanel)
add_subdirectory(MPIWizardPanel)
add_subdirectory(UserTimeWizardPanel)
endif()
