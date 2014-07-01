################################################################################
# Package: Reconstruction
################################################################################
gaudi_subdir(Reconstruction v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg Generation)

find_package(HepMC)
find_package(FastJet)

gaudi_add_module(Reconstruction
                 src/*.cpp
                 INCLUDE_DIRS HepMC FastJet
                 LINK_LIBRARIES HepMC Fastjet GaudiAlgLib)

