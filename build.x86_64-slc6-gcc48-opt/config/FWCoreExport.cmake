# File automatically generated: DO NOT EDIT.

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

add_library(FWCore SHARED IMPORTED)
set_target_properties(FWCore PROPERTIES
  REQUIRED_INCLUDE_DIRS "GaudiAlgLib;GaudiUtilsLib;GaudiKernel;GaudiPluginService;Boost;ROOT;TBB;AIDA;albers"
  REQUIRED_LIBRARIES "GaudiAlgLib;GaudiKernel;albers;GaudiUtilsLib;${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_regex-gcc48-mt-1_55.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libRIO.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libHist.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libXMLIO.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libThread.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMatrix.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;dl;${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;${LCG_releases}/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libTree.so"
  IMPORTED_SONAME "libFWCore.so"
  IMPORTED_LOCATION "${_IMPORT_PREFIX}/lib/libFWCore.so"
  )
add_library(FWCorePlugins MODULE IMPORTED)
set(FWCore_DEPENDENCIES GaudiAlg;GaudiKernel;albers;GaudiCoreSvc)
set(FWCore_VERSION v1r0)
