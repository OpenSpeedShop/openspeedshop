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
    """

    pass

class PidList(OssList):
    """
    Object for creating a list of Process ids.
    """

    pass

class ThreadList(OssList):
    """
    Object for creating a list of thread ids.
    """

    pass

class ClusterList(OssList):
    """
    Object for creating a list of cluster names.
    """

    pass

class MetricList(OssList):
    """
    Object for creating a list of MetricTypes.
    """

    pass

class ExpTypeList(OssList):
    """
    Object for creating a list of experiment types.
    """

    pass

class ViewTypeList(OssList):
    """
    Object for creating a list of view types.
    """

    pass

class ExpId:
    """
    Object for containing an experiment id.
    
    Currently this is just an integer and may not
    need to have a unique class.
    """
    
    def __init__(self,expid) :
    	self._openss_expid = [expid]
    	return
    def __getattr__(self,expid):
    	return getattr(self._openss_expid,expid)
    
class ModifierList(OssList):
    """
    Object for creating a list of modifier names.
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

