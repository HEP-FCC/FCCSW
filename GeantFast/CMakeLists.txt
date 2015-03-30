################################################################################
# Package: GeantFast
################################################################################
gaudi_subdir(GeantFast v1r0)

# this declaration will not be needed in the future
#gaudi_depends_on_subdirs()

find_package(Geant4)
find_package(HepMC)
find_package(CLHEP)
find_package(ROOT)
find_package(Pythia8)

gaudi_add_library(GeantFast
                 src/*.cc
                 INCLUDE_DIRS GeantFast Geant4 HepMC CLHEP ROOT Pythia8
                 LINK_LIBRARIES  Geant4 HepMC CLHEP ROOT Pythia8
		           PUBLIC_HEADERS GeantFast)

