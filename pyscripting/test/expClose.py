# expClose [ <expId_spec> || all ] [ kill ]

import openss

try :

    my_file = openss.FileList("../../usability/phaseII/fred")
    my_viewtype = openss.ViewTypeList("pcsamp")
    openss.expCreate(my_file,my_viewtype)
    
    exp_mod = openss.ModifierList("exp")
    print "Current focused expId: ", openss.list(exp_mod)
    openss.expGo()
    openss.waitForGo()

    print "Current focused expId: ",openss.list(exp_mod)

    openss.expClose()

    print "Current focused expId: ",openss.list(exp_mod)



except openss.error,message:
    print "ERROR: ", message

openss.exit()

#exp1 = openss.ExpId(7)
#my_modifer = openss.ModifierList("all")
#openss.expClose(exp1,my_modifer)
#my_modifer = openss.ModifierList("kill")
#openss.expClose(my_modifer)
#openss.expClose(exp1)
#openss.expClose()

