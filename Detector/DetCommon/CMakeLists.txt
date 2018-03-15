################################################################################
# Package: DetCommon
################################################################################
gaudi_subdir(DetCommon v1r0)

gaudi_depends_on_subdirs(GaudiKernel Detector/DetSegmentation)

find_package(DD4hep COMPONENTS DDG4 DDSegmentation REQUIRED)
find_package(Geant4)
include(${Geant4_USE_FILE})


find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

gaudi_install_headers(DetCommon)
gaudi_add_library(DetCommon
                 src/*.cpp

                 INCLUDE_DIRS DD4hep ROOT Geant4 DetSegmentation
                 LINK_LIBRARIES GaudiKernel DD4hep ROOT Geant4 DetSegmentation
                 PUBLIC_HEADERS DetCommon)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION Detector/DetCommon)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(DetCommon)

include(CTest)
gaudi_add_test(DumpSimpleBox
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/dumpSimpleBox.py)
