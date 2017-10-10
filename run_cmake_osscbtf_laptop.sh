#!/bin/bash

rm -rf build-cbtf
mkdir build-cbtf
pushd build-cbtf

export KRELL_ROOT=~/OSS/krellroot_v2.3.1
export MRNET_ROOT=~/OSS/krellroot_v2.3.1
export DYNINST_ROOT=~/OSS/krellroot_v2.3.1
export CBTF_KRELL_INSTALL_DIR=~/OSS/cbtf_v2.3.1
export CBTF_ARGO_INSTALL_DIR=~/OSS/cbtf_v2.3.1
export CBTF_INSTALL_DIR=~/OSS/cbtf_v2.3.1
export OSS_CBTF_INSTALL_DIR=~/OSS/osscbtf_v2.3.1
export XERCESC_ROOT=~/OSS/krellroot_v2.3.1
export SQLITE3_ROOT=~/OSS/krellroot_v2.3.1

# Locate the find package files in the locations that cmake expects
export MY_CMAKE_PREFIX_PATH="${CBTF_INSTALL_DIR}:${CBTF_KRELL_INSTALL_DIR}:${KRELL_ROOT}"
cmake .. \
       -DCMAKE_BUILD_TYPE=None \
       -DCMAKE_CXX_FLAGS="-g -O2" \
       -DCMAKE_C_FLAGS="-g -O2" \
       -DCMAKE_INSTALL_PREFIX=${OSS_CBTF_INSTALL_DIR}\
       -DCBTF_DIR=${CBTF_INSTALL_DIR} \
       -DCBTF_KRELL_DIR=${CBTF_KRELL_INSTALL_DIR} \
       -DCBTF_ARGONAVIS_DIR=${CBTF_ARGO_INSTALL_DIR} \
       -DINSTRUMENTOR="cbtf" \
       -DRESOLVE_SYMBOLS=symtabapi \
       -DCMAKE_PREFIX_PATH=${MY_CMAKE_PREFIX_PATH} \
       -DBINUTILS_DIR=${KRELL_ROOT} \
       -DBOOST_ROOT=${KRELL_ROOT} \
       -DLIBELF_DIR=${KRELL_ROOT} \
       -DLIBDWARF_DIR=${KRELL_ROOT} \
       -DDYNINST_DIR=${DYNINST_ROOT} \
       -DXERCESC_DIR=${XERCESC_ROOT} \
       -DSQLITE3_DIR=${SQLITE3_ROOT} \
       -DMRNET_DIR=${MRNET_ROOT}

make clean
make
make install

