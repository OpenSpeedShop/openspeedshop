# <ListOf_hostname> = listHosts [ <expId_spec> ]


import openss

my_file = openss.FileList("../../usability/phaseIII/fred")
my_viewtype = openss.ViewTypeList()
my_viewtype += "pcsamp"
my_id = openss.expCreate(my_file,my_viewtype)

my_modifer = openss.ModifierList("hosts")

output = openss.list(my_id,my_modifer)
print output
