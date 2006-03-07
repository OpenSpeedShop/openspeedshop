# This shows a number of ways to build an argument
# 
# You will immediately note that none of this is within
# the OSS name space. I haven't figured out how to do
# this using multiple files yet. Any suggestions would
# be appreciated. I do not like monolithic programming,
# but will do it if forced.
#
# I have my class definitions in one file, my parsing 
# in another and the command definitions in a third.
#
# The translated commandline is printed out for each
# command invocation. It is what will be sent to the
# OpenSpeedShop parser.
#
# The real semantic digesting of the commands is done
# in OpenSpeedShop proper. So, you can shove a lot of
# non-sense through here, but it will get caught down
# the line.

#from openss_classes import *
#from openss_parse import *
#from openss_commands import *

import openss

my_host = openss.HostList()
my_host += ["bosco","111.222.333.444"]
my_host += ["bimbo","bozo"]

my_rank = openss.RankList([2])
my_rank.add( [8])
my_rank += [6]
my_rank += [1,3,(22,33),564]
openss.RankList(my_rank).add(44)

my_rank2 = openss.RankList()
my_rank2.add(1234)
my_rank2.add(34)
my_rank2 += 888

# The number here is hard coded in a lowlevel routine
my_id = openss.expCreate(my_host,my_rank2,my_rank)
print "my_id: ", my_id

# I need to identify the argument as an experiment
# id so you need to get it into an ExpId object.
exp1 = openss.ExpId(my_id)
openss.expGo(exp1)

openss.expView(exp1,"vtop88")

my_cluster = openss.ClusterList("cluster_1")
my_cluster.add(["cluster_2"])
my_cluster += ["cluster_3","cluster_4"]

my_id = openss.expCreate(my_cluster)

my_thread = openss.ThreadList(987)
my_thread += 765
my_thread.add((22,33))

my_id = openss.expCreate(my_thread)

my_file = openss.FileList("file_1")
my_file.add(["file_2"])
my_file += ["file_3","file_4"]

my_id = openss.expCreate(my_file)

my_pid = openss.PidList(387)
my_pid += 2600
my_pid.add((1,25))

my_id = openss.expCreate(my_pid)

my_line = openss.LineList(22222)
my_line += 222999
my_line.add((333,444))

my_id = openss.expCreate(my_line)

my_exptype = openss.ExpTypeList()
my_exptype += "pcsamp"
my_exptype.add("usertime")

my_id = openss.expCreate(my_file, my_exptype)

my_viewtype = openss.ViewTypeList()
my_viewtype += "butterfly"
my_viewtype.add("inclusive")

my_file_bosco = openss.FileList("bosco")

my_id = openss.expView(my_file_bosco, my_viewtype)

metric_1 = openss.MetricType("mtype_1")
metric_2 = openss.MetricType("etype_2", "mtype_2")
