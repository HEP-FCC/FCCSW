################################################################################
# Package: Reconstruction
################################################################################
gaudi_subdir(Reconstruction v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects)

find_package(FastJet)

gaudi_add_module(Reconstruction
                 src/*.cpp
                 INCLUDE_DIRS FastJet
                 LINK_LIBRARIES Fastjet GaudiAlgLib DataObjects)

