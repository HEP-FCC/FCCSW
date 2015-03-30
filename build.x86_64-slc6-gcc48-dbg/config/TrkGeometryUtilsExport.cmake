# File automatically generated: DO NOT EDIT.

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

add_library(TrkGeometryUtils SHARED IMPORTED)
set_target_properties(TrkGeometryUtils PROPERTIES
  REQUIRED_INCLUDE_DIRS "GaudiKernel;GaudiPluginService;Boost;ROOT;TBB;Algebra"
  REQUIRED_LIBRARIES "GaudiKernel;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGenVector.so;dl;${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;${LCG_releases}/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService"
  IMPORTED_SONAME "libTrkGeometryUtils.so"
  IMPORTED_LOCATION "${_IMPORT_PREFIX}/lib/libTrkGeometryUtils.so"
  )
set(Tracking/TrkGeometry/TrkGeometryUtils_DEPENDENCIES GaudiKernel;Algebra)
set(Tracking/TrkGeometry/TrkGeometryUtils_VERSION v1r0)
