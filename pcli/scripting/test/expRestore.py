# <expId> = expRestore  <file_spec>

import oss

my_file = oss.FileList("file_1")

exp1 = oss.expRestore(my_file)

print exp1
