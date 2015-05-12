# Install script for directory: /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/InstallArea/x86_64-slc6-gcc48-opt")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE DIRECTORY FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/cmake/" FILES_MATCHING REGEX "/[^/]*\\.cmake$" REGEX "/\\.svn$" EXCLUDE)
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/scripts" TYPE PROGRAM OPTIONAL FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/cmake/env.py")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/scripts" TYPE DIRECTORY FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/cmake/EnvConfig" FILES_MATCHING REGEX "/[^/]*\\.py$" REGEX "/[^/]*\\.conf$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/include/FCCSW_VERSION.h")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(NOT EXISTS /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/lib/FCCSW.confdb)
                  message(WARNING "creating partial /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/lib/FCCSW.confdb")
                  execute_process(COMMAND /afs/cern.ch/sw/lcg/releases/LCG_68/Python/2.7.6/x86_64-slc6-gcc48-opt/bin/python;/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/GAUDI/GAUDI_v25r2/InstallArea/x86_64-slc6-gcc48-opt/scripts/merge_files.py;--no-stamp --ignore-missing /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/FWCore/genConf/FWCore/FWCorePlugins.confdb;/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Examples/genConf/Examples/Examples.confdb;/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Reconstruction/genConf/Reconstruction/Reconstruction.confdb;/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Simulation/genConf/Simulation/Simulation.confdb;/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Generators/genConf/Generation/Generation.confdb /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/lib/FCCSW.confdb)
                  endif()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/lib/FCCSW.confdb")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(NOT EXISTS /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/lib/FCCSW.components)
                  message(WARNING "creating partial /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/lib/FCCSW.components")
                  execute_process(COMMAND /afs/cern.ch/sw/lcg/releases/LCG_68/Python/2.7.6/x86_64-slc6-gcc48-opt/bin/python;/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/GAUDI/GAUDI_v25r2/InstallArea/x86_64-slc6-gcc48-opt/scripts/merge_files.py;--no-stamp --ignore-missing /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/FWCore/FWCorePlugins.components;/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Examples/Examples.components;/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Reconstruction/Reconstruction.components;/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Simulation/Simulation.components;/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Generators/Generation.components /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/lib/FCCSW.components)
                  endif()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/lib/FCCSW.components")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(NOT EXISTS /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/lib/FCCSWDict.rootmap)
                  message(WARNING "creating partial /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/lib/FCCSWDict.rootmap")
                  execute_process(COMMAND /afs/cern.ch/sw/lcg/releases/LCG_68/Python/2.7.6/x86_64-slc6-gcc48-opt/bin/python;/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/GAUDI/GAUDI_v25r2/InstallArea/x86_64-slc6-gcc48-opt/scripts/merge_files.py;--no-stamp --ignore-missing /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/DataObjects/DataObjectsDict.rootmap /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/lib/FCCSWDict.rootmap)
                  endif()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/lib/FCCSWDict.rootmap")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/FCCSWEnvironment.xml")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/FCCSWConfigVersion.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/FCCSWConfig.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/FCCSWPlatformConfig.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/albersExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/DataObjectsExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/FWCoreExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/ExamplesExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/GeantFastExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/ReconstructionExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/SimulationExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/GeneratorsExport.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/manifest.xml")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/albers/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/DataObjects/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/FWCore/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Examples/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/GeantFast/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Reconstruction/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Simulation/cmake_install.cmake")
  INCLUDE("/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Generators/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
