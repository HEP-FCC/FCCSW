################################################################################
# Package: RecTracker
################################################################################
gaudi_subdir(RecTracker v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface Sim/SimG4Interface Reconstruction/RecInterface)

find_package(Geant4)
find_package(FCCEDM)
find_package(PODIO)
find_package(HepMC)
find_package(DD4hep COMPONENTS DDSegmentation REQUIRED)
find_package(tricktrack)
find_package(ROOT REQUIRED COMPONENTS Geom)
find_package(ACTS COMPONENTS Core  TGeoPlugin DD4hepPlugin)



gaudi_install_headers(RecTracker)
gaudi_install_python_modules()
gaudi_add_library(TrackingUtils
                 src/lib/*.cpp
                 INCLUDE_DIRS DD4hep HepMC  FWCore  FCCEDM PODIO  DetInterface RecInterface SimG4Interface ROOT
                 LINK_LIBRARIES GaudiAlgLib FCCEDM PODIO HepMC DD4hep ROOT 
                 PUBLIC_HEADERS RecTracker) 

gaudi_add_module(RecTracker
                 src/components/*.cpp
                 INCLUDE_DIRS ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface RecInterface TrackingUtils ACTS ${tricktrack_INCLUDE_DIR}
                 LINK_LIBRARIES ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep TrackingUtils ACTS::ACTSCore ACTS::ACTSDD4hepPlugin)


gaudi_add_library(ActsLogger
                 src/lib/ACTSLogger.cpp
                 INCLUDE_DIRS ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface RecInterface ACTS
                 NO_PUBLIC_HEADERS
                 LINK_LIBRARIES ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep ACTS::ACTSCore ACTS::ACTSDD4hepPlugin) 

include(CTest)

gaudi_add_test(FastDigiTest
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/fastDigiTest.py)


gaudi_add_test(MuonsForSeeding
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND ./run fccrun.py  Reconstruction/RecTracker/options/geantSim_TrackerPerformance.py -N 100 -s 0123456 --outName muons_for_seeding.root --singlePart --particle 13)

gaudi_add_test(CombinatorialTrackSeeding
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/CombinatorialSeedingTest.py
			         DEPENDS MuonsForSeeding)

gaudi_add_test(TrickTrackSeeding
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/TrickTrackReco.py
			         DEPENDS MuonsForSeeding)


# TODO: to be re-enabled once ACTS DD4hepPlugin is up to date
#gaudi_add_test(ExtrapolationTest
#               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#			         FRAMEWORK options/extrapolationTest.py
#               ENVIRONMENT LD_PRELOAD+=build.$BINARY_TAG/lib/libActsLogger.so)
