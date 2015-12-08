################################################################################
# Package: SimG4Data
################################################################################
gaudi_subdir(SimG4Data v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects)

find_package(Geant4)

gaudi_install_headers(SimG4Data)

gaudi_add_library(SimG4Data
                 src/*.cpp
                 INCLUDE_DIRS SimG4Data FWCore Geant4
                 LINK_LIBRARIES GaudiAlgLib DataObjects Geant4
                 PUBLIC_HEADERS SimG4Data)
