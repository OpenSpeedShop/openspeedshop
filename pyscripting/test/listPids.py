# <ListOf_pidname> = listPids [ <expId_spec> ] [ <host_list_spec> ]

import openss

list_type = openss.ModifierList("pids")

my_file = openss.FileList("../../usability/phaseII/fred 800 ")
my_type = "pcsamp"
my_host = openss.HostList("localhost")
my_id = openss.expCreate(my_file,my_type,my_host)

openss.expGo()
openss.wait()

output = openss.list(list_type)
print output

output = openss.list(list_type,my_id)
print output

#output = openss.list(list_type,my_host)
#print output

openss.exit()
