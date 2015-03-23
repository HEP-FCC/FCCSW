################################################################################
# Package: GeantFast
################################################################################
gaudi_subdir(GeantFast v1r0)

# this declaration will not be needed in the future
#gaudi_depends_on_subdirs()

find_package(Geant4)
find_package(HepMC)

gaudi_add_library(GeantFast
                 src/*.cc
                 INCLUDE_DIRS Geant4 HepMC GeantFast
                 LINK_LIBRARIES  Geant4 HepMC
		           PUBLIC_HEADERS GeantFast)

