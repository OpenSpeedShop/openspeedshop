# the name of the target operating system
set(CMAKE_SYSTEM_NAME BlueGeneQ-dynamic)
set(_CMAKE_TOOLCHAIN_LOCATION /bgsys/drivers/ppcfloor/gnu-linux/bin)
set(_CMAKE_TOOLCHAIN_PREFIX   powerpc64-bgq-linux-)

# Make sure MPI_COMPILER wrapper matches the gnu compilers.
# Prefer local machine wrappers to driver wrappers here too.
find_program(MPI_COMPILER NAMES mpicxx mpic++ mpiCC mpicc
  PATHS
  /usr/local/bin
  /usr/bin
  /bgsys/drivers/ppcfloor/comm/gcc/bin)

# where is the target environment - point to the non-standard installations of targeted software
#SET(CMAKE_FIND_ROOT_PATH  $CBTF_KRELL_ROOT/$CBTF_TARGET_ARCH $CBTF_BOOST_ROOT/$CBTF_TARGET_ARCH)
SET(CMAKE_FIND_ROOT_PATH  /nfs/tmp2/jeg/rzseq/krell_root/bgq /nfs/tmp2/jeg/rzseq/boost_root/bgq)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

