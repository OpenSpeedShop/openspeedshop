# expEnable [ <expId_spec> || all ]

import openss

try :
    # Create an experiment with pcsamp being the experiment type
    my_file = openss.FileList("../../usability/phaseIII/fred")
    my_exptype = openss.ExpTypeList("pcsamp")
    exp_id = openss.expCreate(my_file,my_exptype)

    # print out the status
    exp_mod = openss.ModifierList("status")
    print "This should say ['Paused']: ", openss.list(exp_mod)
    
    openss.expDisable(exp_id)
    openss.expGo(exp_id)
    print "This should say ['Running']: ", openss.list(exp_mod)
    
    openss.waitForGo()
    print "This should say ['Terminated']: ", openss.list(exp_mod)

    openss.expEnable(exp_id)
    openss.expGo(exp_id)
    print "This should say ['Running']: ", openss.list(exp_mod)
    
    openss.waitForGo()
    print "This should say ['Terminated']: ", openss.list(exp_mod)
    try:
    	data = openss.expView()
    	if (data  == None):
    	    print "FAILED"
    	else:
    	    print "PASSED"
    except openss.error,message:
    	print "ERROR: ", message

except openss.error,message:
    print "ERROR: ", message

openss.exit()
