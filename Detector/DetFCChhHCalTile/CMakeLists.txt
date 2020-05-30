################################################################################
# Package: DetFCChhHCalTile
################################################################################
gaudi_subdir(DetFCChhHCalTile v1r0)

gaudi_depends_on_subdirs(GaudiKernel)


find_package(DD4hep)
find_package(Geant4)


find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/Detector/DetFCChhHCalTile)


gaudi_add_module(DetFCChhHCalTile
		  src/*.cpp
		  INCLUDE_DIRS DD4hep ROOT Geant4
		  LINK_LIBRARIES GaudiKernel DD4hep ROOT)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(DetFCChhHCalTile)

