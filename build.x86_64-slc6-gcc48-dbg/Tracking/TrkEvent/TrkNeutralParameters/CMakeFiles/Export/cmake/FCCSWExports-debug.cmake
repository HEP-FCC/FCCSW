#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
SET(CMAKE_IMPORT_FILE_VERSION 1)

# Compute the installation prefix relative to this file.
GET_FILENAME_COMPONENT(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
GET_FILENAME_COMPONENT(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

# Import target "albers" for configuration "Debug"
SET_PROPERTY(TARGET albers APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(albers PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libRIO.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libTree.so"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libalbers.so"
  IMPORTED_SONAME_DEBUG "libalbers.so"
  )

# Import target "DataObjects" for configuration "Debug"
SET_PROPERTY(TARGET DataObjects APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(DataObjects PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "/afs/cern.ch/sw/lcg/releases/LCG_68/HepMC/2.06.08/x86_64-slc6-gcc48-opt/lib/libHepMC.so;GaudiAlgLib;albers;GaudiUtilsLib;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_regex-gcc48-mt-1_55.so;GaudiKernel;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libRIO.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libHist.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libXMLIO.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libThread.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMatrix.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libTree.so"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libDataObjects.so"
  IMPORTED_SONAME_DEBUG "libDataObjects.so"
  )

# Import target "TrkGeometryUtils" for configuration "Debug"
SET_PROPERTY(TARGET TrkGeometryUtils APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(TrkGeometryUtils PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "GaudiKernel;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGenVector.so;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libTrkGeometryUtils.so"
  IMPORTED_SONAME_DEBUG "libTrkGeometryUtils.so"
  )

# Import target "TrkParametersBase" for configuration "Debug"
SET_PROPERTY(TARGET TrkParametersBase APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(TrkParametersBase PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "TrkGeometryUtils;GaudiKernel;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGenVector.so;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libTrkParametersBase.so"
  IMPORTED_SONAME_DEBUG "libTrkParametersBase.so"
  )

# Import target "RecoGeometry" for configuration "Debug"
SET_PROPERTY(TARGET RecoGeometry APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(RecoGeometry PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGenVector.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGeom.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepCore.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDDSegmentation.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepRec.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepPlugins.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepG4.so;TrkParametersBase;GaudiKernel;TrkGeometryUtils;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libRecoGeometry.so"
  IMPORTED_SONAME_DEBUG "libRecoGeometry.so"
  )

# Import target "FWCore" for configuration "Debug"
SET_PROPERTY(TARGET FWCore APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(FWCore PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "GaudiAlgLib;GaudiKernel;albers;GaudiUtilsLib;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_regex-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libRIO.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libHist.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libXMLIO.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libThread.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMatrix.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libTree.so"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libFWCore.so"
  IMPORTED_SONAME_DEBUG "libFWCore.so"
  )

# Import target "TrkChargedParameters" for configuration "Debug"
SET_PROPERTY(TARGET TrkChargedParameters APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(TrkChargedParameters PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "TrkParametersBase;RecoGeometry;TrkGeometryUtils;GaudiKernel;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGenVector.so;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGeom.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepCore.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDDSegmentation.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepRec.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepPlugins.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepG4.so"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libTrkChargedParameters.so"
  IMPORTED_SONAME_DEBUG "libTrkChargedParameters.so"
  )

# Import target "TrkNeutralParameters" for configuration "Debug"
SET_PROPERTY(TARGET TrkNeutralParameters APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(TrkNeutralParameters PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "TrkParametersBase;RecoGeometry;TrkGeometryUtils;GaudiKernel;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGenVector.so;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGeom.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepCore.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDDSegmentation.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepRec.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepPlugins.so;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib/libDD4hepG4.so"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libTrkNeutralParameters.so"
  IMPORTED_SONAME_DEBUG "libTrkNeutralParameters.so"
  )

# Cleanup temporary variables.
SET(_IMPORT_PREFIX)

# Commands beyond this point should not need to know the version.
SET(CMAKE_IMPORT_FILE_VERSION)
