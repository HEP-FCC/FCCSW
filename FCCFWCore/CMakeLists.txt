################################################################################
# Package: FCCFWCore
################################################################################
gaudi_subdir(FCCFWCore v1r0)

find_package(podio)
find_package(FCCEDM)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg GaudiKernel FWCore)
gaudi_install_scripts()

gaudi_add_module(FCCFWCorePlugins
                 src/components/*.cpp
                 LINK_LIBRARIES GaudiAlgLib GaudiKernel FWCore FCCEDM ROOT FWCore)
target_link_libraries(FCCFWCorePlugins podio::podioRootIO)

