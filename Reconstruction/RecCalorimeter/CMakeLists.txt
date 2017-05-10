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
               FRAMEWORK tests/options/genJetClustering.py)

gaudi_add_test(simulateECalForReconstruction
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/geant_fullsim_ecal_singleparticles.py)

gaudi_add_test(EcalReconstructionFlatNoise
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               DEPENDS simulateECalForReconstruction
               FRAMEWORK tests/options/runEcalReconstructionFlatNoise.py)

gaudi_add_test(EcalReconstructionWithoutNoise
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               DEPENDS simulateECalForReconstruction
               FRAMEWORK tests/options/runEcalReconstructionWithoutNoise.py)

gaudi_add_test(EcalReconstructionCheckNumClusters
               ENVIRONMENT PYTHONPATH+=$ENV{PODIO}/python
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND python Reconstruction/RecCalorimeter/tests/scripts/ecalRecoCheckNumClusters.py
               DEPENDS EcalReconstructionFlatNoise EcalReconstructionWithoutNoise)

gaudi_add_test(EcalReconstructionNoiseFromFile
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               DEPENDS simulateECalForReconstruction
               FRAMEWORK tests/options/runEcalReconstruction.py)

gaudi_add_test(EcalInclinedDigitisation
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/runEcalInclinedDigitisation.py)

gaudi_add_test(simulateHCal
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/geant_fullsim_hcal_singleparticles.py)

gaudi_add_test(HcalDigitisationWithoutNoise
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               DEPENDS simulateHCal
               FRAMEWORK tests/options/runHcalDigitisationWithoutNoise.py)

gaudi_add_test(HcalDigitisationFlatNoise
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               DEPENDS simulateHCal
               FRAMEWORK tests/options/runHcalDigitisationFlatNoise.py)
