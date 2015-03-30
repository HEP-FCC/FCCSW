# File automatically generated: DO NOT EDIT.

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

add_library(TrkChargedParameters SHARED IMPORTED)
set_target_properties(TrkChargedParameters PROPERTIES
  REQUIRED_INCLUDE_DIRS "TrkParametersBase;TrkGeometryUtils;GaudiKernel;GaudiPluginService;Boost;ROOT;TBB;Algebra;RecoGeometry;DD4hep;DetExtensions"
  REQUIRED_LIBRARIES "TrkParametersBase;RecoGeometry;TrkGeometryUtils;GaudiKernel;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGenVector.so;dl;${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;${LCG_releases}/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGeom.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepCore.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDDSegmentation.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepRec.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepPlugins.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepG4.so"
  IMPORTED_SONAME "libTrkChargedParameters.so"
  IMPORTED_LOCATION "${_IMPORT_PREFIX}/lib/libTrkChargedParameters.so"
  )
set(Tracking/TrkEvent/TrkChargedParameters_DEPENDENCIES Tracking/TrkEvent/TrkParametersBase;RecoGeometry)
set(Tracking/TrkEvent/TrkChargedParameters_VERSION v1r0)
