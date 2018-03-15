################################################################################
# Package: DetFCChhTrackerTkLayout
################################################################################
gaudi_subdir(DetFCChhTrackerTkLayout v1r0)

gaudi_depends_on_subdirs(GaudiKernel Detector/DetCommon Detector/DetActs)


find_package(DD4hep)
find_package(Geant4)
find_package(ACTS COMPONENTS Core Examples TGeoPlugin DD4hepPlugin)
include(${Geant4_USE_FILE})

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake )
include( DD4hep )

find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION Detector/DetFCChhTrackerTkLayout)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/triplet DESTINATION Detector/DetFCChhTrackerTkLayout)

gaudi_add_module(DetFCChhTrackerTkLayout
                 src/*.cpp
                 INCLUDE_DIRS DD4hep ROOT Geant4 DetCommon DetActs  ACTS
                 LINK_LIBRARIES GaudiKernel DD4hep ROOT Geant4 DetCommon DetActs ACTS::ACTSCore ACTS::ACTSDD4hepPlugin )

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(DetFCChhTrackerTkLayout)

