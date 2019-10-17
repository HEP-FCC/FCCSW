################################################################################
# Package: DetFCCehTrackerTkLayout
################################################################################
gaudi_subdir(DetFCCehTrackerTkLayout v1r0)

gaudi_depends_on_subdirs(GaudiKernel Detector/DetCommon)



install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/Detector/DetFCCheTrackerTkLayout)
