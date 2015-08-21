################################################################################
# Package: Simulation
################################################################################
gaudi_subdir(Simulation v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects  DetectorDescription/DetDesInterfaces)

find_package(Geant4)
find_package(Delphes)
find_package(FCCEDM)
find_package(ALBERS)
find_package(ROOT COMPONENTS Physics)


gaudi_add_module(Simulation
                 src/*.cpp
                 INCLUDE_DIRS Geant4 FWCore ROOT Delphes FCCEDM ALBERS
                 LINK_LIBRARIES GaudiAlgLib DataObjects Geant4  ROOT Delphes FCCEDM ALBERS)


