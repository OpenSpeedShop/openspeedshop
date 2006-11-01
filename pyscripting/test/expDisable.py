# expDisable [ <expId_spec> || all ]

import openss

try :
    # Create an experiment with pcsamp being the experiment type
    my_file = openss.FileList("../../usability/phaseII/fred 700")
    my_exptype = openss.ExpTypeList("pcsamp")
    exp_id = openss.expCreate(my_file,my_exptype)

    # print out the status
    exp_mod = openss.ModifierList("status")
    print "This should say []: ", openss.list(exp_mod)
    
    openss.expDisable(exp_id)
    openss.expGo(exp_id)
    print "This should say []: ", openss.list(exp_mod)
    
    openss.waitForGo()
    print "This should say []: ", openss.list(exp_mod)

    try:
    	data = openss.expView()
    	if (data  == None):
    	    print "PASSED"
	    openss.wait() # kluge for late exception return
    	else:
    	    print "FAILED"
    except openss.error,message:
    	print "Expected: ", message

except message:
    print "ERROR: ", message

openss.exit()


#exp1 = openss.ExpId(7)
#
#my_modifer = openss.ModifierList("all")
#
#openss.expDisable(exp1,my_modifer)
#
#openss.expDisable(my_modifer)
#
#openss.expDisable(exp1)
#
#openss.expDisable()

