################################################################################
# License Applicability.  Except to the extent portions of this file are
# made subject to an alternative license as permitted in the SGI Free
# Software License B, Version 1.1 (the "License"), the contents of this
# file are subject only to the provisions of the License. You may not use
# this file except in compliance with the License. You may obtain a copy
# of the License at Silicon Graphics, Inc., attn: Legal Services, 1500
# Crittenden Lane, Moutain View, CA 94043, or at:
# 
#      http://oss.sgi.com/projects/FreeB
# 
# Note that, as provided in the License, the Software is distributed on
# on "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
# DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
# CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
# 
# Original Code.  The Original Code is: Open/Speedshop, Version 1.0, released
# <MONTH>, <DAY>, 2004, developed by Silicon Graphics, Inc. The Original
# Code is Copyright (c) 2004 Silicon Graphics, Inc. Copyright in any
# portions created by third parties is as indicated elsewhere herein.
# All Rights Reserved.
################################################################################

AC_INIT(OpenSpeedshop, 1.0)
AC_COPYRIGHT([[Copyright (c) 2004 Silicon Graphics, Inc. All Rights Reserved.]])
AM_INIT_AUTOMAKE

################################################################################
# Setup Configuration Header File
################################################################################

AM_CONFIG_HEADER([config.h])

AH_TOP([
/*******************************************************************************
** License Applicability.  Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1500
** Crittenden Lane, Moutain View, CA 94043, or at:
** 
**      http://oss.sgi.com/projects/FreeB
** 
** Note that, as provided in the License, the Software is distributed on
** on "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
** 
** Original Code.  The Original Code is: OpenSpeedshop, Version 1.0, released
** <MONTH>, <DAY>, <YEAR>, developed by Silicon Graphics, Inc. The Original
** Code is Copyright (c) <YEAR> Silicon Graphics, Inc. Copyright in any
** portions created by third parties is as indicated elsewhere herein.
** All Rights Reserved.
*******************************************************************************/
])

################################################################################
# Check for Standard Build Tools
################################################################################

AC_PROG_CXX

AC_DISABLE_STATIC
AC_LIBLTDL_INSTALLABLE
AC_SUBST(LTDLINCL)
AC_SUBST(LIBLTDL)
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_CONFIG_SUBDIRS(base objects expr_info plugin chart cli gui Panels Panels/pcSamplePanel Panels/SourcePanel Panels/CmdPanel Panels/TopPanel Panels/GettingStartedPanel/GettingStartedClass Panels/GettingStartedPanel Panels/PerformancePanel Panels/UserTimePanel Panels/HW_CounterPanel Panels/IOPanel Panels/FPE_TracingPanel Panels/CreateNewExperimentPanel Panels/Wizards/IntroWizardPanel Panels/Wizards/pcSampleWizardPanel Panels/Wizards/UserTimeWizardPanel Panels/Wizards/FPE_TracingWizardPanel Panels/Wizards/HW_CounterWizardPanel Panels/Wizards/IOWizardPanel )

AC_PROG_INSTALL

################################################################################
# Check for Dyninst (http://www.dyninst.org)
################################################################################

# TODO: implement checks for Dyninst (not optional)

################################################################################
# Check for DPCL (http://oss.software.ibm.com/developerworks/opensource/dpcl)
################################################################################

# TODO: implement checks for DPCL (not optional)


################################################################################
# Generate Output Files
################################################################################

AC_CONFIG_FILES(Makefile
		base/Makefile
		plugin/Makefile
		expr_info/Makefile
		objects/Makefile
		dialogs/Makefile
		chart/Makefile
		cli/Makefile
		gui/Makefile
		Panels/Makefile
		Panels/pcSamplePanel/Makefile
		Panels/UserTimePanel/Makefile
		Panels/SourcePanel/Makefile
		Panels/CmdPanel/Makefile
		Panels/TopPanel/Makefile
		Panels/GettingStartedPanel/GettingStartedClass/Makefile
		Panels/GettingStartedPanel/Makefile
		Panels/PerformancePanel/Makefile
		Panels/DebugPanel/Makefile
		Panels/HW_CounterPanel/Makefile
		Panels/IOPanel/Makefile
		Panels/FPE_TracingPanel/Makefile
		Panels/CreateNewExperimentPanel/Makefile
		Panels/Wizards/Makefile
		Panels/Wizards/IntroWizardPanel/Makefile
		Panels/Wizards/pcSampleWizardPanel/Makefile
		Panels/Wizards/UserTimeWizardPanel/Makefile
		Panels/Wizards/FPE_TracingWizardPanel/Makefile
		Panels/Wizards/HW_CounterWizardPanel/Makefile
		Panels/Wizards/IOWizardPanel/Makefile
)
AC_OUTPUT
