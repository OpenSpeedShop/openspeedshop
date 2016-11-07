
Please see the BuildInstallGuide.txt in the release directory for the latest build
and install information.  That document is based on the "pdf" version 
of the Build and Install Guide that is available in the Files download 
area for this OpenSpeedShop release on sourceforge.

Both documents have valid Open|SpeedShop-2.3 release information.

These are some sample cmake build scripts to use as an example:

-------------------------------------------------------------------

-------------------------------------------------------------------
BUILDING OPENSPEEDSHOP for the cbtf instrumentor:
-------------------------------------------------------------------

LISTING OF THE SCRIPT:

$ cat run_cmake_osscbtf_laptop.sh
rm -rf build-cbtf
mkdir build-cbtf
pushd build-cbtf

export KRELL_ROOT=/opt/krellroot_v2.3.0
export MRNET_ROOT=/opt/krellroot_v2.3.0
export DYNINST_ROOT=/opt/krellroot_v2.3.0
export CBTF_KRELL_INSTALL_DIR=/opt/cbtf_v2.3.0
export CBTF_ARGO_INSTALL_DIR=/opt/cbtf_v2.3.0
export CBTF_INSTALL_DIR=/opt/cbtf_v2.3.0
export OSS_CBTF_INSTALL_DIR=/opt/osscbtf_v2.3.0

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
       -DMRNET_DIR=${MRNET_ROOT} 

make clean
make
make install

RUN THE SCRIPT TO BUILD:
$ sh run_cmake_osscbtf_laptop.sh



