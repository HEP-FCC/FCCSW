################################################################################
# Package: TestGeometry
################################################################################
gaudi_subdir(TestGeometry v1r0)

gaudi_depends_on_subdirs(GaudiKernel FWCore Sim/SimG4Interface Sim/SimG4Common Detector/DetInterface Detector/DetCommon)

find_package(Geant4)
include(${Geant4_USE_FILE})
find_package(DD4hep)
find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

gaudi_install_headers(TestGeometry)

gaudi_add_library(TestGeometry
		  src/lib/*.cpp
		  INCLUDE_DIRS Geant4 ROOT TestGeometry DD4hep
		  LINK_LIBRARIES GaudiKernel Geant4 ROOT DD4hep
        PUBLIC_HEADERS TestGeometry)

gaudi_add_module(TestGeometryPlugins
                 src/components/*.cpp
                 INCLUDE_DIRS Geant4 FWCore SimG4Interface SimG4Common DetInterface DetCommon TestGeometry
                 LINK_LIBRARIES GaudiKernel FWCore Geant4 DetCommon TestGeometry)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
dd4hep_generate_rootmap(TestGeometry)

include(CTest)
gaudi_add_test(GeantOverlapTestFCChhMaster
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/geant_overlap_test_fcchh.py
               FAILREGEX "Overlap is detected")
gaudi_add_test(TestBorderCellIDFromGdml
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               FRAMEWORK tests/options/geant_testcellid_gdml.py)
gaudi_add_test(TestBorderCellIDFromDd4hep
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               FRAMEWORK tests/options/geant_testcellid_dd4hep.py)
gaudi_add_test(TestPrimitiveShapes
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               FRAMEWORK tests/options/test_primitives.py)
gaudi_add_test(CmpBorderCellIDGdmlwithDd4hep
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               COMMAND python ./scripts/cmpCellId.py
               ENVIRONMENT PYTHONPATH+=$ENV{PODIO}/python
               DEPENDS TestBorderCellIDFromDd4hep TestBorderCellIDFromGdml)
gaudi_add_test(TestRandomCellIDFromDd4hepSegmentation
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               FRAMEWORK tests/options/geant_testcellid_dd4hep_segmentation.py)
gaudi_add_test(TestRandomCellIDFromDd4hepVolumes
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               FRAMEWORK tests/options/geant_testcellid_dd4hep_volumes.py)
gaudi_add_test(CmpRandomCellIDDd4hepSegmentationwithVolumes
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               COMMAND python ./scripts/cmpCellIdDd4hep.py
               ENVIRONMENT PYTHONPATH+=$ENV{PODIO}/python
               DEPENDS TestRandomCellIDFromDd4hepSegmentation TestRandomCellIDFromDd4hepVolumes)
gaudi_add_test(TestGFlashHomoCalo
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               FRAMEWORK tests/options/gflash_test_pbwo4.py)
# Test the detector baselines:
gaudi_add_test(OverlapCheckExtrusionsMaterialFCChh
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND python ${DD4hep_ROOT}/python/checkOverlaps.py -c Detector/DetFCChhBaseline1/compact/FCChh_DectMasterMaterial.xml
               PASSREGEX "Number of illegal overlaps/extrusions : 0" )
gaudi_add_test(OverlapCheckSamplingMaterialFCChh
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND python ${DD4hep_ROOT}/python/checkOverlaps.py -c Detector/DetFCChhBaseline1/compact/FCChh_DectMasterMaterial.xml -o s
               PASSREGEX "Number of illegal overlaps/extrusions : 0" )
