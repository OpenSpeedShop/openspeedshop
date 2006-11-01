# <ListOf_threadname> = listThreads [ <expId_spec> ] [ <host_list_spec> ]

import openss

my_id = openss.expCreate()
my_host = openss.HostList(["localhost"])
#my_host = openss.HostList(["host1,host2"])

my_file = openss.FileList("../../usability/phaseII/fred 900")
my_exptype = openss.ExpTypeList("pcsamp")
my_expid = openss.expCreate(my_file,my_exptype,my_host)

list_type = openss.ModifierList("threads")

output = openss.list(list_type)
print output

output = openss.list(list_type,my_id)
print output

output = openss.list(list_type,my_host)
print output

output = openss.list(list_type,my_id,my_host)
print output

