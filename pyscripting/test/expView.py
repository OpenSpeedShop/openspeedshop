# expView [ <expId_spec> ] [ <viewType> ] [ -m <expMetric_list> ] [ <target_list> ]
import openss

my_file = openss.FileList("../../usability/phaseIII/fred")
my_exptype = openss.ExpTypeList("pcsamp")
my_expid = openss.expCreate(my_file,my_exptype)

my_viewtype = openss.ViewTypeList("pcsamp")
my_metric_list = openss.MetricList(["pcsamp","time"])


openss.expGo()
openss.wait_for_go()

#openss.expView(my_expid)

#openss.expView(my_expid,my_viewtype)

#ret = openss.expView(my_expid,my_viewtype,my_metric_list)

# Dump the raw return data.
#print ret
openss.dump_view()

# Go through each list (row) and print out
# the data (columns).
#r_count = len(ret)
#for row_ndx in range(r_count):
#    print " "
#    row =ret[row_ndx]
#    c_count = len(row)
#    for rel_ndx in range(c_count):
#        print row[rel_ndx]

