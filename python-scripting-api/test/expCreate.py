#<expId> = expCreate [ <target_list> ] [ <expType_list> ]

import openss

my_host = openss.HostList()
my_host += ["bimbo","bozo"]

my_rank = openss.RankList()
my_rank += [1,3,(22,33),564]

my_cluster = openss.ClusterList("cluster_1")

my_thread = openss.ThreadList(987)
my_thread += 765

my_file = openss.FileList("file_1")

my_pid = openss.PidList()
my_pid.add((1,25))

my_exptype = openss.ExpTypeList()
my_exptype += "pcsamp"
my_exptype.add("usertime")

# The number here is hard coded in a lowlevel routine
my_id = openss.expCreate(my_host,my_rank,my_cluster,my_thread,my_file,my_pid,my_exptype)
print "my_id: ", my_id

my_id = openss.expCreate(my_pid,my_exptype)
my_id = openss.expCreate(my_pid)
my_id = openss.expCreate(my_exptype)
my_id = openss.expCreate()

