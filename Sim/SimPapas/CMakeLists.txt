################################################################################
# Package: SimPapas
################################################################################
gaudi_subdir(SimPapas v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg)

find_package(FCCEDM)
find_package(PODIO)
find_package(ROOT COMPONENTS Physics EG)
find_package(HepMC)
find_package(papas)

gaudi_add_module(SimPapas
                 src/*.cpp
                 INCLUDE_DIRS FWCore ROOT Delphes FCCEDM PODIO HepMC papas
                 LINK_LIBRARIES GaudiAlgLib ROOT Delphes FCCEDM PODIO HepMC papas)

include(CTest)
gaudi_add_test(fcc-generate  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND $ENV{FCCPHYSICS}/bin/fcc-pythia8-generate $ENV{FCCPHYSICS}/share/ee_ZH_Zmumu_Hbb.txt )

gaudi_add_test(simple_papastool
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/simple_papastool.py DEPENDS fcc-generate)
