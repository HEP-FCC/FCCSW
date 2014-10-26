################################################################################
# Package: FWCore
################################################################################
gaudi_subdir(FWCore v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg GaudiKernel albers)

gaudi_install_headers(FWoore)

gaudi_add_module(Core2
                 src/*.cpp
                 LINK_LIBRARIES GaudiAlgLib GaudiKernel albers)
