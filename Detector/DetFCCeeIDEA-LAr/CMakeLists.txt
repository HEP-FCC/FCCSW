###############################################################################
# Package: DetFCCeeIDEA-LAr
################################################################################
gaudi_subdir(DetFCCeeIDEA-LAr v1r0)

gaudi_depends_on_subdirs(GaudiKernel Detector/DetSegmentation)



install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/Detector/DetFCCeeIDEA-LAr)


