################################################################################
# Package: DetFCCheTrackerTkLayout
################################################################################
gaudi_subdir(DetFCCheTrackerTkLayout v1r0)

gaudi_depends_on_subdirs(GaudiKernel Detector/DetCommon)



install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION Detector/DetFCCheTrackerTkLayout)
