# <listOf_filename> = listSrc [ <expId_spec> ] [ <target> ] 

import openss

list_type = openss.ModifierList("src")

my_viewtype = openss.ViewTypeList("pcsamp")
my_file = openss.FileList("../../usability/phaseII/fred")

my_id	= openss.expCreate(my_file,my_viewtype)
my_host = openss.HostList(["localhost"])

ret = openss.list(list_type)

#output = openss.listSrc(my_id)
#output = openss.listSrc(my_host)
#output = openss.listSrc(my_id,my_host)

print " "
print ret
print " "

r_count = len(ret)
for row_ndx in range(r_count):
   print ret[row_ndx]

openss.exit()
