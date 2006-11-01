import openss

try :

    openss.expCreate()
    my_file = openss.FileList("../../usability/phaseII/fred")
    my_viewtype = openss.ViewTypeList("pcsamp")
    openss.expAttach(my_file,my_viewtype)
    openss.expGo()
    openss.waitForGo()
    #openss.dumpView()
    output = openss.expView()
    print output
    openss.exit()



except openss.error,message:
    print "ERROR: ", message
    openss.exit()

#my_host = openss.HostList()
#my_host += ["bosco","111.222.333.444"]
#my_host += ["bimbo","bozo"]

#my_rank = openss.RankList()
#my_rank += [1,3,(22,33),564]

#my_file_bosco = openss.FileList("bosco")

#my_exptype = openss.ExpTypeList()
#my_exptype += "pcsamp"
#my_exptype.add("usertime")

#my_modifer = openss.ModifierList("all")

#exp1 = openss.ExpId(7)

#openss.expAttach(exp1,my_modifer,my_rank,my_file_bosco,my_exptype)
