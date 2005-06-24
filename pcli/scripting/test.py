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

#from oss_classes import *
#from oss_parse import *
#from oss_commands import *

import oss

my_host = oss.HostList()
my_host += ["bosco","111.222.333.444"]
my_host += ["bimbo","bozo"]

my_rank = oss.RankList([2])
my_rank.add( [8])
my_rank += [6]
my_rank += [1,3,(22,33),564]
oss.RankList(my_rank).add(44)

# Using the default value for a list fails
# here because it just grabs the value set
# by my_host for some reason. Same behavior
# on IRIX and Linux. I'm going to change the
# constructor to take a variable number of
# arguments and check for len value of 0.
my_rank2 = oss.RankList([])
my_rank2.add(1234)
my_rank2.add(34)
my_rank2 += 888

# The number here is hard coded in a lowlevel routine
my_id = oss.expCreate(my_host,my_rank2,my_rank);
print my_id

# I need to identify the argument as an experiment
# id so you need to get it into an ExpId object.
exp1 = oss.ExpId(my_id)
oss.expGo(exp1)

oss.expView(exp1,"vtop88")


