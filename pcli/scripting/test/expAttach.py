import oss

my_host = oss.HostList()
my_host += ["bosco","111.222.333.444"]
my_host += ["bimbo","bozo"]

my_rank = oss.RankList()
my_rank += [1,3,(22,33),564]

my_file_bosco = oss.FileList("bosco")

my_exptype = oss.ExpTypeList()
my_exptype += "pcsamp"
my_exptype.add("usertime")

my_modifer = oss.ModifierList("all")

exp1 = oss.ExpId(7)

oss.expAttach(exp1,my_modifer,my_rank,my_file_bosco,my_exptype)
