# File automatically generated: DO NOT EDIT.

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

add_library(RecoGeometry SHARED IMPORTED)
set_target_properties(RecoGeometry PROPERTIES
  REQUIRED_INCLUDE_DIRS "TrkParametersBase;TrkGeometryUtils;GaudiKernel;GaudiPluginService;Boost;ROOT;TBB;Algebra;DD4hep;DetExtensions"
  REQUIRED_LIBRARIES "${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGenVector.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGeom.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepCore.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDDSegmentation.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepRec.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepPlugins.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepG4.so;TrkParametersBase;GaudiKernel;TrkGeometryUtils;dl;${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;${LCG_releases}/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService"
  IMPORTED_SONAME "libRecoGeometry.so"
  IMPORTED_LOCATION "${_IMPORT_PREFIX}/lib/libRecoGeometry.so"
  )
set(RecoGeometry_DEPENDENCIES Algebra;Tracking/TrkEvent/TrkParametersBase;Tracking/TrkGeometry/TrkGeometryUtils;DetectorDescription/DetExtensions;GaudiKernel)
set(RecoGeometry_VERSION v1r0)
