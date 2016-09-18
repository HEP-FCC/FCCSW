################################################################################
# Package: DetFCChhTrackerSimple
################################################################################
gaudi_subdir(DetFCChhTrackerSimple v1r0)

gaudi_depends_on_subdirs(GaudiKernel Detector/DetCommon)


find_package(DD4hep)
find_package(Geant4)
include(${Geant4_USE_FILE})

find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

find_package(ACTS COMPONENTS Core TGeoPlugin DD4hepPlugin)

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake )
include( DD4hep )

link_directories(${ROOT_INCLUDE_DIR}/../lib)

gaudi_add_module(DetFCChhTrackerSimple
                 src/*.cpp
                 INCLUDE_DIRS DD4hep ROOT Geant4 DetCommon ACTS::ACTSCore ACTS::ACTSDD4hepPlugin
                 LINK_LIBRARIES GaudiKernel DD4hep ROOT Geant4 DetCommon ACTS::ACTSCore ACTS::ACTSDD4hepPlugin)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(DetFCChhTrackerSimple)

