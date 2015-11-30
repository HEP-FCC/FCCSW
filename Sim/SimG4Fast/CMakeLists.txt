################################################################################
# Package: SimG4Fast
################################################################################
gaudi_subdir(SimG4Fast v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects Sim/SimG4Components GeantSim/GeantGeneral)

find_package(Geant4)
include(${Geant4_USE_FILE})
find_package(ROOT)

gaudi_install_headers(SimG4Fast)

gaudi_add_library(SimG4Fast
                 src/*.cpp
                 INCLUDE_DIRS GeantFast GeantGeneral FWCore SimG4Components Geant4 ROOT
                 LINK_LIBRARIES GaudiAlgLib DataObjects GeantGeneral Geant4 ROOT
                 PUBLIC_HEADERS SimG4Fast)
