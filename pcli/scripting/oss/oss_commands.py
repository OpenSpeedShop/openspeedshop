from oss_classes import *
from oss_parse import *


#    """OpenSpeedShop scripting language."""

    ################################################################################
    #
    # expCreate
    #
    ################################################################################
def expCreate(*arglist):
    	cmd_string = deconstruct("expCreate",*arglist)
	return return_int(cmd_string)

    ################################################################################
    #
    # expAttach
    #
    ################################################################################
def expAttach(*arglist):
    	cmd_string = deconstruct("expAttach",*arglist)
	return return_none(cmd_string)

    ################################################################################
    #
    # expClose
    #
    ################################################################################
def expClose(*arglist):
    	cmd_string = deconstruct("expClose",*arglist)
	return return_none(cmd_string)

    ################################################################################
    #
    # expDetach
    #
    ################################################################################
def expDetach(*arglist):
    	cmd_string = deconstruct("expDetach",*arglist)
	return return_none(cmd_string)

    ################################################################################
    #
    # expDisable
    #
    ################################################################################
def expDisable(*arglist):
    	cmd_string = deconstruct("expDisable",*arglist)
	return return_none(cmd_string)

    ################################################################################
    #
    # expEnable
    #
    ################################################################################
def expEnable(*arglist):
    	cmd_string = deconstruct("expEnable",*arglist)
	return return_none(cmd_string)

    ################################################################################
    #
    # expFocus
    #
    ################################################################################
def expFocus(*arglist):
    	cmd_string = deconstruct("expFocus",*arglist)
	return return_int(cmd_string)

    ################################################################################
    #
    # expGo
    #
    ################################################################################
def expGo(*arglist):
    	cmd_string = deconstruct("expGo",*arglist)
	return return_none(cmd_string)

    ################################################################################
    #
    # expPause
    #
    ################################################################################
def expPause(*arglist):
    	cmd_string = deconstruct("expPause",*arglist)
	return return_none(cmd_string)

    ################################################################################
    #
    # expRestore
    #
    ################################################################################
def expRestore(*arglist):
    	cmd_string = deconstruct("expRestore",*arglist)
	return return_int(cmd_string)

    ################################################################################
    #
    # expSave
    #
    ################################################################################
def expSave(*arglist):
    	cmd_string = deconstruct("expSave",*arglist)
	return return_none(cmd_string)

    ################################################################################
    #
    # expSetParam
    #
    ################################################################################
def expSetParam(*arglist):
    	cmd_string = deconstruct("expSetParam",*arglist)
	return return_none(cmd_string)

    ################################################################################
    #
    # expStatus
    #
    ################################################################################
def expStatus(*arglist):
    	cmd_string = deconstruct("expStatus",*arglist)
	return return_none(cmd_string)

    ################################################################################
    #
    # expView
    #
    ################################################################################
def expView(*arglist):
    	cmd_string = deconstruct("expView",*arglist)
	return return_none(cmd_string)

    ################################################################################
    #
    # listBreaks
    #
    ################################################################################
def listBreaks(*arglist):
    	cmd_string = deconstruct("listBreaks",*arglist)
	return return_int_list(cmd_string)

    ################################################################################
    #
    # listExp
    #
    ################################################################################
def listExp(*arglist):
    	cmd_string = deconstruct("listExp",*arglist)
	return return_int_list(cmd_string)

    ################################################################################
    #
    # listHosts
    #
    ################################################################################
def listHosts(*arglist):
    	cmd_string = deconstruct("listHosts",*arglist)
	return return_string_list(cmd_string)

    ################################################################################
    #
    # listMetrics
    #
    ################################################################################
def listMetrics(*arglist):
    	cmd_string = deconstruct("listMetrics",*arglist)
	return return_string_list(cmd_string)

    ################################################################################
    #
    # listObj
    #
    ################################################################################
def listObj(*arglist):
    	cmd_string = deconstruct("listObj",*arglist)
	return return_string_list(cmd_string)

    ################################################################################
    #
    # listParams
    #
    ################################################################################
def listParams(*arglist):
    	cmd_string = deconstruct("listParams",*arglist)
	return return_string_list(cmd_string)

    ################################################################################
    #
    # listPids
    #
    ################################################################################
def listPids(*arglist):
    	cmd_string = deconstruct("listPids",*arglist)
	return return_int_list(cmd_string)

    ################################################################################
    #
    # listRanks
    #
    ################################################################################
def listRanks(*arglist):
    	cmd_string = deconstruct("listRanks",*arglist)
	return return_int_list(cmd_string)

    ################################################################################
    #
    # listSrc
    #
    ################################################################################
def listSrc(*arglist):
    	cmd_string = deconstruct("listSrc",*arglist)
	return return_string(cmd_string)

    ################################################################################
    #
    # listStatus
    #
    ################################################################################
def listStatus(*arglist):
    	cmd_string = deconstruct("listStatus",*arglist)
	return return_string_list(cmd_string)

    ################################################################################
    #
    # listThreads
    #
    ################################################################################
def listThreads(*arglist):
    	cmd_string = deconstruct("listThreads",*arglist)
	return return_int_list(cmd_string)

    ################################################################################
    #
    # listTypes
    #
    ################################################################################
def listTypes(*arglist):
    	cmd_string = deconstruct("listTypes",*arglist)
	return return_string_list(cmd_string)

    ################################################################################
    #
    # listViews
    #
    ################################################################################
def listViews(*arglist):
    	cmd_string = deconstruct("listTypes",*arglist)
	return return_string_list(cmd_string)

    ################################################################################
    #
    # clearBreak
    #
    #  
    #
    ################################################################################
def oss_clearBreak(*arglist):
    	#print arglist
    	cmd_string = deconstruct("clearBreak",*arglist)

    ################################################################################
    #
    # exit
    #
    #  
    #
    ################################################################################
def oss_exit(*arglist):
    	#print arglist
    	cmd_string = deconstruct("exit",*arglist)

    ################################################################################
    #
    # help
    #
    #  
    #
    ################################################################################
def oss_help(*arglist):
    	#print arglist
    	cmd_string = deconstruct("help",*arglist)

    ################################################################################
    #
    # history
    #
    #  
    #
    ################################################################################
def oss_history(*arglist):
    	#print arglist
    	cmd_string = deconstruct("history",*arglist)

    ################################################################################
    #
    # log
    #
    #  
    #
    ################################################################################
def oss_log(*arglist):
    	#print arglist
    	cmd_string = deconstruct("log",*arglist)

    ################################################################################
    #
    # openGui
    #
    #  
    #
    ################################################################################
def oss_openGui(*arglist):
    	#print arglist
    	cmd_string = deconstruct("openGui",*arglist)

    ################################################################################
    #
    # playBack
    #
    #  
    #
    ################################################################################
def oss_playBack(*arglist):
    	#print arglist
    	cmd_string = deconstruct("playBack",*arglist)

    ################################################################################
    #
    # record
    #
    #  
    #
    ################################################################################
def oss_record(*arglist):
    	#print arglist
    	cmd_string = deconstruct("record",*arglist)

    ################################################################################
    #
    # setBreak
    #
    #  
    #
    ################################################################################
def oss_setBreak(*arglist):
    	#print arglist
    	cmd_string = deconstruct("setBreak",*arglist)

