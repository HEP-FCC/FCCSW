################################################################################
#Package : DetComponents
################################################################################
gaudi_subdir(DetComponents v1r0)

gaudi_depends_on_subdirs( FWCore
                         Detector/DetInterface
                         Detector/DetSegmentation
                         Detector/DetCommon)

find_package(ROOT COMPONENTS MathCore GenVector Geom Tree)

find_package(Eigen)


gaudi_add_module(DetComponents
                 src/*.cpp
                 INCLUDE_DIRS GaudiKernel ROOT DD4hep Geant4 FWCore DetInterface DetSegmentation DetCommon Eigen FCCEDM
                 LINK_LIBRARIES GaudiKernel ROOT DD4hep ${DD4hep_COMPONENT_LIBRARIES} DetSegmentation DetCommon  FWCore FCCEDM)
target_link_libraries(DetComponents ${Geant4_LIBRARIES})
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
               ENVIRONMENT PYTHONPATH+=${PODIO_PYTHON_DIR}
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND python Detector/DetComponents/tests/scripts/check_inclinedEcal_positions.py
               DEPENDS positionsInclinedEcal)
gaudi_add_test(positionsTracker
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/positions_tracker.py)
gaudi_add_test(positionsPositiveCaloEndcap
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/positions_endcap.py)
gaudi_add_test(positionsPositiveCaloEndcapCheck
               ENVIRONMENT PYTHONPATH+=${PODIO_PYTHON_DIR}
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND python Detector/DetComponents/tests/scripts/check_endcapEcal_sublayer_positions.py
               DEPENDS positionsPositiveCaloEndcap)
gaudi_add_test(positionsNegativeCaloEndcap
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/positions_negativeEndcap.py)
gaudi_add_test(positionsNegativeCaloEndcapCheck
               ENVIRONMENT PYTHONPATH+=${PODIO_PYTHON_DIR}
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND python Detector/DetComponents/tests/scripts/check_negativeEndcapEcal_sublayer_positions.py
               DEPENDS positionsNegativeCaloEndcap)
gaudi_add_test(RewriteBitfield
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/rewriteBitfield.py)
