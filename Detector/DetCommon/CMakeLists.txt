################################################################################
# Package: DetCommon
################################################################################
gaudi_subdir(DetCommon v1r0)

gaudi_depends_on_subdirs(GaudiKernel Detector/DetSegmentation)

find_package(DD4hep COMPONENTS DDG4 DDSegmentation REQUIRED NO_CMAKE_SYSTEM_PATH)
find_package(Geant4 NO_CMAKE_SYSTEM_PATH)
include(${Geant4_USE_FILE})

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake )
include( DD4hep )

find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED NO_CMAKE_SYSTEM_PATH)

gaudi_install_headers(DetCommon)
gaudi_add_library(DetCommon
                 src/*.cpp
                 INCLUDE_DIRS DD4hep ROOT Geant4 DetSegmentation
                 LINK_LIBRARIES GaudiKernel DD4hep ROOT Geant4 DetSegmentation ${DD4hep_COMPONENT_LIBRARIES}
                 PUBLIC_HEADERS DetCommon)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(DetCommon)

include(CTest)
gaudi_add_test(DumpSimpleBox
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/dumpSimpleBox.py)
