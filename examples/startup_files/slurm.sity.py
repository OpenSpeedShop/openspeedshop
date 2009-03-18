print ""
print "======================================================="
print "=                                                     ="
print "=  WELCOME TO OPEN|SPEEDSHOP 1.9.2 devel (jegs vers)  ="
print "=                                                     ="
print "======================================================="

#mrnetcfg = os.environ['HOME'] + "/.openspeedshop"
mrnetcfg = os.environ['HOME'] + os.sep + '.openspeedshop'


if os.path.isdir(mrnetcfg):
   mrnetcfg = mrnetcfg + '/.openss-mrnet-topology'
else:
   #Make Directory $HOME/.openspeedshop
   os.mkdir(mrnetcfg, 0755)
   mrnetcfg = mrnetcfg + '/.openss-mrnet-topology'


if (os.environ.has_key("SLURM_JOBID")):
    os.system("mrnet_slurm")
else:
    cfg=open(mrnetcfg,'w')
    cfg.write("localhost:0 => localhost:1;")
    cfg.close()
    
print ""
print "An MRNet Configuration has been created for you - please"
print "check if this is consistent with your partition."
print ""
print "Current MRNet configuration in " + mrnetcfg
print ""
os.system("cat "+mrnetcfg)
print ""



