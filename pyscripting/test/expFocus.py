# <expId> = expFocus [ <expId_spec> ]

import openss

my_file = openss.FileList("../../usability/phaseII/fred")

# Define and run pcsamp experiment
pcsamp_viewtype = openss.ViewTypeList()
pcsamp_viewtype += "pcsamp"
pcsamp_expid = openss.expCreate(my_file,pcsamp_viewtype)
openss.expGo()
openss.wait()
print pcsamp_expid

#define and run usertime experiment
user_viewtype = openss.ViewTypeList()
user_viewtype += "usertime"
user_expid = openss.expCreate(my_file,user_viewtype)
openss.expGo()
openss.wait()
print user_expid

expid = openss.expFocus()
print expid
openss.dumpView()

expid = openss.expFocus(pcsamp_expid)
print expid
openss.dumpView()

expid = openss.expFocus()
print expid

openss.exit()
