################################################################################
# Package: Generation
################################################################################
gaudi_subdir(Generation v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects)

find_package(HepMC)
find_package(Geant4)
find_package(Pythia8 COMPONENTS pythia8 pythia8tohepmc)

gaudi_install_headers(Generation)

gaudi_add_module(Generation
                 src/*.cpp
                 INCLUDE_DIRS HepMC Pythia8 Geant4
                 LINK_LIBRARIES HepMC GaudiAlgLib DataObjects Pythia8 Geant4)
