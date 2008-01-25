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

debug = True #Enables Simple Debugging Output

def LDPCL(template='*'):
     if(debug):
          print 'LDPCL Called....With template = ' + template
     StartDPCLwithLibs(template, os.environ['OPENSS_PREFIX'],
                       os.environ['OPENSS_PREFIX'])

def DPCL():
     if(debug):
          print 'DPCL Called...'
     print 'Starting DPCL daemon on local node...'
     LDPCL('*')

def SDPCL(templateString):
     if(debug):
          print 'SDPCL Called, templateString=',templateString
     if (os.environ.has_key('SLURM_JOBID')):
          if(debug):
               print 'CPUs Is: ',os.environ['SLURM_CPUS_ON_NODE']
               print 'SLURM_JOBID Defined!..'
          LDPCL('srun -c 8 -l *')
     elif (os.environ.has_key('PBS_JOBID')):
          if(debug):
               print 'PBS_JOBID Defined!..'
          LDPCL(templateString)
     else:
          print 'No Job ID defined'

def PDPCL(sid):
     print 'Starting DPCL daemons for SLURM job ',sid
     LDPCL('srun -c 8 --jobid '+str(sid)+' *')

#Parses ifconfig output
#Returns myr0 inet addr string
def getMyrInetAddr():
     pat = commands.getoutput('/sbin/ifconfig')
     target = pat.find('myri0')
     start = pat.find('inet addr:', target) + 10
     end = pat.find(' ', start)
     return pat[start:end]     

def main():
     if (os.environ.has_key('SLURM_JOBID')):
          SDPCL()
     elif (os.environ.has_key('PBS_JOBID')):
          nodes = os.environ.get('NODES')
          listener_port = DpcldListenerPort[-5:]
          if(debug):
               print "DpcldListenerPort Value: " + listener_port
          bpshstr = ('bpsh ' + nodes + ' dpcld -p '+ getMyrInetAddr() + ':' +
                 listener_port)
          if(debug):
               print 'BPSH Command: ' + bpshstr
          mach = commands.getoutput('uname -n')
          machstr = 'dpcld -p ' + mach + ':' + listener_port
          if(debug):
               print 'Machine Command: ' + machstr
#          tcshcmd = 'tcsh -c ' + '"' + bpshstr + '& ' + machstr + '&' + '"'
#         if(debug):
#             print 'About To Exec: ' + tcshcmd
          #Hack...for now.
          #os.system(tcshcmd)
          #SDPCL('* ' + bpshstr)
          SDPCL(bpshstr + '& ' +'*')
#          SDPCL(bpshstr + '& ' + machstr +'*')
     else:
          DPCL()

if __name__ == '__main__' :
     if(debug):
          print 'starting...'
     main()
     if(debug):
          print 'done...'
            
