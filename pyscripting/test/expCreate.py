#<expId> = expCreate [ <target_list> ] [ <expType_list> ]

import openss

try :

    my_file = openss.FileList("../../usability/phaseII/fred 2800")
    my_viewtype = openss.ViewTypeList("pcsamp")
    openss.expCreate(my_file,my_viewtype)
    openss.expGo()
    openss.wait()
    openss.dumpView()
    #output = openss.expView()
    #print output
    openss.exit()

except openss.error,message:
    print "ERROR: ", message
    openss.exit()
