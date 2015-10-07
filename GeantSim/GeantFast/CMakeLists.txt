################################################################################
# Package: GeantFast
################################################################################
gaudi_subdir(GeantFast v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects GeantSim/GeantComponents)

find_package(Geant4)
find_package(HepMC)
find_package(CLHEP)
find_package(ROOT)
find_package(FCCEDM)
find_package(ALBERS)

gaudi_install_headers(GeantFast)

gaudi_add_library(GeantFast
                 src/*.cpp
                 INCLUDE_DIRS GeantFast  FWCore GeantComponents Geant4 HepMC CLHEP ROOT FCCEDM ALBERS
                 LINK_LIBRARIES GaudiAlgLib DataObjects Geant4 HepMC CLHEP ROOT FCCEDM ALBERS
                 PUBLIC_HEADERS GeantFast)
