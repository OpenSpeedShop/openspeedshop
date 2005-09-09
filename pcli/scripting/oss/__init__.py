"""
    
    B{OpenSpeedShop} I{python} interface module.
    
    The idea is to match the standard B{OpenSpeedShop}
    commandline interface using a pure I{python} API.
    
    A simple example::
    
    	import oss

    	# Create a FileList object with the name of
	# the application to analyse.
    	my_file = oss.FileList("myprog")
	
	# Determine the type of experiment to run.
	my_exptype = oss.ExpTypeList("pcsamp")

    	# Register the experiment to be run.
	my_id = oss.expCreate(my_file,my_exptype)
	
	# Run the instrumented application
	oss.expGo()

    	# Define what metrics we want to use. In this case
	# it is exclusive time.
    	my_metric_list = oss.MetricList()
    	my_metric_list += "exclusive"

    	# Describe the type of view we want and get the 
	# results of the experiment.
	my_viewtype = oss.ViewTypeList()
    	my_viewtype += "pcsamp"
	result = oss.expView(my_id,my_viewtype,my_metric_list)

    	# Print out the results. In this case the results are
	# in a double array. Normally either the structure of
	# the return value will be known or one will have to 
	# query each of the object elements.
    	r_count = len(ret)
    	for row_ndx in range(r_count):
    	    print " "
    	    row =ret[row_ndx]
    	    c_count = len(row)
    	    for rel_ndx in range(c_count):
            	print row[rel_ndx]

"""
__all__ = ["oss_commands","oss_parse","oss_classes"]

from oss_classes import *
from oss_parse import *
from oss_commands import *

