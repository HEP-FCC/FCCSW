################################################################################
# Package: Simulation
################################################################################
gaudi_subdir(Simulation v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects GeantFast)

find_package(Geant4)

gaudi_add_module(Simulation
                 src/*.cpp
                 INCLUDE_DIRS Geant4
                 LINK_LIBRARIES GaudiAlgLib DataObjects Geant4 GeantFast)

