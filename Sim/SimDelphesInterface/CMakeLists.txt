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
                 LINK_LIBRARIES FWCore GaudiAlgLib ROOT Delphes FCCEDM PODIO HepMC)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/data DESTINATION ${CMAKE_INSTALL_DATADIR}/Sim/SimDelphesInterface)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/options DESTINATION ${CMAKE_INSTALL_DATADIR}/Sim/SimDelphesInterface)

target_compile_options(SimDelphesInterface PUBLIC "-Wno-overloaded-virtual")

include(CTest)
gaudi_add_test(PythiaDelphes
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/PythiaDelphes_config.py)
