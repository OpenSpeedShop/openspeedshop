# record [ <file_spec> ]

import openss

my_file = openss.FileList("myexp.openss")

output = openss.record(my_file)
output = openss.record()

print output
