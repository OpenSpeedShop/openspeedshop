# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
#SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER   /opt/cray/xt-asyncpe/default/bin/cc -dynamic)
SET(CMAKE_CXX_COMPILER /opt/cray/xt-asyncpe/default/bin/CC -dynamic)

# where is the target environment - point to the non-standard installations of targeted software
#SET(CMAKE_FIND_ROOT_PATH  $CBTF_KRELL_ROOT/$CBTF_TARGET_ARCH $CBTF_BOOST_ROOT/$CBTF_TARGET_ARCH)
SET(CMAKE_FIND_ROOT_PATH  /users/jeg/todi/krell_root/cray-xk /users/jeg/todi/boost_root/cray-xk)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


