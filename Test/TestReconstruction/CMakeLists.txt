################################################################################
# Package: TestReconstruction
################################################################################
gaudi_subdir(TestReconstruction v1r0)

gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface Detector/DetCommon Detector/DetSegmentation)
find_package(Geant4)
include(${Geant4_USE_FILE})

gaudi_add_module(TestTestPlugins
                 src/*.cpp
                 INCLUDE_DIRS FWCore DetInterface Geant4 DetCommon DetSegmentation
                 LINK_LIBRARIES GaudiAlgLib FWCore Geant4 DetCommon DetSegmentation)

include(CTest)
gaudi_add_test(LookForNeighbours
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestGeometry/tests/
               FRAMEWORK tests/options/testneighbours.py)
