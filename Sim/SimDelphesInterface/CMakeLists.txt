################################################################################
# Package: SimDelpesInterface
################################################################################
gaudi_subdir(SimDelphesInterface v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects)

find_package(Delphes)
find_package(FCCEDM)
find_package(PODIO)
find_package(ROOT COMPONENTS Physics EG)


gaudi_add_module(SimDelphesInterface
                 src/*.cpp
                 INCLUDE_DIRS FWCore ROOT Delphes FCCEDM PODIO
                 LINK_LIBRARIES GaudiAlgLib DataObjects ROOT Delphes FCCEDM PODIO)


