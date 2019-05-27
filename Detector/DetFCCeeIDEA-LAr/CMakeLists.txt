###############################################################################
# Package: DetFCCeeIDEA-LAr
################################################################################
gaudi_subdir(DetFCCeeIDEA-LAr v1r0)

gaudi_depends_on_subdirs(GaudiKernel Detector/DetSegmentation)



install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION Detector/DetFCCeeIDEA)


set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
