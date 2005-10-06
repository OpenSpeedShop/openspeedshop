# <ListOf_filename> = listObj [ <expId_spec> ] [ <target> ]

import oss


my_file = oss.FileList("file_1")

# The number here is hard coded in a lowlevel routine
my_id = oss.ExpId(oss.expCreate())

output = oss.listObj(my_id,my_file)
output = oss.listObj(my_file)
output = oss.listObj(my_id)
output = oss.listObj()

print output
