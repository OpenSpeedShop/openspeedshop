# playBack <file_spec>

import openss

my_file = openss.FileList("myexp.openss")

output = openss.openss_playBack(my_file)
#output = openss.openss_playBack()

print output
