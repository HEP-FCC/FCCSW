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

# Import target "FWCore" for configuration "Release"
set_property(TARGET FWCore APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(FWCore PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "GaudiAlgLib;GaudiKernel;albers;GaudiUtilsLib;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_regex-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libRIO.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libHist.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libXMLIO.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libThread.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMatrix.so;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libMathCore.so;dl;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_filesystem-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_thread-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib/libboost_system-gcc48-mt-1_55.so;/afs/cern.ch/sw/lcg/releases/LCG_68/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib/libtbb.so;GaudiPluginService;/afs/cern.ch/sw/lcg/releases/LCG_68/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libTree.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libFWCore.so"
  IMPORTED_SONAME_RELEASE "libFWCore.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS FWCore )
list(APPEND _IMPORT_CHECK_FILES_FOR_FWCore "${_IMPORT_PREFIX}/lib/libFWCore.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
