################################################################################
# Package: DetFCChhTrackerTkLayout
################################################################################
gaudi_subdir(DetFCChhTrackerTkLayout v1r0)

gaudi_depends_on_subdirs(GaudiKernel Detector/DetCommon)


find_package(DD4hep)
find_package(Eigen)
find_package(Geant4)
find_package(Acts COMPONENTS Core IdentificationPlugin TGeoPlugin DD4hepPlugin DigitizationPlugin)
include(${Geant4_USE_FILE})

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake )
include( DD4hep )

find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION ${CMAKE_INSTALL_DATADIR}/Detector/DetFCChhTrackerTkLayout)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/triplet DESTINATION ${CMAKE_INSTALL_DATADIR}/Detector/DetFCChhTrackerTkLayout)

gaudi_add_module(DetFCChhTrackerTkLayout
                 src/*.cpp
                 INCLUDE_DIRS FWCore DD4hep ROOT Geant4 DetCommon Eigen  Acts
                 LINK_LIBRARIES FWCore GaudiKernel DD4hep ROOT Geant4 DetCommon ActsCore ActsDD4hepPlugin ActsDigitizationPlugin)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(DetFCChhTrackerTkLayout)

