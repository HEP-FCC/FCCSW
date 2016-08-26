################################################################################
# Package: Visualization
################################################################################
gaudi_subdir(Visualization v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore)

find_package(ROOT COMPONENTS Geom MathCore RIO Eve GenVector Gui REQUIRED)


find_package(DD4hep COMPONENTS DDCore DDG4 DDEve REQUIRED)


find_package(FCCEDM)
find_package(PODIO)


gaudi_add_library(Visualization   NO_PUBLIC_HEADERS          src/*.cpp
                 INCLUDE_DIRS DD4hep ROOT Geant4 PODIO FCCEDM ROOT 
                 LINK_LIBRARIES GaudiKernel DD4hep ROOT PODIO FCCEDM ${DD4hep_DDEVE_LIBRARY} 
                 )


