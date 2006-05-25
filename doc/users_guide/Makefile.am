###############################################################################
# Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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

# directories that will be affected
SUBDIRS = TroubleShootingGuide

DIST_SUBDIRS = TroubleShootingGuide

EXTRA_DIST = BuildingInstallingDPCL.html \
	ExperimentMenuItems.png \
	FileMenu.png \
	HelpMenu.png \
	index.html \
	InitialPanel.png \
	IntroWizardRightMouseDown.png \
	Makefile.am \
	OpenSpeedShop_Logo.jpg \
	OpenSpeedShopPackageNotes.html \
	PC_SampleExperiment-2.png \
	PC_SampleExperiment-4-Split.png \
	PC_SampleExperiment-5-f3click.png \
	PC_SampleExperiment-non-wizard-1.png \
	PC_SampleExperiment-non-wizard-2.png \
	PC_SampleExperiment-non-wizard-3.png \
	PC_SampleExperiment-non-wizard-4.png \
	PC_SampleExperiment-non-wizard-5.png \
	PC_SampleExperiment-non-wizard-6.png \
	PC_SampleExperiment-non-wizard-7.png \
	PC_SampleWizard-AttachToRunningProcess-fred1.png \
	PC_SampleWizard-AttachToRunningProcess-fred.png \
	PC_SampleWizard-AttachToRunningProcess-GUI-nonwizard1.png \
	PC_SampleWizard-AttachToRunningProcess-GUI-nonwizard.png \
	PC_SampleWizard-AttachToRunningProcess-MPIJOB1.png \
	PC_SampleWizard-AttachToRunningProcess.png \
	PCSampling-StatsPanel.png \
	PCSampling-StatsPanel-Split.png \
	PC_SamplingWizard-2.png \
	PC_SamplingWizard-3A.png \
	PC_SamplingWizard-3.png \
	PC_SamplingWizard-4.png \
	PC_SamplingWizard-5.png \
	PC_SamplingWizard-6_OR_PC_SampleExperiment-1.png \
	Preferences-General.png \
	Preferences-Source.png \
	Preferences-Stats.png \
	Preferences-ManageProcessPanel.png \
	rpmQuery.asc \
	SourcePanel.png \
	StatsPanel.png \
	ToolsMenu.png \
	valid-html401.png \
	WizardMenuItems.png \
	HWC-WizardInto.png  \
	HWC-Wizard2.png  \
	UserTimeIntroWizard.png  \
	UserTimeWizard2.png  \
	UserTimeWizardArgs.png  \
	UserTimeWizardExecutables.png  \
	UserTimeWizardLoadFred.png  \
	UserTimeWizardSummary.png  \
	UserTimeExperiment.png  \
	UserTimeExperimentResults.png


package = @PACKAGE@
packver = @PACKAGE@-@VERSION@
#docdir  = ${prefix}/share/doc/@PACKAGE@-@VERSION@/users_guide
#docdir2  = ${prefix}/share/doc/@PACKAGE@-@VERSION@/users_guide/TroubleShootingGuide
#docdir3  = ${prefix}/share/doc/@PACKAGE@-@VERSION@
docdir  = ${prefix}/share/doc/packages/OpenSpeedShop/users_guide
docdir2  = ${prefix}/share/doc/packages/OpenSpeedShop/users_guide/TroubleShootingGuide
docdir3  = ${prefix}/share/doc/packages/OpenSpeedShop

install-reference: index.html
	@$(NORMAL_INSTALL)
	${mkinstalldirs} ${DESTDIR}${docdir}
	${mkinstalldirs} ${DESTDIR}${docdir2}
	if test -f index.html ; then \
		for file in *.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir}/$$basefile; \
		done ; \
		for file in *.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir}/$$basefile; \
		done ; \
		${INSTALL_DATA} OpenSpeedShop_Logo.jpg ${DESTDIR}${docdir} ;\
	fi
	if test -f TroubleShootingGuide/index.html ; then \
		for file in TroubleShootingGuide/*.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir2}/$$basefile; \
		done ; \
		for file in TroubleShootingGuide/*.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir2}/$$basefile; \
		done ; \
		${INSTALL_DATA} TroubleShootingGuide/doxygen.css ${DESTDIR}${docdir2} ;\
	fi

uninstall-reference: index.html
	@$(NORMAL_UNINSTALL)
	@dir='${<D}'; \
	echo "dir =@dir"
	for file in $$dir/* ; do \
		basefile=`echo $$file | sed -e 's,^.*/,,'`; \
		rm -f ${DESTDIR}${docdir}/$$basefile; \
		rm -f ${DESTDIR}${docdir2}/$$basefile; \
	done
	-rm -rf ${DESTDIR}${docdir}/
	-rm -rf ${DESTDIR}${docdir2}/
	-rm -rf ${DESTDIR}${docdir3}/

all-local: index.html

install-data-am: install-reference

uninstall: uninstall-reference
