if(NOT BUILD_TESTING)
  return()
endif()
################################################################################
# Package: TestReconstruction
################################################################################
gaudi_subdir(TestReconstruction v1r0)

gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface Detector/DetCommon Detector/DetSegmentation)


find_package(ROOT COMPONENTS Geom)



gaudi_add_module(TestReconstructionPlugins
                 src/*.cpp
                 INCLUDE_DIRS FWCore DetInterface TrackingUtils Geant4 DetCommon DetSegmentation GaudiKernel FCCEDM
                 LINK_LIBRARIES FWCore GaudiAlgLib FWCore TrackingUtils DetCommon DetSegmentation FCCEDM)
target_link_libraries(TestReconstructionPlugins ${Geant4_LIBRARIES})

include(CTest)
gaudi_add_test(LookForNeighbours
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
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
