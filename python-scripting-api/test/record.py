# record [ <file_spec> ]

import openss

my_file = openss.FileList("myexp.openss")

output = openss.openss_record(my_file)
output = openss.openss_record()

print output
