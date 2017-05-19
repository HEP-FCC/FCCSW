################################################################################
# Package: RecTracker
################################################################################
gaudi_subdir(RecTracker v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface Reconstruction/RecInterface)

find_package(FCCEDM)
find_package(PODIO)
find_package(HepMC)
find_package(DD4hep COMPONENTS DDG4 DDSegmentation DDRec REQUIRED)
find_package(ROOT)
find_package(ACTS COMPONENTS Core Examples TGeoPlugin DD4hepPlugin)


gaudi_install_headers(RecTracker)
gaudi_add_library(TrackingUtils
                 src/lib/*.cpp
                 INCLUDE_DIRS ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface RecInterface
                 LINK_LIBRARIES ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep ${DD4hep_COMPONENT_LIBRARIES} 
                 PUBLIC_HEADERS RecTracker) 
gaudi_add_module(RecTracker
                 src/components/*.cpp
                 INCLUDE_DIRS ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface RecInterface TrackingUtils ACTS
                 LINK_LIBRARIES ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep TrackingUtils ${DD4hep_COMPONENT_LIBRARIES} ACTS::ACTSCore ACTS::ACTSDD4hepPlugin)

include(CTest)

gaudi_add_test(FastDigiTest
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/fastDigiTest.py
			         DEPENDS Examples.GeantFullSimTrackerHits)

gaudi_add_test(CombinatorialTrackSeeding
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/combinatorialTrackSeedingTest.py
			         DEPENDS Examples.GeantFullSimTrackerHits)

gaudi_add_test(ExtrapolationTest
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/extrapolationTest.py)
