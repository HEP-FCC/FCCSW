################################################################################
# Package: RecTracker
################################################################################
gaudi_subdir(RecTracker v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Sim/SimG4Common Sim/SimG4Interface Detector/DetInterface Reconstruction/RecInterface)

find_package(Geant4)
include(${Geant4_USE_FILE})
find_package(DD4hep COMPONENTS DDCore DDG4 DDSegmentation DDRec REQUIRED)
find_package(FCCEDM)
find_package(PODIO)
find_package(ROOT)
find_package(ACTS COMPONENTS Core Examples TGeoPlugin DD4hepPlugin)


gaudi_install_headers(RecTracker)
gaudi_add_library(TrackingUtils
                 src/lib/*.cpp
                 INCLUDE_DIRS DD4hep  FWCore  FCCEDM PODIO  DetInterface RecInterface SimG4Interface ROOT
                 LINK_LIBRARIES ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep ROOT ${DD4hep_COMPONENT_LIBRARIES}
                 PUBLIC_HEADERS RecTracker) 
gaudi_add_module(RecTracker
                 src/components/*.cpp
                 INCLUDE_DIRS Geant4 FWCore SimG4Common SimG4Interface DD4hep ROOT RecInterface TrackingUtils ACTS
                 LINK_LIBRARIES ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep ${DD4hep_COMPONENT_LIBRARIES} TrackingUtils ACTS::ACTSCore ACTS::ACTSDD4hepPlugin)

include(CTest)

gaudi_add_test(FastDigiTest
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/fastDigiTest.py
			         DEPENDS Examples.GeantFullSimTrackerHits)

gaudi_add_test(CombinatorialTrackSeeding
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/combinatorialTrackSeedingTest.py
			         DEPENDS Examples.GeantFullSimTrackerHits)

# TODO: to be re-enabled once ACTS DD4hepPlugin is up to date
#gaudi_add_test(ExtrapolationTest
#               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#			         FRAMEWORK options/extrapolationTest.py)
