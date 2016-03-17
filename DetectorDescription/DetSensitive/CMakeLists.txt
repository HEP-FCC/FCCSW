################################################################################
# Package: DetSensitive
################################################################################
gaudi_subdir(DetSensitive v1r0)

gaudi_depends_on_subdirs(GaudiKernel)

find_package(DD4hep)
find_package(Geant4)
include(${Geant4_USE_FILE})
set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake )
include(DD4hep)

find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

gaudi_add_library(DetSensitive
		  src/*.cpp
		  INCLUDE_DIRS Geant4 DD4hep ROOT
		  LINK_LIBRARIES GaudiKernel Geant4 DD4hep ROOT)


set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(DetSensitive)
