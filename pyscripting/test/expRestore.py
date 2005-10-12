# <expId> = expRestore  <file_spec>

import openss

my_file = openss.FileList("file_1")

exp1 = openss.expRestore(my_file)

print exp1
