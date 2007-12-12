import code
import os
import os.path
import re
import string
import sys
import types
import tempfile

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
        LDPCL("mpirun -np 4 *")
    else:
        print "No Job ID defined"

def PDPCL(sid):
    print "Starting DPCL daemons for SLURM job ",sid
    LDPCL("srun -c 8 --jobid "+str(sid)+" *")


if (os.environ.has_key("SLURM_JOBID")):
   SDPCL()
elif (os.environ.has_key("PBS_JOBID")):
   SDPCL()
else:
   DPCL()



