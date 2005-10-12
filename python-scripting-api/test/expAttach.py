import openss

my_host = openss.HostList()
my_host += ["bosco","111.222.333.444"]
my_host += ["bimbo","bozo"]

my_rank = openss.RankList()
my_rank += [1,3,(22,33),564]

my_file_bosco = openss.FileList("bosco")

my_exptype = openss.ExpTypeList()
my_exptype += "pcsamp"
my_exptype.add("usertime")

my_modifer = openss.ModifierList("all")

exp1 = openss.ExpId(7)

openss.expAttach(exp1,my_modifer,my_rank,my_file_bosco,my_exptype)
