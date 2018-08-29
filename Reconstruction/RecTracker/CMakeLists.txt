################################################################################
# Package: RecTracker
################################################################################
gaudi_subdir(RecTracker v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Sim/SimG4Common Sim/SimG4Interface Detector/DetInterface Reconstruction/RecInterface)

find_package(DD4hep)
find_package(HepMC)
find_package(Geant4)
find_package(FCCEDM)
find_package(PODIO)
find_package(ROOT REQUIRED COMPONENTS Geom Tree MathCore TreePlayer Physics)
find_package(ACTS COMPONENTS Core  TGeoPlugin DD4hepPlugin)

#set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake )
#include( DD4hep )


gaudi_install_headers(RecTracker)
gaudi_add_library(TrackingUtils
                 src/lib/*.cpp
                 INCLUDE_DIRS DD4hep HepMC  FWCore  FCCEDM PODIO  DetInterface RecInterface SimG4Interface ROOT
                 LINK_LIBRARIES GaudiAlgLib FCCEDM PODIO HepMC DD4hep ROOT 
                 PUBLIC_HEADERS RecTracker) 

gaudi_add_module(RecTracker
                 src/components/*.cpp
                 INCLUDE_DIRS DD4hep Geant4 FWCore SimG4Common SimG4Interface  ROOT RecInterface TrackingUtils ACTS
                 LINK_LIBRARIES ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep TrackingUtils ACTS::ACTSCore ACTS::ACTSDD4hepPlugin)


gaudi_add_library(ActsLogger
                 src/lib/ACTSLogger.cpp
                 INCLUDE_DIRS ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface RecInterface ACTS
                 NO_PUBLIC_HEADERS
                 LINK_LIBRARIES ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep ACTS::ACTSCore ACTS::ACTSDD4hepPlugin) 

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
#			         FRAMEWORK options/extrapolationTest.py
#               ENVIRONMENT LD_PRELOAD+=build.$BINARY_TAG/lib/libActsLogger.so)
