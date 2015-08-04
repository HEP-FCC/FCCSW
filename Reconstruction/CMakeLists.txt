################################################################################
# Package: Reconstruction
################################################################################
gaudi_subdir(Reconstruction v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects)

find_package(FastJet)
find_package(ROOT COMPONENTS Physics)

gaudi_add_module(Reconstruction
                 src/*.cpp
                 INCLUDE_DIRS FastJet ROOT FWCore
                 LINK_LIBRARIES Fastjet ROOT GaudiAlgLib DataObjects)

