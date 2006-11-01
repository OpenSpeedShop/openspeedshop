# expEnable [ <expId_spec> || all ]

import openss

# First time disable the data collection.
try :
    # Create an experiment with pcsamp being the experiment type
    my_file = openss.FileList("../../usability/phaseII/fred 900")
    my_exptype = openss.ExpTypeList("pcsamp")
    exp_id = openss.expCreate(my_file,my_exptype)

    # print out the status
    exp_mod = openss.ModifierList("status")
    print "This should say ['Paused']: ", openss.list(exp_mod)
    
    openss.expDisable(exp_id)
    openss.expGo(exp_id)
    print "This should say ['Running']: ", openss.list(exp_mod)
    
    openss.wait()
    print "This should say ['Terminated']: ", openss.list(exp_mod)

#    openss.expEnable(exp_id)
#    openss.expGo(exp_id)
#    print "This should say ['Running']: ", openss.list(exp_mod)
#    
#    openss.wait()
#    print "This should say ['Terminated']: ", openss.list(exp_mod)

    try:
    	data = openss.expView()
    	if (data  == None):
    	    print "PASSED"
	    openss.wait() # kluge for late exception return
    	else:
    	    print "FAILED"
    except openss.error,message:
    	print  message

except openss.error,message:
    print "ERROR: ", message

# Second time reenable the data collection.
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
    openss.expEnable(exp_id)
    print "This should say ['Running']: ", openss.list(exp_mod)
    
    openss.wait()
    print "This should say ['Terminated']: ", openss.list(exp_mod)

#    openss.expEnable(exp_id)
#    openss.expGo(exp_id)
#    print "This should say ['Running']: ", openss.list(exp_mod)
#    
#    openss.wait()
#    print "This should say ['Terminated']: ", openss.list(exp_mod)

    try:
    	data = openss.expView()
    	if (data  == None):
    	    print "FAILED"
	    openss.list(exp_mod) # kluge for late exception return
    	else:
    	    print "PASSED"
    except openss.error,message:
    	print  message

except openss.error,message:
    print "ERROR: ", message

openss.exit()
