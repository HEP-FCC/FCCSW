################################################################################
#Package : DetFCCeeECalInclined
################################################################################
gaudi_subdir(DetFCCeeECalInclined v1r0)

gaudi_depends_on_subdirs(GaudiKernel)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/Detector/DetFCCeeECalInclined)
