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
# Original Code.  The Original Code is: FutureTool, Version 1.0, released
# <MONTH>, <DAY>, 2004, developed by Silicon Graphics, Inc. The Original
# Code is Copyright (c) 2004 Silicon Graphics, Inc. Copyright in any
# portions created by third parties is as indicated elsewhere herein.
# All Rights Reserved.
################################################################################

AC_INIT(FutureTool, 1.0)
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
** Original Code.  The Original Code is: FutureTool, Version 1.0, released
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
AC_CONFIG_SUBDIRS(base objects ftf expr_info plugin chart cli gui Panels Panels/ToolbarPanel Panels/pcSamplePanel Panels/pcSamplePanel/pcSamplePanelWizard Panels/pcSamplePanel/pcSamplePanelControl Panels/UserTimePanelWizard Panels/SourcePanel Panels/CmdPanel Panels/TopPanel Panels/GettingStartedPanel/GettingStartedClass Panels/GettingStartedPanel Panels/PerformancePanel Panels/UserTimePanel Panels/IntroWizardPanel )  

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
		ftf/Makefile
		expr_info/Makefile
		objects/Makefile
		ladDialog/Makefile
		chart/Makefile
		cli/Makefile
		gui/Makefile
		Panels/Makefile
		Panels/ToolbarPanel/Makefile
		Panels/pcSamplePanel/Makefile
		Panels/pcSamplePanel/pcSamplePanelWizard/Makefile
		Panels/pcSamplePanel/pcSamplePanelControl/Makefile
		Panels/UserTimePanel/Makefile
		Panels/UserTimePanel/UserTimePanelWizard/Makefile
		Panels/SourcePanel/Makefile
		Panels/CmdPanel/Makefile
		Panels/TopPanel/Makefile
		Panels/GettingStartedPanel/GettingStartedClass/Makefile
		Panels/GettingStartedPanel/Makefile
		Panels/PerformancePanel/Makefile
		Panels/IntroWizardPanel/Makefile
		Panels/DebugPanel/Makefile
)
AC_OUTPUT
