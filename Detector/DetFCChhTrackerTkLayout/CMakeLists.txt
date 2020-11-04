################################################################################
# Package: DetFCChhTrackerTkLayout
################################################################################
gaudi_subdir(DetFCChhTrackerTkLayout v1r0)

gaudi_depends_on_subdirs(GaudiKernel Detector/DetCommon)




find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/Detector/DetFCChhTrackerTkLayout)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/triplet DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/Detector/DetFCChhTrackerTkLayout)

gaudi_add_module(DetFCChhTrackerTkLayout
                 src/*.cpp
                 INCLUDE_DIRS FWCore DD4hep ROOT Geant4 DetCommon Eigen 
                 LINK_LIBRARIES FWCore GaudiKernel DD4hep ROOT DetCommon)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(DetFCChhTrackerTkLayout)

