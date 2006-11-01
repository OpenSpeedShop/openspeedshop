# expDetach [ <expId_spec> ] [ <target_list> ] [ <expType_list> ]

import openss

try :
    # Create an experiment with pcsamp being the experiment type
    my_file = openss.FileList("../../usability/phaseII/fred 750")
    my_exptype = openss.ExpTypeList("pcsamp")
    exp_id = openss.expCreate(my_file,my_exptype)

    # print out the type
    exp_mod = openss.ModifierList("types")
    print "This should say [pcsamp]: ", openss.list(exp_mod)
    
    openss.expDetach(exp_id,my_exptype)
    print "This should say []: ", openss.list(exp_mod)
    
    my_exptype = openss.ExpTypeList("usertime")
    openss.expAttach(exp_id,my_exptype)
    print "This should say [usertime]: ", openss.list(exp_mod)
    
    openss.expGo(exp_id)
    openss.wait()

    print openss.expView()
    openss.wait() # kluge for delayed exception
    
except openss.error,message:
    print "ERROR: ", message

openss.exit()

#my_id = openss.ExpId(openss.expCreate())
#
#my_host = openss.HostList()
#my_host += ["bimbo","bozo"]
#
#my_rank = openss.RankList()
#my_rank += [1,3,(22,33),564]

#my_cluster = openss.ClusterList("cluster_1")

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
# The number here is hard coded in a lowlevel routine
#openss.expDetach(my_id,my_host,my_rank,my_cluster,my_thread,my_file,my_pid,my_exptype)
#
#openss.expDetach(my_id,my_pid,my_exptype)
#openss.expDetach(my_id,my_pid)
#openss.expDetach(my_id,my_exptype)
#openss.expDetach(my_id)
#openss.expDetach(my_pid,my_exptype)
#openss.expDetach(my_pid)
#openss.expDetach(my_exptype)
#openss.expDetach()
