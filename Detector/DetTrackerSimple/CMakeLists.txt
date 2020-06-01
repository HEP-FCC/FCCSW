################################################################################
# Package: DetTrackerSimple
################################################################################
gaudi_subdir(DetTrackerSimple v1r0)

gaudi_depends_on_subdirs(GaudiKernel Detector/DetCommon)





find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION Detector/DetTrackerSimple)

gaudi_add_module(DetTrackerSimple
                 src/*.cpp
                 INCLUDE_DIRS DD4hep ROOT Geant4 DetCommon
                 LINK_LIBRARIES GaudiKernel DD4hep ROOT DetCommon)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
dd4hep_generate_rootmap(DetTrackerSimple)

