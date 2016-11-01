################################################################################
# Package: RecCalorimeter
################################################################################
gaudi_subdir(RecCalorimeter v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface Detector/DetSegmentation Detector/DetCommon Reconstruction/RecInterface)

find_package(FastJet)
find_package(ROOT COMPONENTS Physics)
find_package(FCCEDM)
find_package(PODIO)
find_package(HepMC)
find_package(DD4hep COMPONENTS DDSegmentation)
find_package(Geant4)

gaudi_add_module(RecCalorimeterPlugins
                 src/components/*.cpp
                 INCLUDE_DIRS FastJet ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface DetSegmentation Geant4 DetCommon RecInterface RecCalorimeter
                 LINK_LIBRARIES Fastjet ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep DetSegmentation Geant4 DetCommon ${DD4hep_COMPONENT_LIBRARIES})

include(CTest)
gaudi_add_test(genJetClustering
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/genJetClustering.py)

gaudi_add_test(simulateECalForReconstruction
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               ENVIRONMENT LD_PRELOAD+=build.$BINARY_TAG/lib/libDetSegmentation.so
               FRAMEWORK options/geant_fullsim_ecal_singleparticles.py)

gaudi_add_test(runEcalReconstruction
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               ENVIRONMENT LD_PRELOAD+=build.$BINARY_TAG/lib/libDetSegmentation.so
               DEPENDS simulateECalForReconstruction
               FRAMEWORK options/runEcalReconstruction.py)
