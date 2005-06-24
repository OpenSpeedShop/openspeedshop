
class MyList:
    	"base class for all lists"
    	# 
    	def __init__(self,input_list=[]) :
    	    self._oss_object = input_list
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

class ExpTypeList(MyList):
    	pass

class ExpId:
    	def __init__(self,expid) :
    	    self._oss_expid = [expid]
	    return
    	def __getattr__(self,expid):
    	    return getattr(self._oss_expid,expid)
    

class ParmList(MyList):
    	pass

class xx(MyList):
    	pass

