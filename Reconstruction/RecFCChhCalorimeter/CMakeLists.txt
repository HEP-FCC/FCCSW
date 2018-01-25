################################################################################
# Package: RecFCChhCalorimeter
################################################################################
gaudi_subdir(RecFCChhCalorimeter v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface Detector/DetSegmentation Detector/DetCommon Reconstruction/RecInterface Reconstruction/RecCalorimeter)

find_package(FastJet)
find_package(ROOT COMPONENTS Physics)
find_package(FCCEDM)
find_package(PODIO)
find_package(HepMC)
find_package(DD4hep COMPONENTS DDSegmentation)
find_package(Geant4)

gaudi_add_module(RecFCChhCalorimeterPlugins
                 src/components/*.cpp
                 INCLUDE_DIRS FastJet ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface DetSegmentation Geant4 DetCommon RecInterface RecCalorimeter RecFCChhCalorimeter
                 LINK_LIBRARIES Fastjet ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep DetSegmentation Geant4 DetCommon ${DD4hep_COMPONENT_LIBRARIES})

include(CTest)

gaudi_add_test(simulateFullCaloSystemForCellPositions
	       WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	       FRAMEWORK ../RecCalorimeter/tests/options/runFullCaloSystem_SimAndDigitisation.py)

gaudi_add_test(reconstructFullCaloSystemCellPositions
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	       DEPENDS simulateFullCaloSystemForCellPositions
	       FRAMEWORK tests/options/recoPositions_fullCaloSystem.py)
