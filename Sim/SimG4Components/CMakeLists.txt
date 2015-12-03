################################################################################
# Package: SimG4Components
################################################################################
gaudi_subdir(SimG4Components v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Sim/SimG4Common DetectorDescription/DetDesInterfaces Sim/SimG4Interface)

find_package(Geant4)
include(${Geant4_USE_FILE})

gaudi_install_headers(SimG4Components)

gaudi_add_module(SimG4Components
                 src/*.cpp
                 INCLUDE_DIRS Geant4 FWCore SimG4Common
                 LINK_LIBRARIES GaudiAlgLib Geant4 FWCore SimG4Common)
