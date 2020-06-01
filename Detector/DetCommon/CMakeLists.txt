################################################################################
# Package: DetCommon
################################################################################
gaudi_subdir(DetCommon v1r0)

gaudi_depends_on_subdirs(Detector/DetSegmentation)



include(${Geant4_USE_FILE})
find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

gaudi_add_library(DetCommon
                 src/*.cpp
                 INCLUDE_DIRS DD4hep ROOT Geant4 DetSegmentation
                 LINK_LIBRARIES GaudiKernel DD4hep ROOT DetSegmentation
                 PUBLIC_HEADERS DetCommon)
target_link_libraries(DetCommon ${Geant4_LIBRARIES} DD4hep::DDG4)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/Detector/DetCommon)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(DetCommon)

include(CTest)
gaudi_add_test(DumpSimpleBox
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/dumpSimpleBox.py)
