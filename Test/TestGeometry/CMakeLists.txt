################################################################################
# Package: TestGeometry
################################################################################
gaudi_subdir(TestGeometry v1r0)

gaudi_depends_on_subdirs(FWCore Test/TestGeometryLib Sim/SimG4Interface Sim/SimG4Common Detector/DetInterface Detector/DetCommon)


find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)


gaudi_add_module(TestGeometry
                 src/components/*.cpp
                 INCLUDE_DIRS Geant4 FWCore SimG4Interface SimG4Common DetInterface DetCommon TestGeometryLib FCCEDM
                 LINK_LIBRARIES GaudiKernel FWCore DetCommon TestGeometryLib FCCEDM) 
target_link_libraries(TestGeometry ${Geant4_LIBRARIES})


include(CTest)
gaudi_add_test(GeantOverlapTestFCChhMaster
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/geant_overlap_test_fcchh.py
               FAILREGEX "Overlap is detected")
gaudi_add_test(TestBorderCellIDFromGdml
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/geant_testcellid_gdml.py)
gaudi_add_test(TestBorderCellIDFromDd4hep
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/geant_testcellid_dd4hep.py)
gaudi_add_test(CmpBorderCellIDGdmlwithDd4hep
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND python Test/TestGeometry/tests/scripts/cmpCellId.py
               ENVIRONMENT PYTHONPATH+=${PODIO_PYTHON_DIR}
               DEPENDS TestBorderCellIDFromDd4hep TestBorderCellIDFromGdml)
gaudi_add_test(TestRandomCellIDFromDd4hepSegmentation
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/geant_testcellid_dd4hep_segmentation.py)
gaudi_add_test(TestRandomCellIDFromDd4hepVolumes
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/geant_testcellid_dd4hep_volumes.py)
gaudi_add_test(CmpRandomCellIDDd4hepSegmentationwithVolumes
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND python Test/TestGeometry/tests/scripts/cmpCellIdDd4hep.py
               ENVIRONMENT PYTHONPATH+=${PODIO_PYTHON_DIR}
               DEPENDS TestRandomCellIDFromDd4hepSegmentation TestRandomCellIDFromDd4hepVolumes)
gaudi_add_test(TestGFlashHomoCalo
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/gflash_test_pbwo4.py)
