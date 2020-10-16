################################################################################
# Package: Generation
################################################################################
gaudi_subdir(Generation v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore)

find_package(HepMC)
find_package(Pythia8 COMPONENTS pythia8 pythia8tohepmc)
find_package(FCCEDM)
find_package(PODIO)
find_package(HepPDT)
find_package(EvtGen)

gaudi_install_headers(Generation)
gaudi_install_python_modules()

gaudi_add_module(Generation
                 src/components/*.cpp
                 INCLUDE_DIRS Generation HepMC Pythia8 FWCore FCCEDM PODIO HepPDT EvtGen
                 LINK_LIBRARIES HepMC GaudiAlgLib FWCore Pythia8 FCCEDM PODIO HepPDT ${EVTGEN_LIBRARIES} EDM4HEP::edm4hep EDM4HEP::edm4hepDict)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/data DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/Generation)

include(CTest)

gaudi_add_test(particleGun
               FRAMEWORK options/particleGun.py
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})


gaudi_add_test(GenPythiaMatching
               COMMAND  fccrun  Examples/options/pythia.py --Filename Generation/data/powheg_ggHH_events.cmd
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
              )


