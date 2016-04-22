################################################################################
# Package: DetSensitive
################################################################################
gaudi_subdir(DetSensitive v1r0)

gaudi_depends_on_subdirs(GaudiKernel)

find_package(DD4hep COMPONENTS DDG4 REQUIRED)
find_package(Geant4)
include(${Geant4_USE_FILE})
include(DD4hep)

find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

gaudi_install_headers(DetSensitive)

gaudi_add_library(DetSensitive
		  src/*.cpp
		  INCLUDE_DIRS Geant4 DD4hep ROOT
		  LINK_LIBRARIES GaudiKernel Geant4 DD4hep ROOT ${DD4hep_COMPONENT_LIBRARIES}
        PUBLIC_HEADERS SimG4Common)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
dd4hep_generate_rootmap(DetSensitive)
