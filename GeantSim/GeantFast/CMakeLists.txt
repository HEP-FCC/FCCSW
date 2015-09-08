################################################################################
# Package: GeantFast
################################################################################
gaudi_subdir(GeantFast v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg GeantSim/GeantComponents)

find_package(Geant4)
find_package(HepMC)
find_package(CLHEP)
find_package(ROOT)

gaudi_install_headers(GeantFast)

gaudi_add_library(GeantFast
                 src/*.cpp
                 INCLUDE_DIRS GeantFast GeantComponents Geant4 HepMC CLHEP ROOT
                 LINK_LIBRARIES GaudiAlgLib Geant4 HepMC CLHEP ROOT
                 PUBLIC_HEADERS GeantFast)
