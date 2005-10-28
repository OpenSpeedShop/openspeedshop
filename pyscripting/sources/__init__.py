################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
    
    The idea is to match the standard B{OpenSpeedShop}
    commandline interface using a pure I{python} API.
    
    Generally, one wants to create an experiment, associate
    it with the application to be profiled, describe the
    type of data to be collected. 
    
    After the experiment is defined one runs it, either
    letting it run its course or partially. This generates
    raw information that is accessed with the B{expView()}
    command. There may be multiple views available for
    a particular experiment type (See B{list()}) and can be
    set with the B{expSetParam()} command.
    
    A simple example::
    
    	import openss

    	# Create a FileList object with the name of
	# the application to analyse.
    	my_file = openss.FileList("myprog")
	
	# Determine the type of experiment to run.
	my_exptype = openss.ExpTypeList("pcsamp")

    	# Register the experiment to be run.
	my_id = openss.expCreate(my_file,my_exptype)
	
	# Run the instrumented application
	openss.expGo()

    	# expGo is the only asynchrous command. We may need
	# to wait until our app is done before harvesting
	# the data.
    	openss.waitForGo()
	
    	# Describe the type of view we want and get the 
	# results of the experiment.
	my_viewtype = openss.ViewTypeList()
    	my_viewtype += "pcsamp"
	result = openss.expView(my_id,my_viewtype)

    	# Print out the results. In this case the results are
	# in a double array. Normally either the structure of
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

