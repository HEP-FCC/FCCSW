################################################################################
# Package: RecTracker
################################################################################
gaudi_subdir(RecTracker v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface Sim/SimG4Interface Reconstruction/RecInterface)

find_package(FCCEDM)
find_package(PODIO)
find_package(DD4hep)
find_package(ROOT REQUIRED COMPONENTS Geom)
find_package(Acts COMPONENTS Core IdentificationPlugin TGeoPlugin DD4hepPlugin Legacy)


gaudi_install_headers(RecTracker)
gaudi_install_python_modules()
gaudi_add_library(TrackingUtils
                 src/lib/*.cpp
                 INCLUDE_DIRS DD4hep HepMC  FWCore  FCCEDM PODIO  DetInterface RecInterface SimG4Interface ROOT
                 LINK_LIBRARIES GaudiAlgLib FCCEDM PODIO DD4hep ROOT 
                 PUBLIC_HEADERS RecTracker tricktrack) 

gaudi_add_module(RecTracker
                 src/components/*.cpp
                 INCLUDE_DIRS DD4hep Geant4 FWCore SimG4Common SimG4Interface  ROOT RecInterface TrackingUtils Acts
                 LINK_LIBRARIES ROOT GaudiAlgLib FCCEDM PODIO DD4hep TrackingUtils ActsCore ActsDD4hepPlugin ActsLegacy)


gaudi_add_library(ActsLogger
                 src/lib/ACTSLogger.cpp
                 INCLUDE_DIRS ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface RecInterface Acts
                 NO_PUBLIC_HEADERS
                 LINK_LIBRARIES ROOT GaudiAlgLib FCCEDM PODIO DD4hep ActsCore)

include(CTest)

gaudi_add_test(FastDigiTest
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/fastDigiTest.py)


gaudi_add_test(MuonsForSeeding
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND ./run fccrun.py  Reconstruction/RecTracker/options/geantSim_TrackerPerformance.py -N 10 -s 0123456 --outName muons_for_seeding.root --singlePart --particle 13)

gaudi_add_test(CombinatorialTrackSeeding
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/CombinatorialSeedingTest.py
			         DEPENDS MuonsForSeeding)

gaudi_add_test(TrickTrackSeeding
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/TrickTrackReco.py
			         DEPENDS MuonsForSeeding)


gaudi_add_test(ExtrapolationTest
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			         FRAMEWORK options/extrapolationTest.py)

