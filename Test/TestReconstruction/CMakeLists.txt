################################################################################
# Package: TestReconstruction
################################################################################
gaudi_subdir(TestReconstruction v1r0)

gaudi_depends_on_subdirs(GaudiAlg GaudiKernel FWCore Detector/DetInterface Detector/DetCommon Detector/DetSegmentation)
find_package(Geant4)
find_package(ACTS COMPONENTS Core)
find_package(DD4hep)

include(${Geant4_USE_FILE})

gaudi_add_module(TestTestPlugins
                 src/*.cpp

                 INCLUDE_DIRS FWCore DetInterface TrackingUtils Geant4 DetCommon DetSegmentation GaudiKernel
                 LINK_LIBRARIES GaudiAlgLib FWCore Geant4 TrackingUtils DetCommon DetSegmentation)


include(CTest)
gaudi_add_test(LookForNeighbours
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestReconstruction/tests/
               FRAMEWORK tests/options/testneighbours.py)

gaudi_add_test(CountCellsXYZ
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/testcellcountingXYZ.py)

gaudi_add_test(CountCellsRPhi
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/testcellcountingRPhi.py)

gaudi_add_test(CountCellsPhiEta
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/testcellcountingPhiEta.py)

gaudi_add_test(GeantFullSimWithGeantino
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/geant_fullsim_pgun_geantino.py)

gaudi_add_test(GeantFullSimWithGeantinoFCCeh
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/geant_fullsim_pgun_geantino_eh.py)
