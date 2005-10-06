# expSave [ <expId_spec> ] [ copy ] <file_spec>

import oss

my_expid = oss.ExpId(7)

my_modifer = oss.ModifierList("copy")

my_file = oss.FileList("file_1")

oss.expSave(my_expid,my_modifer,my_file)

oss.expSave(my_modifer,my_file)

oss.expSave(my_expid,my_file)

oss.expSave(my_file)

