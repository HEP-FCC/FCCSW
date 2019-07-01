################################################################################
# Package: DetFCCeeHCalTile
################################################################################
gaudi_subdir(DetFCCeeHCalTile v1r0)

gaudi_depends_on_subdirs(GaudiKernel)





install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION Detector/DetFCCeeHCalTile)


