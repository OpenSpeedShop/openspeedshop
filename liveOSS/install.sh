#!/bin/bash 

# Package Version Numbers

default_oss_prefix=/opt/OSS

# Variables for the search for prerequisite components.

# Identifies missing prerequisites

# Prints Script Usage
function usage() {
    cat << EOF
    usage:
        install.sh [--with-option choiceNum] install.sh [-h | --help]

EOF
}

# Prints Script Info
function about() {
    cat << EOF

    ---------------------------------------------------------------------------
    This script builds RPMs and supports installation as non-root through cpio
    
    The default install prefix is /opt/OSS. If another is preferred, please set
    the OPENSS_PREFIX environment variable to reflect the new target directory   
     
    If you plan on installing RPMs as root, it will let you know at what point
    ---------------------------------------------------------------------------

EOF
}

# Prints Script Choices
function choices() {
    cat << EOF
    Choices:

    0  - Set ~/.rpmmacros %_topdir to:
         $PWD  
         original will be copied to .rpmmacros.##
    0a - View environment variables that may be set
    
EOF
}

# Print Important Environmental Variables
function envvars() {
    cat << EOF
    
    OPENSS Environment Variables:
        -General
         OPENSS_PREFIX           set to alternate install prefix.  default is /opt/OSS
         OPENSS_INSTRUMENTOR     set to the underlying instrumentation type openss will use.  default is mrnet (includes offline)
        
        -Open|SpeedShop MPI and Vampirtrace
         OPENSS_MPI_LAM          set to MPI LAM installation dir. default is null.
         OPENSS_MPI_LAMPI        set to MPI LAMPI installation dir. default is null.
         OPENSS_MPI_OPENMPI      set to MPI OPENMPI installation dir. default is null.
         OPENSS_MPI_MPICH        set to MPI MPICH installation dir. default is null.
         OPENSS_MPI_MPICH2       set to MPI MPICH2 installation dir. default is null.
         OPENSS_MPI_MPICH_DRIVER mpich/mpich2 driver name [ch-p4]. default is null.
         OPENSS_MPI_MPT          set to SGI MPI MPT installation dir. default is null.
         OPENSS_MPI_MVAPICH      set to MPI MVAPICH installation dir. default is null.
         OPENSS_MPI_MVAPICH2     set to MPI MVAPICH2 installation dir. default is null.
         OPENSS_OFED             set to OPEN FABRICS installation dir. default is /usr.
         OPENSS_BINUTILS         set to alternative directory location for binutils components. default is /usr
         OPENSS_PYTHON           set to alternative directory location for python components. default is /usr
         OPENSS_IMPLICIT_TLS     When set, this enables Open|SpeedShop to use implicitly created tls storage. default is explicit.
    
EOF

}

# Print General OSS Getting Started Info
function default_envs() {
    cat << EOF
    
    Current Values for OPENSS Environment Variables:
        -General

EOF
	if [ $OPENSS_PREFIX ]; then
		echo "         Using OPENSS_PREFIX=$OPENSS_PREFIX"
	fi       
	if [ $OPENSS_INSTRUMENTOR ]; then
		echo "         Using OPENSS_INSTRUMENTOR=$OPENSS_INSTRUMENTOR"
	fi       
	if [ $OPENSS_BINUTILS ]; then
		echo "         Using OPENSS_BINUTILS=$OPENSS_BINUTILS"
	fi       
	if [ $OPENSS_OFED ]; then
		echo "         Using OPENSS_OFED=$OPENSS_OFED"
	fi       
	if [ $OPENSS_PYTHON ]; then
		echo "         Using OPENSS_PYTHON=$OPENSS_PYTHON"
	fi       
	if [ $OPENSS_EXPLICIT_TLS ]; then
		echo "         Using explicitly generated Thread local storage areas within Open|SpeedShop"
	fi       
	if [ $OPENSS_IMPLICIT_TLS ]; then
		echo "         Using implicitly generated Thread local storage areas within Open|SpeedShop"
	fi       
    cat << EOF
    
        -Open|SpeedShop MPI and Vampirtrace

EOF
	if [ $OPENSS_MPI_LAM ]; then
		echo "         Using OPENSS_MPI_LAM=$OPENSS_MPI_LAM"
	fi       
	if [ $OPENSS_MPI_LAMPI ]; then
		echo "         Using OPENSS_MPI_LAMPI=$OPENSS_MPI_LAMPI"
	fi       
	if [ $OPENSS_MPI_OPENMPI ]; then
		echo "         Using OPENSS_MPI_OPENMPI=$OPENSS_MPI_OPENMPI"
	fi       
	if [ $OPENSS_MPI_MPICH ]; then
		echo "         Using OPENSS_MPI_MPICH=$OPENSS_MPI_MPICH"
	fi       
	if [ $OPENSS_MPI_MPICH_DRIVER ]; then
		echo "         Using OPENSS_MPI_MPICH_DRIVER=$OPENSS_MPI_MPICH_DRIVER"
	fi       
	if [ $OPENSS_MPI_MPICH2 ]; then
		echo "         Using OPENSS_MPI_MPICH2=$OPENSS_MPI_MPICH2"
	fi       
	if [ $OPENSS_MPI_MPT ]; then
		echo "         Using OPENSS_MPI_MPT=$OPENSS_MPI_MPT"
	fi       
	if [ $OPENSS_MPI_MVAPICH ]; then
		echo "         Using OPENSS_MPI_MVAPICH=$OPENSS_MPI_MVAPICH"
	fi       
	if [ $OPENSS_MPI_MVAPICH2 ]; then
		echo "         Using OPENSS_MPI_MVAPICH2=$OPENSS_MPI_MVAPICH2"
	fi       
    cat << EOF

EOF
}

# Print General OSS Getting Started Info
function getstarted() {
	if [ $OPENSS_INSTRUMENTOR -a $OPENSS_INSTRUMENTOR == "mrnet" ]; then
		cat << EOF
        
        In the base case you need to add prefix/bin to your PATH variable
        and prefix/<lib> path to your LD_LIBRARY_PATH environment variable
        
        If you use module files - a base module file is located in 
        OpenSpeedShop/startup_files that you can use - just change the prefix
        to your actual install location
EOF
        else
		cat << EOF
        
        In the base case you need to add prefix/bin to your PATH variable
        and prefix/<lib> path to your LD_LIBRARY_PATH environment variable
        
        If you use module files - a base module file is located in 
        OpenSpeedShop/startup_files that you can use - just change the prefix
        to your actual install location
EOF
	fi       

}

# Print instrumentor information.  If $OPENSS_INSTRUMENTOR is not defined,
# OPENSS_INSTRUMENTOR is set to mrnet.
function instrumentor() {
    if [ $OPENSS_INSTRUMENTOR ]; then
        cat << EOF

    You are building for Open|SpeedShop instrumentor: $OPENSS_INSTRUMENTOR
    You have manually specified a Open|SpeedShop instrumentor using
    the environment variable OPENSS_INSTRUMENTOR to override the
    default Open|SpeedShop instrumentor: mrnet.
        
EOF
    else
        cat << EOF

    You are building for default MRNet Open|SpeedShop instrumentor: mrnet.
    If you wish to change the Open|SpeedShop instrumentor, use
    the OPENSS_INSTRUMENTOR to specify: mrnet, or offline.

EOF
            export OPENSS_INSTRUMENTOR="mrnet"
    fi 
}

# Main Build Function
function build() { 

    if [ -z "$1" ]; then #If no parameter is passed to the function, we are in
        about            #interactive mode
        choices
        echo "-last option: " 
        cat LAST_OPTION
        echo "Enter Option: "
        read nanswer
    else #If parameter is passed, set nanswer and default instrumentor
        instrumentor
        nanswer=$1
    fi
    echo $nanswer > LAST_OPTION
    if [ "$nanswer" = 0  -o "$nanswer" = 9 ]; then
        echo 
        echo "Re-write ~/.rpmmacros file to point to local directories."
        echo "Your original will be saved as ~/.rpmmacros.$$ - rename manually"
        echo "when build process is completed"
        echo 
        echo "Re-Write ~/.rpmmacros? <y/n>"
        echo
        
        if [ "$nanswer" = 9 ]; then
            answer=Y
        else
            read answer
        fi
        
        if [ "$answer" = Y -o "$answer" = y ]; then
            ./Build-RPM newrpmloc
            more ~/.rpmmacros
            echo
            echo "ready to continue..."
        fi
    fi
    if [ "$nanswer" = 0a ]; then
        envvars
    fi
}
#End Functions ----------------------------------------------------------------

if [ `uname -m` = "x86_64" -o `uname -m` = " x86-64" ]; then
    LIBDIR="lib64"
else
    LIBDIR="lib"
fi

#sys=`uname -n | grep -o '[^0-9]\{0,\}'`
sys=`uname -n `
export MACHINE=$sys
echo '    machine: ' $sys

imode=true #Interactive Mode left as true if no args passed

while [ $# -gt 0 ]; do
    case "$1" in
        -h)
            about
            usage
            choices
            exit;;
       --help)
            about
            usage
            choices
            exit;;
       --with-option)
            imode=false #Arguments Passed, Switch To Arg Mode
            if [ -z "$2" ]; then #Check If Build Option is Present
                usage
                exit
            else
                optionnum=$2
            fi
            shift;;
       *)
            usage
            exit;;
    esac
    shift
done

if $imode; then
    build
else
    build $optionnum
fi

exit
