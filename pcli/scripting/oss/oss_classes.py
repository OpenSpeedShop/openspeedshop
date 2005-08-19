import types

class MyList:
    	"base class for all lists"
    	# 
    	def __init__(self,input_list=None) :
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

    	    self._oss_object = t_list
	    return
    
    	def __getattr__(self,name):
    	    return getattr(self._oss_object,name)

    	def add(self,input_1):
    	    if isinstance(input_1,list):
   	    	self._oss_object.extend(input_1)
    	    else:
   	    	self._oss_object.append(input_1)

    	def __iadd__(self,input_1):
    	    if isinstance(input_1,list):
   	    	self._oss_object.extend(input_1)
    	    else:
   	    	self._oss_object.append(input_1)
    	    return self

    	# 


    ###########################################
#   <metric_name> ::= <string>
#   <expMetric> ::= [<expType>::]<metric_name>
#   <expMetric_list> ::= <expMetric> [ , <expMetric> ] +

class MetricType:
    	# 
    	def __init__(self,val_1, val_2=None) :

    	    if val_2 is None:
	    	self._oss_metric_name = val_1
		self._oss_exp_type = None
	    else:
    	    	self._oss_metric_name = val_2
    	    	self._oss_exp_type = val_1

    ###########################################

class HostList(MyList):
    	pass

class RankList(MyList):
    	pass

class FileList(MyList):
    	pass

class PidList(MyList):
    	pass

class ThreadList(MyList):
    	pass

class ClusterList(MyList):
    	pass

class MetricList(MyList):
    	pass

class ExpTypeList(MyList):
    	pass

class ViewTypeList(MyList):
    	pass

class ExpId:
    	def __init__(self,expid) :
    	    self._oss_expid = [expid]
	    return
    	def __getattr__(self,expid):
    	    return getattr(self._oss_expid,expid)
    
class ModifierList(MyList):
    	pass

class ParamList(MyList):
    	pass

class BreakList(MyList):
    	pass

class LineNoList(MyList):
    	pass

class xx(MyList):
    	pass

