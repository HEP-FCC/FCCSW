################################################################################
# Package: TestGeometry
################################################################################
gaudi_subdir(TestGeometry v1r0)

gaudi_depends_on_subdirs(GaudiKernel FWCore Sim/SimG4Interface Sim/SimG4Common)

find_package(Geant4)
include(${Geant4_USE_FILE})
find_package(ROOT)

find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

gaudi_install_headers(TestGeometry)

gaudi_add_library(TestGeometry
		  src/lib/*.cpp
		  INCLUDE_DIRS Geant4 ROOT TestGeometry
		  LINK_LIBRARIES GaudiKernel Geant4 ROOT
        PUBLIC_HEADERS TestGeometry)

gaudi_add_module(TestGeometryPlugins
                 src/components/*.cpp
                 INCLUDE_DIRS Geant4 FWCore SimG4Interface SimG4Common TestGeometry
                 LINK_LIBRARIES GaudiKernel FWCore Geant4 TestGeometry)

include(CTest)
gaudi_add_test(TestCellIDFromGdml
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               FRAMEWORK tests/options/geant_testcellid_gdml.py)
gaudi_add_test(TestCellIDFromDd4hepSegmentation
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               FRAMEWORK tests/options/geant_testcellid_dd4hep_segmentation.py)
gaudi_add_test(TestCellIDFromDd4hepVolumes
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               FRAMEWORK tests/options/geant_testcellid_dd4hep_volumes.py)
gaudi_add_test(CmpCellIDGdmlDd4hep
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               COMMAND python ./scripts/cmpCellId.py
               ENVIRONMENT PYTHONPATH+=$ENV{PODIO}/python
               DEPENDS TestCellIDFromDd4hepSegmentation TestCellIDFromGdml)
gaudi_add_test(CmpCellIDDd4hepSegmentationVolumes
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               COMMAND python ./scripts/cmpCellIdDd4hep.py
               ENVIRONMENT PYTHONPATH+=$ENV{PODIO}/python
               DEPENDS TestCellIDFromDd4hepSegmentation TestCellIDFromDd4hepVolumes)
