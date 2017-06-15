################################################################################
#Package : DetComponents
################################################################################
gaudi_subdir(DetComponents v1r0)

gaudi_depends_on_subdirs(GaudiKernel
                         FWCore
                         Detector/DetInterface
                         Detector/DetSegmentation
                         Detector/DetCommon
                         Detector/DetExtensions)

find_package(ROOT COMPONENTS MathCore GenVector Geom Tree)

find_package(DD4hep COMPONENTS DDG4 DDSegmentation DDRec REQUIRED)
find_package(ACTS COMPONENTS Core Examples TGeoPlugin DD4hepPlugin)

find_package(Geant4)
include(${Geant4_USE_FILE})
gaudi_add_module(DetComponents
                 src/*.cpp
                 INCLUDE_DIRS GaudiKernel ROOT DD4hep Geant4 FWCore DetInterface DetExtensions DetSegmentation DetCommon ACTS
                 LINK_LIBRARIES GaudiKernel ROOT DD4hep ${DD4hep_COMPONENT_LIBRARIES} DetSegmentation DetCommon Geant4 FWCore ACTS::ACTSCore ACTS::ACTSDD4hepPlugin)

include(CTest)
gaudi_add_test(RedoSegmentationXYZ
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/redoSegmentationXYZ.py)
gaudi_add_test(RedoSegmentationPhiEta
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/redoSegmentationPhiEta.py)
gaudi_add_test(MergeCells
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/mergeCells.py)
gaudi_add_test(MergeLayers
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/mergeLayers.py)
gaudi_add_test(positionsInclinedEcal
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/positions_inclinedEcal.py)
gaudi_add_test(positionsInclinedEcalCheck
               ENVIRONMENT PYTHONPATH+=$ENV{PODIO}/python
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND python Detector/DetComponents/tests/scripts/check_inclinedEcal_positions.py
               DEPENDS positionsInclinedEcal)
gaudi_add_test(positionsTracker
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/positions_tracker.py)
