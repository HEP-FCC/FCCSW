################################################################################
# Package: RecTracker
################################################################################
gaudi_subdir(RecTracker v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface)

find_package(FCCEDM)
find_package(PODIO)
find_package(HepMC)
find_package(DD4hep)
find_package(ROOT)

gaudi_add_module(RecTracker
                 src/components/*.cpp
                 INCLUDE_DIRS ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface RecInterface
                 LINK_LIBRARIES ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep ${DD4hep_COMPONENT_LIBRARIES})

include(CTest)
#gaudi_add_test(genJetClustering
#               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#               FRAMEWORK options/genJetClustering.py)
#
#gaudi_add_test(runEcalReconstruction
#               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#               ENVIRONMENT LD_PRELOAD+=build.$BINARY_TAG/lib/libDetSegmentation.so	
#               FRAMEWORK options/runEcalReconstruction.py)
