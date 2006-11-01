# expPause [ <expId_spec> || all ]

import openss
import os

my_file = openss.FileList("../../usability/phaseII/fred 900")
my_viewtype = openss.ViewTypeList("pcsamp")
my_expid = openss.expCreate(my_file,my_viewtype)

openss.expGo()
openss.expPause()
os.system("sleep 20")
print "Not yet"

openss.expGo()
openss.expPause()
os.system("sleep 20")
print "Not yet"

openss.expGo()
openss.expPause()
os.system("sleep 20")
print "Not yet"

openss.expGo()
os.system("sleep 20")
print "Should be done now"

#my_modifer = openss.ModifierList("all")

#openss.expPause(exp1,my_modifer)

#openss.expPause(my_modifer)

#openss.expPause(exp1)

#openss.expPause()
