# Install script for directory: /afs/cern.ch/user/j/jhrdinka/FCC/FCCSW

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/InstallArea/x86_64-slc6-gcc48-dbg")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Debug")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE DIRECTORY FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/cmake/" FILES_MATCHING REGEX "/[^/]*\\.cmake$" REGEX "/\\.svn$" EXCLUDE)
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/scripts" TYPE PROGRAM OPTIONAL FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/cmake/env.py")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/scripts" TYPE DIRECTORY FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/cmake/EnvConfig" FILES_MATCHING REGEX "/[^/]*\\.py$" REGEX "/[^/]*\\.conf$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/include/FCCSW_VERSION.h")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(NOT EXISTS /afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/lib/FCCSW.confdb)
                  message(WARNING "creating partial /afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/lib/FCCSW.confdb")
                  execute_process(COMMAND /afs/cern.ch/sw/lcg/releases/LCG_68/Python/2.7.6/x86_64-slc6-gcc48-opt/bin/python;/afs/cern.ch/user/j/jhrdinka/FCC/GAUDI/GAUDI_v25r2/InstallArea/x86_64-slc6-gcc48-opt/scripts/merge_files.py;--no-stamp --ignore-missing /afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/DetDesServices/genConf/DetDesServices/DetDesServices.confdb;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/Detectors/genConf/Detectors/Detectors.confdb;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/DetConverterTools/Geant4GeoConverter/genConf/Geant4GeoConverter/Geant4GeoConverter.confdb;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/DetConverterTools/RecoGeoConverter/genConf/RecoGeoConverter/RecoGeoConverter.confdb;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/FWCore/genConf/FWCore/FWCorePlugins.confdb;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Examples/genConf/Examples/Examples.confdb;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Generation/genConf/Generation/Generation.confdb;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Reconstruction/genConf/Reconstruction/Reconstruction.confdb;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Simulation/genConf/Simulation/Simulation.confdb;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Test/TestAlgorithms/genConf/TestAlgorithms/TestAlgorithms.confdb /afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/lib/FCCSW.confdb)
                  endif()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/lib/FCCSW.confdb")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(NOT EXISTS /afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/lib/FCCSW.components)
                  message(WARNING "creating partial /afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/lib/FCCSW.components")
                  execute_process(COMMAND /afs/cern.ch/sw/lcg/releases/LCG_68/Python/2.7.6/x86_64-slc6-gcc48-opt/bin/python;/afs/cern.ch/user/j/jhrdinka/FCC/GAUDI/GAUDI_v25r2/InstallArea/x86_64-slc6-gcc48-opt/scripts/merge_files.py;--no-stamp --ignore-missing /afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/DetDesServices/DetDesServices.components;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/Detectors/Detectors.components;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/DetConverterTools/Geant4GeoConverter/Geant4GeoConverter.components;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/DetConverterTools/RecoGeoConverter/RecoGeoConverter.components;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/FWCore/FWCorePlugins.components;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Examples/Examples.components;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Generation/Generation.components;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Reconstruction/Reconstruction.components;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Simulation/Simulation.components;/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Test/TestAlgorithms/TestAlgorithms.components /afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/lib/FCCSW.components)
                  endif()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/lib/FCCSW.components")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(NOT EXISTS /afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/lib/FCCSWDict.rootmap)
                  message(WARNING "creating partial /afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/lib/FCCSWDict.rootmap")
                  execute_process(COMMAND /afs/cern.ch/sw/lcg/releases/LCG_68/Python/2.7.6/x86_64-slc6-gcc48-opt/bin/python;/afs/cern.ch/user/j/jhrdinka/FCC/GAUDI/GAUDI_v25r2/InstallArea/x86_64-slc6-gcc48-opt/scripts/merge_files.py;--no-stamp --ignore-missing /afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DataObjects/DataObjectsDict.rootmap /afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/lib/FCCSWDict.rootmap)
                  endif()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/lib/FCCSWDict.rootmap")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/FCCSWEnvironment.xml")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/FCCSWConfigVersion.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/FCCSWConfig.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/FCCSWPlatformConfig.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/albersExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/AlgebraExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/DataObjectsExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/DetDesInterfacesExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/IConverterExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/TrkGeometryUtilsExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/TrkParametersBaseExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/DetExtensionsExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/RecoGeometryExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/DetDesServicesExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/DetectorsExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/Geant4GeoConverterExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/RecoGeoConverterExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/FWCoreExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/ExamplesExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/GenerationExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/ReconstructionExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/SimulationExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/TestAlgorithmsExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/TrkChargedParametersExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/TrkNeutralParametersExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/config/manifest.xml")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/albers/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Algebra/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DataObjects/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/DetDesInterfaces/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/DetConverterTools/IConverter/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Tracking/TrkGeometry/TrkGeometryUtils/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Tracking/TrkEvent/TrkParametersBase/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/DetExtensions/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/RecoGeometry/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/DetDesServices/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/Detectors/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/DetConverterTools/Geant4GeoConverter/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/DetectorDescription/DetConverterTools/RecoGeoConverter/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/FWCore/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Examples/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Generation/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Reconstruction/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Simulation/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Test/TestAlgorithms/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Tracking/TrkEvent/TrkChargedParameters/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/Tracking/TrkEvent/TrkNeutralParameters/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
