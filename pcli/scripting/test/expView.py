# expView [ <expId_spec> ] [ <viewType> ] [ -m <expMetric_list> ] [ <target_list> ]
import oss

my_host = oss.HostList()
my_host += ["bosco","111.222.333.444"]

my_rank = oss.RankList()
my_rank += [1,3,(22,33),564]

my_file_bosco = oss.FileList("bosco")

my_viewtype = oss.ViewTypeList()
my_viewtype += "pcsamp"

my_modifer = oss.ModifierList("all")

my_expid = oss.ExpId(7)

oss.expView(my_expid,my_modifer,my_rank,my_file_bosco,my_viewtype)
