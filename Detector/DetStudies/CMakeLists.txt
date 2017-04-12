################################################################################
# Package: DetStudies
################################################################################
gaudi_subdir(DetStudies v1r0)

gaudi_depends_on_subdirs(GaudiKernel
                         FWCore
                         Detector/DetInterface)

find_package(ROOT COMPONENTS MathCore GenVector Geom Tree)
find_package(DD4hep COMPONENTS DDSegmentation REQUIRED)
find_package(FCCEDM)
find_package(PODIO)
find_package(CLHEP)

gaudi_add_module(DetStudies
                 src/components/*.cpp
                 INCLUDE_DIRS GaudiKernel ROOT DD4hep FWCore CLHEP
                 LINK_LIBRARIES GaudiKernel ROOT DD4hep ${DD4hep_COMPONENT_LIBRARIES} FWCore CLHEP)

include(CTest)
gaudi_add_test(samplingFractionInclinedEcal
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               ENVIRONMENT LD_PRELOAD+=build.$BINARY_TAG/lib/libDetSegmentation.so
               FRAMEWORK tests/options/samplingFraction_inclinedEcal.py)

gaudi_add_test(positionsInclinedEcal
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               ENVIRONMENT LD_PRELOAD+=build.$BINARY_TAG/lib/libDetSegmentation.so
               DEPENDS simulateInclinedEcal
               FRAMEWORK tests/options/positions_inclinedEcal.py)

gaudi_add_test(positionsInclinedEcalCheckNonZero
               ENVIRONMENT PYTHONPATH+=$ENV{PODIO}/python
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND python Detector/DetStudies/tests/scripts/check_nonzero_inclinedEcal_positions.py
               DEPENDS simulateInclinedEcal positionsInclinedEcal)
