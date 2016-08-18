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
add_subdirectory(SourcePanel)
add_subdirectory(CmdPanel)
add_subdirectory(CustomExperimentPanel)
add_subdirectory(CompareExperimentsPanel)
add_subdirectory(StatsPanel)
add_subdirectory(ManageProcessesPanel)
add_subdirectory(CustomizeStatsPanel)
add_subdirectory(loadPanel)
add_subdirectory(DebugPanel)

add_subdirectory(pcSamplePanel)
add_subdirectory(UserTimePanel)
add_subdirectory(IOPanel)
add_subdirectory(IOPPanel)
add_subdirectory(IOTPanel)
add_subdirectory(FPE_TracingPanel)
add_subdirectory(MPIPanel)
add_subdirectory(MPITPanel)

if (OTF_FOUND)
if (VT_FOUND)
add_subdirectory(MPIOTFPanel)
endif()
endif()

add_subdirectory(MPIPPanel)
add_subdirectory(MEMPanel)
add_subdirectory(PTHREADSPanel)

# Take out this if PAPI_FOUND check so that OSS/CBTF builds the
# hwc* panels: if(PAPI_FOUND)

add_subdirectory(HW_CounterPanel)
add_subdirectory(HWCTimePanel)
add_subdirectory(HWCSampPanel)

# Take out this if PAPI_FOUND check so that OSS/CBTF builds the
# hwc* panels: endif()

add_subdirectory(OMPTPPanel)
endif()
