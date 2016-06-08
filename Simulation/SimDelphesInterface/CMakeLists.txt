################################################################################
# Package: SimDelpesInterface
################################################################################
gaudi_subdir(SimDelphesInterface v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg)

find_package(Delphes)
find_package(FCCEDM)
find_package(PODIO)
find_package(ROOT COMPONENTS Physics EG)
find_package(HepMC)

gaudi_add_module(SimDelphesInterface
                 src/*.cpp
                 INCLUDE_DIRS FWCore ROOT Delphes FCCEDM PODIO HepMC
                 LINK_LIBRARIES GaudiAlgLib ROOT Delphes FCCEDM PODIO HepMC)


include(CTest)
gaudi_add_test(PythiaDelphes
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/PythiaDelphes_config.py)
