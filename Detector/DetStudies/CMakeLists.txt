################################################################################
#Package : DetStudies
################################################################################
gaudi_subdir(DetStudies v1r0)

gaudi_depends_on_subdirs(GaudiKernel
                         FWCore
                         Detector/DetInterface)

find_package(ROOT COMPONENTS MathCore GenVector Geom Tree)

find_package(FCCEDM)
find_package(PODIO)
find_package(CLHEP)

gaudi_add_module(DetStudies
                 src/components/*.cpp
                 INCLUDE_DIRS GaudiKernel ROOT DD4hep FWCore CLHEP FCCEDM
                 LINK_LIBRARIES GaudiKernel ROOT DD4hep ${DD4hep_COMPONENT_LIBRARIES} FWCore CLHEP FCCEDM)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/tests DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/DetStudies)

include(CTest)
gaudi_add_test(samplingFractionInclinedEcal
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/samplingFraction_inclinedEcal.py)

gaudi_add_test(upstreamMaterialInclinedEcal
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/upstreamMaterial_inclinedEcal.py)
