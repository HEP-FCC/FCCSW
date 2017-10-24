################################################################################
# Package: SimPapas
################################################################################
gaudi_subdir(SimPapas v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg Sim/SimG4Common)

find_package(Geant4)
include(${Geant4_USE_FILE})

find_package(FCCEDM)
find_package(PODIO)
find_package(ROOT COMPONENTS Tree Physics EG)
find_package(papas)

gaudi_add_module(SimPapas
                 src/*.cpp
                 INCLUDE_DIRS FWCore ROOT FCCEDM PODIO papas SimG4Common
                 LINK_LIBRARIES GaudiAlgLib ROOT FCCEDM PODIO ROOT papas)

include(CTest)
gaudi_add_test(simple_papastool
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/simple_papastool.py)
