import code
import os
import os.path
import re
import string
import sys
import types
import tempfile
import commands

import PY_Input

def LDPCL(template="*"):
    StartDPCLwithLibs(template,os.environ['OPENSS_PREFIX'],os.environ['OPENSS_PREFIX'])

def DPCL():
    print "Starting DPCL daemon on local node"
    LDPCL("*")

def SDPCL():
#    print "CPUs is",os.environ['SLURM_CPUS_ON_NODE']
    if (os.environ.has_key("SLURM_JOBID")):
        LDPCL("srun -c 8 -l *")
    elif (os.environ.has_key("PBS_JOBID")):
        LDPCL(cmd)
    else:
        print "No Job ID defined"

def PDPCL(sid):
    print "Starting DPCL daemons for SLURM job ",sid
    LDPCL("srun -c 8 --jobid "+str(sid)+" *")


if (os.environ.has_key("SLURM_JOBID")):
   SDPCL()
elif (os.environ.has_key("PBS_JOBID")):
   cmd= "/sbin/ifconfig"
   pat=commands.getoutput(cmd)
   target=pat.find('myri0')
   start=pat.find('inet addr:', target) + 10
   end=pat.find(' ', start)
   addr=pat[start:end]
   nodes = os.environ.get("NODES")
#   DpcldListenerPort='00000015001'
   listener_port = DpcldListenerPort[-5:]
#       Trying to form a statement of this type/form
#        bpsh 62-63 dpcld -p 141.111.5.182:15002
#        bpsh <nodelist> dpcld -p <myr01_inet_addr>:<listener_port>
   cmd = 'bpsh ' + nodes + ' dpcld -p '+ addr + ':' + listener_port + ' *'
   print cmd
   mach=commands.getoutput('uname -n')
   cmd1 = 'dpcld -p ' + mach + ':' + listener_port + ' &'
   print cmd1
   cmd = cmd + ' ; ' + cmd1
   print cmd

   SDPCL()
else:
   DPCL()



