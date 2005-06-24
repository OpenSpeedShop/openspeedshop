from oss_classes import *
from oss_parse import *


#    """OpenSpeedShop scripting language."""

    ################################################################################
    #
    # expCreate
    #
    #  
    #
    ################################################################################
def expCreate(*arglist):
    	#print arglist
    	cmd_string = deconstruct("expCreate",*arglist)
	return return_int(cmd_string)

    ################################################################################
    #
    # expAttach
    #
    #  
    #
    ################################################################################
def expAttach(*arglist):
    	#print arglist
    	cmd_string = deconstruct("expAttach",*arglist)
	return_none(cmd_string)
	return

    ################################################################################
    #
    # expClose
    #
    #  
    #
    ################################################################################
def expClose(*arglist):
    	#print arglist
    	cmd_string = deconstruct("expClose",*arglist)

    ################################################################################
    #
    # expDetach
    #
    #  
    #
    ################################################################################
def expDetach(*arglist):
    	#print arglist
    	cmd_string = deconstruct("expDetach",*arglist)

    ################################################################################
    #
    # expDisable
    #
    #  
    #
    ################################################################################
def expDisable(*arglist):
    	#print arglist
    	deconstruct("expDisable",*arglist)

    ################################################################################
    #
    # expEnable
    #
    #  
    #
    ################################################################################
def expEnable(*arglist):
    	#print arglist
    	cmd_string = deconstruct("expEnable",*arglist)

    ################################################################################
    #
    # expFocus
    #
    #  
    #
    ################################################################################
def expFocus(*arglist):
    	#print arglist
    	cmd_string = deconstruct("expFocus",*arglist)

    ################################################################################
    #
    # expGo
    #
    #  
    #
    ################################################################################
def expGo(*arglist):
    	#print arglist
    	cmd_string = deconstruct("expGo",*arglist)
	return_none(cmd_string)
	return

    ################################################################################
    #
    # expPause
    #
    #  
    #
    ################################################################################
def expPause(*arglist):
    	#print arglist
    	cmd_string = deconstruct("expPause",*arglist)

    ################################################################################
    #
    # expRestore
    #
    #  
    #
    ################################################################################
def expRestore(*arglist):
    	#print arglist
    	cmd_string = deconstruct("expRestore",*arglist)

    ################################################################################
    #
    # expSave
    #
    #  
    #
    ################################################################################
def expSave(*arglist):
    	#print arglist
    	cmd_string = deconstruct("expSave",*arglist)

    ################################################################################
    #
    # expSetParam
    #
    #  
    #
    ################################################################################
def expSetParam(*arglist):
    	#print arglist
    	cmd_string = deconstruct("expSetParam",*arglist)

    ################################################################################
    #
    # expView
    #
    #  
    #
    ################################################################################
def expView(*arglist):
    	#print arglist
    	cmd_string = deconstruct("expView",*arglist)
	return_none(cmd_string)
	return

    ################################################################################
    #
    # listBreaks
    #
    #  
    #
    ################################################################################
def listBreaks(*arglist):
    	#print arglist
    	cmd_string = deconstruct("listBreaks",*arglist)

    ################################################################################
    #
    # listExp
    #
    #  
    #
    ################################################################################
def listExp(*arglist):
    	#print arglist
    	cmd_string = deconstruct("listExp",*arglist)

    ################################################################################
    #
    # listHosts
    #
    #  
    #
    ################################################################################
def listHosts(*arglist):
    	#print arglist
    	cmd_string = deconstruct("listHosts",*arglist)

    ################################################################################
    #
    # listObj
    #
    #  
    #
    ################################################################################
def listObj(*arglist):
    	#print arglist
    	cmd_string = deconstruct("listObj",*arglist)

    ################################################################################
    #
    # listPids
    #
    #  
    #
    ################################################################################
def listPids(*arglist):
    	#print arglist
    	cmd_string = deconstruct("listPids",*arglist)

    ################################################################################
    #
    # ListMetrics
    #
    #  
    #
    ################################################################################
def ListMetrics(*arglist):
    	#print arglist
    	cmd_string = deconstruct("ListMetrics",*arglist)

    ################################################################################
    #
    # listParams
    #
    #  
    #
    ################################################################################
def listParams(*arglist):
    	#print arglist
    	cmd_string = deconstruct("listParams",*arglist)

    ################################################################################
    #
    # listRanks
    #
    #  
    #
    ################################################################################
def listRanks(*arglist):
    	#print arglist
    	cmd_string = deconstruct("listRanks",*arglist)

    ################################################################################
    #
    # listStatus
    #
    #  
    #
    ################################################################################
def listStatus(*arglist):
    	#print arglist
    	cmd_string = deconstruct("listStatus",*arglist)

    ################################################################################
    #
    # listThreads
    #
    #  
    #
    ################################################################################
def listThreads(*arglist):
    	#print arglist
    	cmd_string = deconstruct("listThreads",*arglist)

    ################################################################################
    #
    # listTypes
    #
    #  
    #
    ################################################################################
def listTypes(*arglist):
    	#print arglist
    	cmd_string = deconstruct("listTypes",*arglist)

    ################################################################################
    #
    # listViews
    #
    #  
    #
    ################################################################################
def listViews(*arglist):
    	#print arglist
    	cmd_string = deconstruct("listViews",*arglist)

    ################################################################################
    #
    # xx
    #
    #  
    #
    ################################################################################
def oss_clearBreak(*arglist):
    	#print arglist
    	cmd_string = deconstruct("clearBreak",*arglist)

    ################################################################################
    #
    # xx
    #
    #  
    #
    ################################################################################
def oss_exit(*arglist):
    	#print arglist
    	cmd_string = deconstruct("exit",*arglist)

    ################################################################################
    #
    # xx
    #
    #  
    #
    ################################################################################
def oss_help(*arglist):
    	#print arglist
    	cmd_string = deconstruct("help",*arglist)

    ################################################################################
    #
    # xx
    #
    #  
    #
    ################################################################################
def oss_history(*arglist):
    	#print arglist
    	cmd_string = deconstruct("history",*arglist)

    ################################################################################
    #
    # xx
    #
    #  
    #
    ################################################################################
def oss_log(*arglist):
    	#print arglist
    	cmd_string = deconstruct("log",*arglist)

    ################################################################################
    #
    # xx
    #
    #  
    #
    ################################################################################
def oss_openGui(*arglist):
    	#print arglist
    	cmd_string = deconstruct("openGui",*arglist)

    ################################################################################
    #
    # xx
    #
    #  
    #
    ################################################################################
def oss_playBack(*arglist):
    	#print arglist
    	cmd_string = deconstruct("playBack",*arglist)

    ################################################################################
    #
    # xx
    #
    #  
    #
    ################################################################################
def oss_record(*arglist):
    	#print arglist
    	cmd_string = deconstruct("record",*arglist)

    ################################################################################
    #
    # xx
    #
    #  
    #
    ################################################################################
def oss_setBreak(*arglist):
    	#print arglist
    	cmd_string = deconstruct("setBreak",*arglist)

