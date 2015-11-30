################################################################################
# Package: GeantGeneral
################################################################################
gaudi_subdir(GeantGeneral v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects GeantSim/GeantComponents)

find_package(Geant4)
find_package(ROOT)
find_package(FCCEDM)
find_package(ALBERS)

gaudi_install_headers(GeantGeneral)

gaudi_add_library(GeantGeneral
                 src/*.cpp
                 INCLUDE_DIRS GeantGeneral FWCore Geant4 ROOT FCCEDM ALBERS
                 LINK_LIBRARIES GaudiAlgLib DataObjects Geant4 ROOT FCCEDM ALBERS
                 PUBLIC_HEADERS GeantGeneral)
