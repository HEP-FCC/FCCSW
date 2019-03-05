################################################################################
#Package : DetFCCeeECalInclined
################################################################################
gaudi_subdir(DetFCCeeECalInclined v1r0)

gaudi_depends_on_subdirs(GaudiKernel)



install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION Detector/DetFCCeeECalInclined)

#dd4hep_generate_rootmap(DetFCCeeECalInclined)
