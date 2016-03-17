################################################################################
# Package: DetSensitive
################################################################################
gaudi_subdir(DetSensitive v1r0)

find_package(Geant4)
include(${Geant4_USE_FILE})
find_package(DD4hep)
find_package(ROOT REQUIRED COMPONENTS Geom)

gaudi_add_library(DetSensitive
		  src/*.cpp
		  INCLUDE_DIRS Geant4 DD4hep ROOT
		  LINK_LIBRARIES Geant4 DD4hep ROOT
        PUBLIC_HEADERS DetSensitive)
