################################################################################
# Package: RecFCChhCalorimeter
################################################################################
gaudi_subdir(RecFCChhCalorimeter v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface Detector/DetSegmentation Detector/DetCommon Reconstruction/RecInterface Reconstruction/RecCalorimeter)

find_package(FastJet)
find_package(ROOT COMPONENTS Physics Tree)
find_package(FCCEDM)
find_package(PODIO)
find_package(HepMC)



gaudi_add_module(RecFCChhCalorimeterPlugins
                 src/components/*.cpp
                 INCLUDE_DIRS FWCore FastJet ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface DetSegmentation Geant4 DetCommon RecInterface RecCalorimeter RecFCChhCalorimeter
                 LINK_LIBRARIES FWCore Fastjet ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep DetSegmentation DetCommon)
target_link_libraries(RecFCChhCalorimeterPlugins ${Geant4_LIBRARIES})

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/tests DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/RecFCChhCalorimeter)

include(CTest)

gaudi_add_test(simulateFullCaloSystemForCellPositions
	       WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	       FRAMEWORK ../RecCalorimeter/tests/options/runFullCaloSystem_SimAndDigitisation.py)

gaudi_add_test(reconstructFullCaloSystemCellPositions
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	       DEPENDS simulateFullCaloSystemForCellPositions
	       FRAMEWORK tests/options/recoPositions_fullCaloSystem.py)

gaudi_add_test(buildingCellNoiseMap
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	       FRAMEWORK tests/options/noiseLevelPerCell.py)

gaudi_add_test(buildingCellNeighboursMap
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	       FRAMEWORK tests/options/neighbours.py)
