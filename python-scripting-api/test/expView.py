# expView [ <expId_spec> ] [ <viewType> ] [ -m <expMetric_list> ] [ <target_list> ]
import openss

my_host = openss.HostList()
my_host += ["bosco","111.222.333.444"]

my_rank = openss.RankList()
my_rank += [1,3,(22,33),564]

my_file_bosco = openss.FileList("bosco")

my_viewtype = openss.ViewTypeList()
my_viewtype += "pcsamp"

#my_modifer = openss.ModifierList("all")

my_metric_list = openss.MetricList()
my_metric_list += "exclusive"
my_metric_list += ("pcsamp","inclusive")

my_expid = openss.ExpId(7)

openss.expView(my_expid,my_viewtype,my_metric_list,my_rank,my_file_bosco)

openss.expView(my_expid)

openss.expView(my_expid,my_viewtype)

ret = openss.expView(my_expid,my_viewtype,my_metric_list)

#This is dummied up data packaged in  the C++ module 
# and passed back by as a python object.
print ret

r_count = len(ret)
for row_ndx in range(r_count):
    print " "
    row =ret[row_ndx]
    c_count = len(row)
    for rel_ndx in range(c_count):
        print row[rel_ndx]

