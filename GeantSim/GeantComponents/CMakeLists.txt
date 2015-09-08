################################################################################
# Package: GeantComponents
################################################################################
gaudi_subdir(GeantComponents v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg)

find_package(Geant4)
find_package(ROOT)

gaudi_install_headers(GeantComponents)

gaudi_add_module(GeantComponents
                 src/*.cpp
                 INCLUDE_DIRS ROOT Geant4
                 LINK_LIBRARIES GaudiAlgLib ROOT Geant4)
