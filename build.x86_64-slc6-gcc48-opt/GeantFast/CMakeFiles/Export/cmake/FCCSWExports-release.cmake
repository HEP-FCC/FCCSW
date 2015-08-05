#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "albers" for configuration "Release"
set_property(TARGET albers APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(albers PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libRIO.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libTree.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libalbers.so"
  IMPORTED_SONAME_RELEASE "libalbers.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS albers )
list(APPEND _IMPORT_CHECK_FILES_FOR_albers "${_IMPORT_PREFIX}/lib/libalbers.so" )

# Import target "DataObjects" for configuration "Release"
set_property(TARGET DataObjects APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(DataObjects PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "/afs/cern.ch/sw/lcg/releases/LCG_68/HepMC/2.06.08/x86_64-slc6-gcc48-opt/lib/libHepMC.so;GaudiAlgLib;albers;GaudiUtilsLib;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_regex-gcc48-mt-1_55.so;GaudiKernel;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libRIO.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libHist.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libXMLIO.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libThread.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMatrix.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libTree.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libDataObjects.so"
  IMPORTED_SONAME_RELEASE "libDataObjects.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS DataObjects )
list(APPEND _IMPORT_CHECK_FILES_FOR_DataObjects "${_IMPORT_PREFIX}/lib/libDataObjects.so" )

# Import target "TrkGeometryUtils" for configuration "Release"
set_property(TARGET TrkGeometryUtils APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(TrkGeometryUtils PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "GaudiKernel;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGenVector.so;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libTrkGeometryUtils.so"
  IMPORTED_SONAME_RELEASE "libTrkGeometryUtils.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS TrkGeometryUtils )
list(APPEND _IMPORT_CHECK_FILES_FOR_TrkGeometryUtils "${_IMPORT_PREFIX}/lib/libTrkGeometryUtils.so" )

# Import target "TrkParametersBase" for configuration "Release"
set_property(TARGET TrkParametersBase APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(TrkParametersBase PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "TrkGeometryUtils;GaudiKernel;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGenVector.so;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libTrkParametersBase.so"
  IMPORTED_SONAME_RELEASE "libTrkParametersBase.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS TrkParametersBase )
list(APPEND _IMPORT_CHECK_FILES_FOR_TrkParametersBase "${_IMPORT_PREFIX}/lib/libTrkParametersBase.so" )

# Import target "RecoGeometry" for configuration "Release"
set_property(TARGET RecoGeometry APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(RecoGeometry PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGenVector.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGeom.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDD4hepCore.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDDSegmentation.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDD4hepRec.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDD4hepPlugins.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDD4hepG4.so;TrkParametersBase;GaudiKernel;TrkGeometryUtils;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libRecoGeometry.so"
  IMPORTED_SONAME_RELEASE "libRecoGeometry.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS RecoGeometry )
list(APPEND _IMPORT_CHECK_FILES_FOR_RecoGeometry "${_IMPORT_PREFIX}/lib/libRecoGeometry.so" )

# Import target "FWCore" for configuration "Release"
set_property(TARGET FWCore APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(FWCore PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "GaudiAlgLib;GaudiKernel;albers;GaudiUtilsLib;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_regex-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libRIO.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libHist.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libXMLIO.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libThread.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMatrix.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libTree.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libFWCore.so"
  IMPORTED_SONAME_RELEASE "libFWCore.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS FWCore )
list(APPEND _IMPORT_CHECK_FILES_FOR_FWCore "${_IMPORT_PREFIX}/lib/libFWCore.so" )

# Import target "GeantFast" for configuration "Release"
set_property(TARGET GeantFast APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GeantFast PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "G4Tree;G4FR;G4GMocren;G4visHepRep;G4RayTracer;G4VRML;G4vis_management;G4modeling;G4interfaces;G4persistency;G4analysis;G4error_propagation;G4readout;G4physicslists;G4run;G4event;G4tracking;G4parmodels;G4processes;G4digits_hits;G4track;G4particles;G4geometry;G4materials;G4graphics_reps;G4intercoms;G4global;G4clhep;G4zlib;G4geomUSolids;/afs/cern.ch/sw/lcg/releases/LCG_68/HepMC/2.06.08/x86_64-slc6-gcc48-opt/lib/libHepMC.so;/afs/cern.ch/sw/lcg/releases/LCG_68/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Cast-2.1.4.1.so;/afs/cern.ch/sw/lcg/releases/LCG_68/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Evaluator-2.1.4.1.so;/afs/cern.ch/sw/lcg/releases/LCG_68/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Exceptions-2.1.4.1.so;/afs/cern.ch/sw/lcg/releases/LCG_68/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-GenericFunctions-2.1.4.1.so;/afs/cern.ch/sw/lcg/releases/LCG_68/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Geometry-2.1.4.1.so;/afs/cern.ch/sw/lcg/releases/LCG_68/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Random-2.1.4.1.so;/afs/cern.ch/sw/lcg/releases/LCG_68/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-RandomObjects-2.1.4.1.so;/afs/cern.ch/sw/lcg/releases/LCG_68/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-RefCount-2.1.4.1.so;/afs/cern.ch/sw/lcg/releases/LCG_68/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Vector-2.1.4.1.so;/afs/cern.ch/sw/lcg/releases/LCG_68/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Matrix-2.1.4.1.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/MCGenerators/pythia8/186/x86_64-slc6-gcc48-opt/lib/libpythia8.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libGeantFast.so"
  IMPORTED_SONAME_RELEASE "libGeantFast.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS GeantFast )
list(APPEND _IMPORT_CHECK_FILES_FOR_GeantFast "${_IMPORT_PREFIX}/lib/libGeantFast.so" )

# Import target "TrkChargedParameters" for configuration "Release"
set_property(TARGET TrkChargedParameters APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(TrkChargedParameters PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "TrkParametersBase;RecoGeometry;TrkGeometryUtils;GaudiKernel;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGenVector.so;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGeom.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDD4hepCore.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDDSegmentation.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDD4hepRec.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDD4hepPlugins.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDD4hepG4.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libTrkChargedParameters.so"
  IMPORTED_SONAME_RELEASE "libTrkChargedParameters.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS TrkChargedParameters )
list(APPEND _IMPORT_CHECK_FILES_FOR_TrkChargedParameters "${_IMPORT_PREFIX}/lib/libTrkChargedParameters.so" )

# Import target "TrkNeutralParameters" for configuration "Release"
set_property(TARGET TrkNeutralParameters APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(TrkNeutralParameters PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "TrkParametersBase;RecoGeometry;TrkGeometryUtils;GaudiKernel;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGenVector.so;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libGeom.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDD4hepCore.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDDSegmentation.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDD4hepRec.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDD4hepPlugins.so;/afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/lib/libDD4hepG4.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libTrkNeutralParameters.so"
  IMPORTED_SONAME_RELEASE "libTrkNeutralParameters.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS TrkNeutralParameters )
list(APPEND _IMPORT_CHECK_FILES_FOR_TrkNeutralParameters "${_IMPORT_PREFIX}/lib/libTrkNeutralParameters.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
