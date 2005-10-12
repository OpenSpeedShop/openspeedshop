# expSave [ <expId_spec> ] [ copy ] <file_spec>

import openss

my_expid = openss.ExpId(7)

my_modifer = openss.ModifierList("copy")

my_file = openss.FileList("file_1")

openss.expSave(my_expid,my_modifer,my_file)

openss.expSave(my_modifer,my_file)

openss.expSave(my_expid,my_file)

openss.expSave(my_file)

