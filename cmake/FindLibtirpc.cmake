# sets Libtirpc_FOUND Libtirpc_INCLUDE_DIRS

find_path(Libtirpc_INCLUDE_DIR
  HINTS /usr/include
  NAMES rpc/xdr.h
  PATH_SUFFIXES tirpc
)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Libtirpc REQUIRED_VARS Libtirpc_INCLUDE_DIR)

find_library(Libtirpc_LIBRARY_SHARED NAMES tirpc
    HINTS /usr
    PATH_SUFFIXES lib lib64
    )

message(STATUS "LIBTIRPC_FOUND:" ${LIBTIRPC_FOUND})
message(STATUS "Libtirpc_FOUND:" ${Libtirpc_FOUND})
message(STATUS "Libtirpc_INCLUDE_DIR:" ${Libtirpc_INCLUDE_DIR})

if(LIBTIRPC_FOUND)
  set(Libtirpc_INCLUDE_DIRS ${Libtirpc_INCLUDE_DIR})
  set(Libtirpc_LIBRARIES ${Libtirpc_LIBRARY_SHARED})
  mark_as_advanced(
    Libtirpc_INCLUDE_DIRS Libtirpc_LIBRARIES
  )
  message(STATUS "Libtirpc_INCLUDE_DIRS:" ${Libtirpc_INCLUDE_DIRS})
  message(STATUS "Libtirpc_LIBRARIES:" ${Libtirpc_LIBRARIES})
else()
  set(Libtirpc_INCLUDE_DIRS "")
  set(Libtirpc_LIBRARIES "")
endif()


