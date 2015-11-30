################################################################################
# Package: SimG4Fast
################################################################################
gaudi_subdir(SimG4Fast v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects Sim/SimG4Components Sim/SimG4Interface GeantSim/GeantGeneral)

find_package(Geant4)
include(${Geant4_USE_FILE})
find_package(ROOT)

gaudi_install_headers(SimG4Fast)

gaudi_add_library(SimG4Fast
                 src/lib/*.cpp
                 INCLUDE_DIRS GeantGeneral FWCore SimG4Components SimG4Interface Geant4 ROOT
                 LINK_LIBRARIES GaudiAlgLib DataObjects GeantGeneral Geant4 ROOT
                 PUBLIC_HEADERS SimG4Fast)

gaudi_add_module(SimG4FastPlugins
                 src/components/*.cpp
                 INCLUDE_DIRS Geant4 GeantGeneral SimG4Fast SimG4Interface
                 LINK_LIBRARIES GaudiAlgLib Geant4 SimG4Fast GeantGeneral)
