################################################################################
# Package: Visualization
################################################################################
gaudi_subdir(Visualization v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore)

find_package(ROOT COMPONENTS Geom MathCore RIO Eve GenVector Gui REQUIRED)


set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake )
find_package(DD4hep COMPONENTS DDCore DDG4 DDEve REQUIRED)

include(DD4hep)

find_package(FCCEDM)
find_package(PODIO)


gaudi_add_library(Visualization
                 NO_PUBLIC_HEADERS
                 src/*.cpp
                 INCLUDE_DIRS DD4hep ROOT Geant4 PODIO FCCEDM ROOT
                 LINK_LIBRARIES GaudiKernel DD4hep ROOT PODIO FCCEDM ${DD4hep_DDEVE_LIBRARY}
                 )
set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(Visualization)



