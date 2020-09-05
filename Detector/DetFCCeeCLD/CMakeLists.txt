###############################################################################
# Package: DetFCCeeCLD
################################################################################
gaudi_subdir(DetFCCeeCLD v1r0)

gaudi_depends_on_subdirs(Detector/DetSegmentation)

find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/Detector/DetFCCeeCLD)

gaudi_add_module(DetFCCeeCLD
		  src/*.cpp
		  INCLUDE_DIRS DD4hep ROOT Geant4 DetSegmentation
		  LINK_LIBRARIES GaudiKernel DD4hep ROOT DetSegmentation)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(DetFCCeeCLD)
