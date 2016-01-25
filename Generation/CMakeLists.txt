################################################################################
# Package: Generation
################################################################################
gaudi_subdir(Generation v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects FWCore)

find_package(HepMC)
find_package(Pythia8 COMPONENTS pythia8 pythia8tohepmc)
find_package(FCCEDM)
find_package(ALBERS)

gaudi_install_headers(Generation)

gaudi_add_module(Generation
                 src/components/*.cpp
                 INCLUDE_DIRS Generation HepMC Pythia8 FWCore FCCEDM ALBERS
                 LINK_LIBRARIES HepMC GaudiAlgLib DataObjects Pythia8 FCCEDM ALBERS)
