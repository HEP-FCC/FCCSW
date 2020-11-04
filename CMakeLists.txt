CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

option(FCCSW_PAPAS "Whether to build Papas." OFF)
option(SAS "Whether to run static code analysis with SAS." OFF)
if (SAS)
    find_package(sas)
    enable_sas(
        FORMAT
        COMPARISON_REPORT_DIR "${CMAKE_BINARY_DIR}/sas_report"
        IGNORE_DIRS ${CMAKE_SOURCE_DIR}/build ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_SOURCE_DIR}/doc
        )
endif(SAS)

option(ENABLE_CPACK "Whether to enable CPack." OFF)
if(ENABLE_CPACK)
  include(CPack)
  include(cmake/FCCSWCPack.cmake)
endif()

if( NOT DEFINED ENV{LCG_VERSION})
  set(HOST_BINARY_TAG x86_64-linux-gcc9-opt)
endif()

#---------------------------------------------------------------
# to use ROOT targets, find_package ROOT must come before find_package Gaudi
# (for now) # see issue Gaudi#103
find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)
find_package(DD4hep COMPONENTS DDG4 DDRec REQUIRED)
find_package(Geant4)



# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

include(GNUInstallDirs)

# Set up C++ Standard
# ``-DCMAKE_CXX_STANDARD=<standard>`` when invoking CMake
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

if(NOT CMAKE_CXX_STANDARD MATCHES "14|17")
  message(FATAL_ERROR "Unsupported C++ standard: ${CMAKE_CXX_STANDARD}")
endif()

# Prevent CMake falls back to the latest standard the compiler does support
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disables the use of compiler-specific extensions, hence makes sure the code
# works for a wider range of compilers
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "Setting C++ standard: '${CMAKE_CXX_STANDARD}'.")

if (${APPLE})
    set(CPP_STANDARD_FLAGS "-std=c++${CMAKE_CXX_STANDARD}\ -stdlib=libc++")
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPP_STANDARD_FLAGS}")

configure_file(cmake/fccrun.in ${CMAKE_BINARY_DIR}/fccrun)
configure_file(cmake/setup.sh.in ${CMAKE_BINARY_DIR}/setup.sh)

install(FILES ${CMAKE_BINARY_DIR}/fccrun
        DESTINATION .
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE GROUP_READ
                    WORLD_EXECUTE WORLD_READ
        RENAME run)

install(FILES ${CMAKE_BINARY_DIR}/setup.sh
        DESTINATION .
        PERMISSIONS OWNER_WRITE OWNER_READ
                    GROUP_READ
                    WORLD_READ)


find_package(FCCEDM)

gaudi_project(FCCSW  v0r15
              USE Gaudi v33r1 )


