# <ListOf_hostname> = listHosts [ <expId_spec> ]


import openss

list_type = openss.ModifierList("hosts")

my_file = openss.FileList("../../usability/phaseII/fred")
my_viewtype = openss.ViewTypeList()
my_viewtype += "pcsamp"
my_id = openss.expCreate(my_file,my_viewtype)

output = openss.list(my_id,list_type)
print output
output = openss.list(list_type)
print output
openss.expGo()
output = openss.list(list_type)
print output
openss.exit()
