#!/bin/bash

rm -rf build-cbtf
mkdir build-cbtf
pushd build-cbtf

export KRELL_ROOT=/opt/DEVEL/krellroot_v2.2.4
export MRNET_ROOT=/opt/DEVEL/krellroot_v2.2.4
export DYNINST_ROOT=/opt/DEVEL/krellroot_v2.2.4
export CBTF_KRELL_INSTALL_DIR=/opt/DEVEL/cbtf_v2.2.4
export CBTF_ARGO_INSTALL_DIR=/opt/DEVEL/cbtf_v2.2.4
export CBTF_INSTALL_DIR=/opt/DEVEL/cbtf_v2.2.4
export OSS_CBTF_INSTALL_DIR=/opt/DEVEL/osscbtf_v2.2.4
export XERCESC_ROOT=/opt/DEVEL/krellroot_v2.2.4

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
       -DLIBELF_DIR=${KRELL_ROOT} \
       -DLIBDWARF_DIR=${KRELL_ROOT} \
       -DDYNINST_DIR=${DYNINST_ROOT} \
       -DXERCESC_DIR=${XERCESC_ROOT} \
       -DMRNET_DIR=${MRNET_ROOT}

make clean
make
make install

