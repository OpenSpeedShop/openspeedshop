# playBack <file_spec>

import openss

my_file = openss.FileList("myexp.openss")

output = openss.playBack(my_file)
#output = openss.playBack()

print output
