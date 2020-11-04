################################################################################
# Package: RecTracker
################################################################################
gaudi_subdir(RecTracker v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface Sim/SimG4Interface Reconstruction/RecInterface)

find_package(CLHEP)
find_package(FCCEDM)
find_package(PODIO)
find_package(Eigen)

find_package(ROOT REQUIRED COMPONENTS Geom)

gaudi_install_python_modules()
gaudi_add_library(TrackingUtils
                 src/lib/*.cpp
                 INCLUDE_DIRS DD4hep HepMC  FWCore  FCCEDM PODIO  DetInterface RecInterface SimG4Interface ROOT Eigen
                 LINK_LIBRARIES GaudiAlgLib FCCEDM PODIO DD4hep ROOT 
                 PUBLIC_HEADERS RecTracker tricktrack) 

gaudi_add_module(RecTracker
                 src/components/*.cpp
                 INCLUDE_DIRS DD4hep Geant4 FWCore SimG4Common SimG4Interface  ROOT RecInterface TrackingUtils CLHEP Eigen
                 LINK_LIBRARIES ROOT GaudiAlgLib FCCEDM PODIO DD4hep TrackingUtils CLHEP)



install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/options DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/Reconstruction/RecTracker)

include(CTest)


gaudi_add_test(FastDigiTest
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/fastDigiTest.py)

gaudi_add_test(MuonsForSeeding
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK  options/geantSim_TrackerPerformance.py)

gaudi_add_test(TrickTrackSeeding
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/TrickTrackReco.py
			         DEPENDS MuonsForSeeding)

