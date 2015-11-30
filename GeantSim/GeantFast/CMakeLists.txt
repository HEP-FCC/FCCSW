################################################################################
# Package: GeantFast
################################################################################
gaudi_subdir(GeantFast v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects GeantSim/GeantComponents GeantSim/GeantGeneral)

find_package(Geant4)
find_package(ROOT)

gaudi_install_headers(GeantFast)

gaudi_add_library(GeantFast
                 src/*.cpp
                 INCLUDE_DIRS GeantFast GeantGeneral FWCore GeantComponents Geant4 ROOT
                 LINK_LIBRARIES GaudiAlgLib DataObjects GeantGeneral Geant4 ROOT
                 PUBLIC_HEADERS GeantFast)
