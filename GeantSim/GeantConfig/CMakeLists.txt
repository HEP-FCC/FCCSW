################################################################################
# Package: GeantConfig
################################################################################
gaudi_subdir(GeantConfig v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg GeantSim/GeantGeneral GeantSim/GeantFast)

find_package(Geant4)

gaudi_install_headers(GeantConfig)

gaudi_add_module(GeantConfig
                 src/*.cpp
                 INCLUDE_DIRS Geant4 GeantGeneral GeantFast
                 LINK_LIBRARIES GaudiAlgLib Geant4 GeantFast GeantGeneral)
