# expSave [ <expId_spec> ] [ copy ] <file_spec>

import openss
import os

my_file = openss.FileList("../../usability/phaseII/fred 900")
my_viewtype = openss.ViewTypeList("pcsamp")
my_expid = openss.expCreate(my_file,my_viewtype)

openss.expGo()
openss.wait()

my_modifer = openss.ModifierList("copy")

my_file = openss.FileList("file_1.dat")
os.system("rm -f file_1.dat")
os.system("ls file_1.dat")

openss.expSave(my_expid,my_modifer,my_file)
os.system("ls file_1.dat")

my_file = openss.FileList("file_2.dat")
os.system("rm -f file_2.dat")
openss.expSave(my_modifer,my_file)
os.system("ls file_2.dat")

my_file = openss.FileList("file_3.dat")
os.system("rm -f file_3.dat")
openss.expSave(my_expid,my_file)
os.system("ls file_3.dat")

my_file = openss.FileList("file_4.dat")
os.system("rm -f file_4.dat")
openss.expSave(my_file)
os.system("ls file_4.dat")

openss.exit()
