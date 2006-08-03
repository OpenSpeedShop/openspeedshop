################################################################################
# Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

import os
import os.path



################################################################################
# DPCL Daemon Startup Function
################################################################################

def StartDPCL(template="*", installed="/usr"):
    """Start DPCL daemon(s) for Open|SpeedShop.

    Start the DPCL daemon(s) and attach it/them to this Open|SpeedShop session.
    Assembles a command for setting all the necessary environment variables and
    lauching the daemon(s). That command is then executed using system().

    The parameter 'installed' indicates the directory in which DPCL/Dyninst is
    installed and is used to configure the environment. The parameter 'template'
    specifies the format of the command to be used to start the daemon. This
    string should contain a single '*' symbol that is replaced with the actual
    command for starting the daemon. For example, if MPI is to be used to invoke
    the daemon on 128 nodes, the template might be 'mpirun -np 128 *' which
    would then be replaced with 'mpirun -np 128 env ... dpcld -p ...' before
    actual execution.
    """

    # Locate the DPCL dameon
    daemon = installed + "/bin/dpcld"
    if not os.path.isfile(daemon):
        raise RuntimeError("Failed to locate the DPCL Daemon.")

    # Locate the DPCL library directory
    libdir = installed + "/lib64"
    if not os.path.isdir(libdir):
        libdir = installed + "/lib"
    if not os.path.isdir(libdir):
        raise RuntimeError("Failed to locate the DPCL library directory.")

    # Form the command that will start the daemons
    if template.count("*") is not 1:
        raise RuntimeError("Template string must contain exactly one * symbol.")
    command = "env" + \
              " LD_LIBRARY_PATH=" + libdir + \
              " DYNINSTAPI_RT_LIB=" + libdir + "/libdyninstAPI_RT.so.1" \
              " DPCL_RT_LIB=" + libdir + "/libdpclRT.so.1" + \
              " " + daemon + " -p " + DpcldListenerPort
    command = string.replace(template, "*", command)
    command = command + " &"

    # Execute the command to start the daemon(s)
    print "\nStarting DPCL daemon(s) using the command \"%s\"...\n" % command
    os.system(command)



################################################################################
# Load the user-specific Python startup file (if it exists)
################################################################################

if os.environ.has_key("HOME"):
    user_specific_startup_file = os.environ["HOME"] + "/.openss.py"
    if os.path.isfile(user_specific_startup_file):
        execfile(user_specific_startup_file)
