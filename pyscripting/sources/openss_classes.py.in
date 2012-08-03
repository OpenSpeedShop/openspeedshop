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

import types

class OssList:
    "Base class for all lists"
    # 
    def __init__(self,input_list=None) :

    	"""
	Construct a list
	
	input_list currenty handles::
	    - An empty list
	    - An individual integer
	    - An individual long integer
	    - an individual string
	
	Any other type or nothing will produce and
	empty list.
	"""

    	t_list = []

    	if input_list is None:
    	    input_list=[]
    	elif type(input_list) is types.StringType:
    	    t_list.append(input_list)
    	elif type(input_list) is types.IntType:
    	    t_list.append(input_list)
    	elif type(input_list) is types.LongType:
    	    t_list.append(input_list)
    	else:
    	    t_list = input_list

    	self._openss_object = t_list
    	return
    
    def __getattr__(self,name):
    	return getattr(self._openss_object,name)

    def add(self,input_1):
    	"""
	Allows + operations.
	"""
    	if isinstance(input_1,list):
    	    self._openss_object.extend(input_1)
    	else:
    	    self._openss_object.append(input_1)

    def __iadd__(self,input_1):
    	"""
	Allows += operations.
	"""
    	if isinstance(input_1,list):
    	    self._openss_object.extend(input_1)
    	else:
    	    self._openss_object.append(input_1)
    	return self


    ###########################################
#   <metric_name> ::= <string>
#   <expMetric> ::= [<expType>::]<metric_name>
#   <expMetric_list> ::= <expMetric> [ , <expMetric> ] +

class MetricType:
    """
    A tuple that describes an experiment metric.
    """
    # 
    def __init__(self,val_1, val_2=None) :
    	"""
	Construct a single metric MetricType object.
	
	Since the first value is optional in the 
	B{OpenSpeedShop} grammar, we check to see
	how many arguments are given before assigning
	the internal elements.
	    - If only one value, that will be the metric
	    name.
	    - If two values, the first will be the experiment
	    type and the second the metric name.
	"""

    	if val_2 is None:
    	    self._openss_metric_name = val_1
    	    self._openss_exp_type = None
    	else:
    	    self._openss_metric_name = val_2
    	    self._openss_exp_type = val_1

    ###########################################

class HostList(OssList):
    """
    Object for creating a list of hosts.
    
    Hosts can be in the form of:
    	- 4 byte integer IP addresses.
	- 8 byte integer IP addresses.
	- String representing the host.
    """

    pass

class RankList(OssList):
    """
    Object for creating a list of ranks.
    
    Currently ranks are represented as integers,
    either:
    	- individually
	- a size 2 tuple which represents a range
	- a list of individuals and tuples.

    	my_rank = openss.RankList()

    	my_rank += [1,3,(22,33),564]
    """

    pass

class FileList(OssList):
    """
    Object for creating a list of file names.

    Currently file names are represented as strings.
    You  may need to use the full path.
    """

    pass

class PidList(OssList):
    """
    Object for creating a list of Process ids.

    Currently pids are represented as integers.
    """

    pass

class ThreadList(OssList):
    """
    Object for creating a list of thread ids.

    Currently thread ids are represented as integers.
    """

    pass

class MetricList(OssList):
    """
    Object for creating a list of MetricTypes.

    Use the list() command with the 'metrics' modifier
    to see which MetricTypes are available.
    
    """

    pass

class ExpTypeList(OssList):
    """
    Object for creating a list of experiment types.
    
    Experiment types or ExpTypes basically define
    what experiment data collectors are used for 
    a given experiment. These collectors are in the
    form of plugins.
    
    Use the list() command with the 'exptypes' modifier
    to see which ExpTypes are available.
    
    If you are unfamiliar with a particular ExpType it
    may be neccesary to 'run openss -cli' and then 
    use the 'help <exptype>' command, where <exptype>
    is the string of the ExpType in question.
    """

    pass

class ViewTypeList(OssList):
    """
    Object for creating a list of view types.
    
    Available ViewTypes will vary depending on
    what ExpType collector is used and what
    view plugins are present. 
    
    Use the list() command with the 'views' modifier
    to see which ViewTypes can be used for your
    experiment.
    """

    pass

class ExpId:
    """
    Object for containing an experiment id.
    
    Currently this is just an integer and may not
    need to have a unique class.
    
    The experiment id is the connection you have 
    between a particular defined experiment and
    commands. When you invoke an expCreate() or
    expAttach() command the return value is the
    experiment id representing this experiment.
    
    If you are only running one experiment the
    experiment id is not really interesting since
    the default id will be that of the currently
    focused experiment.
    
    If on the other hand, you are running multiple
    experiments, sometimes simutaniously, it is
    neccesary to use the experiment id to execute
    a command on the intended experiment.
    """
    
    def __init__(self,expid) :
    	self._openss_expid = expid
    	return
    def __getattr__(self,expid):
    	return getattr(self._openss_expid,expid)
    
class CVIdList:
    """
    Object for containing CustomView id.
    
    Currently this is just an integer and may not
    need to have a unique class.
    
    The CustomView id represents a custom view that
    was created using B{CViewCreate}().
    
    Ususally the CVId will represent a single column
    of information of an output view. It is most often
    used for comparison views.
    """
    
    def __init__(self,expid) :
    	self._openss_expid = expid
    	return
    def __getattr__(self,expid):
    	return getattr(self._openss_expid,expid)
    
class ModifierList(OssList):
    """
    Object for creating a list of modifier names.
    
    Modifiers are different for each command and
    the documentation for a specific command should
    be read to see which modifier strings are appropriate
    and what their affect on the command is. 
    """

    pass

class ParamList(OssList):
    """
    Object for creating a list of parameter types.
    """

    pass

class BreakList(OssList):
    """
    Used to build a list of break ids.
    
    The optional initializer would be either a single
    integer or a list of integers.
    """
    
    pass

class LineNoList(OssList):
    """
    Object for creating a list of line numbers.
    """

    pass

class xx(OssList):
    	pass

