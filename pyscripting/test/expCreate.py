#<expId> = expCreate [ <target_list> ] [ <expType_list> ]

import openss

try :

    my_file = openss.FileList("../../usability/phaseIII/fred 90")
    my_viewtype = openss.ViewTypeList("pcsamp")
    openss.expCreate(my_file,my_viewtype,9)
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
#my_host += ["bimbo","bozo"]
#
#my_rank = openss.RankList()
#my_rank += [1,3,(22,33),564]
#
#my_cluster = openss.ClusterList("cluster_1")
#
#my_thread = openss.ThreadList(987)
#my_thread += 765
#
#my_file = openss.FileList("file_1")
#
#my_pid = openss.PidList()
#my_pid.add((1,25))
#
#my_exptype = openss.ExpTypeList()
#my_exptype += "pcsamp"
#my_exptype.add("usertime")
#
### The number here is hard coded in a lowlevel routine
#my_id = openss.expCreate(my_host,my_rank,my_cluster,my_thread,my_file,my_pid,my_exptype)
#print "my_id: ", my_id
#
#my_id = openss.expCreate(my_pid,my_exptype)
#my_id = openss.expCreate(my_pid)
#my_id = openss.expCreate(my_exptype)
#my_id = openss.expCreate()

