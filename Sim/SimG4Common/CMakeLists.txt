################################################################################
# Package: SimG4Common
################################################################################
gaudi_subdir(SimG4Common v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg)

find_package(Geant4)
find_package(ROOT)
find_package(FCCEDM)
find_package(PODIO)

gaudi_add_library(SimG4Common
                 src/*.cpp
                 INCLUDE_DIRS FWCore Geant4 ROOT FCCEDM PODIO
                 LINK_LIBRARIES GaudiAlgLib ROOT  FCCEDM PODIO
                 PUBLIC_HEADERS SimG4Common)
target_link_libraries(SimG4Common ${GEANT4_LIBRARIES})
