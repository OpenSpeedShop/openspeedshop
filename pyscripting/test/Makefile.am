################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007-2010 Krell Institute  All Rights Reserved.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

EXTRA_DIST = clearBreak.py  expAttach.py   expDetach.py   expGo.py \
	expSetParam.py  listHosts.py    listPids.py    listThreads.py \
	exit_2.py      expClose.py    expDisable.py  expPause.py  \
	expView.py      listMetrics.py  listRanks.py   listTypes.py
	exit_3.py      expCompare.py  expEnable.py   expRestore.py  \
	listBreaks.py   listObj.py      listSrc.py     listViews.py \
	exit.py        expCreate.py   expFocus.py    expSave.py    \
	listExp.py      listParams.py   listStatus.py \
	runall-py  runone-py setBreak.csh README

check: 
	ln -s $(OPENSS_PREFIX)/$(abi_libdir_name)/libopenss-cli.so.0.0.0 ../openss/PYopenss.so; 
	ln -s ../openss openss 
clean:
	rm -rf openss/PYopenss.so openss
