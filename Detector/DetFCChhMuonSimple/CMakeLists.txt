################################################################################
#Package : DetFCChhMuonSimple
################################################################################
gaudi_subdir(DetFCChhMuonSimple v1r0)

gaudi_depends_on_subdirs(GaudiKernel
                        Detector/DetExtensions)


find_package(DD4hep)
find_package(Geant4)
include(${Geant4_USE_FILE})

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake )

find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(DetFCChhMuonSimple)

