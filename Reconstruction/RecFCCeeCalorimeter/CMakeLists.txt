################################################################################
# Package: RecFCCeeCalorimeter
################################################################################
gaudi_subdir(RecFCCeeCalorimeter v1r0)

gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface Detector/DetSegmentation Detector/DetCommon Reconstruction/RecInterface Reconstruction/RecCalorimeter)

find_package(FastJet)
find_package(ROOT COMPONENTS Physics Tree)
find_package(FCCEDM)
find_package(PODIO)
find_package(HepMC)

gaudi_add_module(RecFCCeeCalorimeterPlugins
                 src/components/*.cpp
                 INCLUDE_DIRS FWCore FastJet ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface DetSegmentation Geant4 DetCommon RecInterface RecCalorimeter RecFCCeeCalorimeter
                 LINK_LIBRARIES FWCore Fastjet ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep DetSegmentation DetCommon)
target_link_libraries(RecFCCeeCalorimeterPlugins ${Geant4_LIBRARIES})

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/options DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/RecFCCeeCalorimeter)
