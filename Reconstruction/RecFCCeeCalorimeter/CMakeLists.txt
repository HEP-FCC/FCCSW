################################################################################
# Package: RecFCCeeCalorimeter
################################################################################
gaudi_subdir(RecFCCeeCalorimeter v1r0)

gaudi_depends_on_subdirs(GaudiAlg FCCFWCore Detector/DetInterface Detector/DetSegmentation Detector/DetCommon Reconstruction/RecInterface Reconstruction/RecCalorimeter)



install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/options DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/RecFCCeeCalorimeter)
