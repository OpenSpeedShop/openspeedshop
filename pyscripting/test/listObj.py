# <ListOf_filename> = listObj [ <expId_spec> ] [ <target> ]

import openss

list_type = openss.ModifierList("obj")

my_viewtype = openss.ViewTypeList("pcsamp")
my_file = openss.FileList("../../usability/phaseII/fred")

my_id	= openss.expCreate(my_file,my_viewtype)

ret = openss.list(list_type)

print " "
print ret
print " "

r_count = len(ret)
for row_ndx in range(r_count):
   print ret[row_ndx]

openss.exit()
