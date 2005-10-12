# <ListOf_filename> = listObj [ <expId_spec> ] [ <target> ]

import openss


my_file = openss.FileList("file_1")

# The number here is hard coded in a lowlevel routine
my_id = openss.ExpId(openss.expCreate())

output = openss.listObj(my_id,my_file)
output = openss.listObj(my_file)
output = openss.listObj(my_id)
output = openss.listObj()

print output
