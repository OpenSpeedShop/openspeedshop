# log [ <file_spec> ]

import oss

my_file = oss.FileList("myexp.log")

output = oss.oss_log(my_file)
output = oss.oss_log()

print output
