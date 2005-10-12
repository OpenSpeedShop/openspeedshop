# log [ <file_spec> ]

import openss

my_file = openss.FileList("myexp.log")

output = openss.openss_log(my_file)
output = openss.openss_log()

print output
