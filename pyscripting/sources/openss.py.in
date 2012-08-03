################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
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

"""
    
    B{OpenSpeedShop} I{python} interface package.
    
    This package is essentially another UI for
    B{OpenSpeedShop}. Currently this is done by converting the
    APIs objects into B{OpenSpeedShop} CLI (Command Line
    Interface) syntax passing them to a common parser and
    semantic handler. The rational for this method is to reduce
    duplicate code and make behavior consistant across UIs.
    
    To see which classes are available and what arguments
    they take, go to the B{Table of Contents} for the API and
    choose B{openss.openss_classes}.
    
    This also means that one needs to reference the 
    B{Open|SpeedShop User's Guide} for detailed descriptions
    of experiment setup and behavior, as well as details about
    experiment collectors and view options.

    B{OpenSpeedShop} is developed to be extensible through
    dynamic plugins. This means that some information may not
    be available in a static document. Some information may
    only be available through the help facilities of the GUI
    or CLI (Command Line Interface). It is strongly suggested
    that the B{OpenSpeedShop} I{python} API user become
    familiar with one or both of these UIs in order to glean
    information not available in the standard documentation. 

    If the underlying B{OpenSpeedShop} encounters an error
    it will throw a python exception: B{openss.error}.

    Generally, one wants to create an experiment, associate it
    with the application to be profiled, and describe the type of
    data to be collected. 
    
    After the experiment is defined one runs it, either
    letting it run its course or run partially. This generates raw
    information that is accessed with the B{expView()}
    command. There may be multiple views available for a
    particular experiment type (See B{list()}) and can be set
    with the B{expSetParam()} command.
    
    A simple example::
    
    	import openss

    	# Create a FileList object with the name of
	# the application to analyze.
    	my_file = openss.FileList("myprog")

	# If I wanted to pass an argument to "myprog"
	# I would have put it within the parenthesis
	# along with the executable name:
	#    my_file = openss.FileList("myprog 893")

	# Determine the type of experiment to run.
	my_exptype = openss.ExpTypeList("pcsamp")

    	# Register the experiment to be run.
	my_id = openss.expCreate(my_file,my_exptype)
	
	# Run the instrumented application
	openss.expGo()

    	# expGo is the only asynchronous command. We may need
	# to wait until our app is done before harvesting
	# the data.
    	openss.wait()
	
    	# Describe the type of view we want and get the 
	# results of the experiment.
	my_viewtype = openss.ViewTypeList()
    	my_viewtype += "pcsamp"
	result = openss.expView(my_id,my_viewtype)

    	# Print out the results. In this case the results are
	# in a 2 dimensional array. Normally either the structure of
	# the return value will be known or one will have to 
	# query each of the object elements.
    	r_count = len(result)
    	for row_ndx in range(r_count):
    	    print " "
    	    row =result[row_ndx]
    	    c_count = len(row)
    	    for rel_ndx in range(c_count):
            	print row[rel_ndx]

    	# Cleanup any intermediate openss files.
    	openss.exit()

    
"""
__all__ = ["openss_commands","openss_parse","openss_classes"]

from openss_classes import *
from openss_parse import *
from openss_commands import *

