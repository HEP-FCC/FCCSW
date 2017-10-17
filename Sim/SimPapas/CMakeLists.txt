################################################################################
# Package: SimPapas
################################################################################
gaudi_subdir(SimPapas v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg Sim/SimG4Common Sim/SimPapasDetector)

find_package(Geant4)
include(${Geant4_USE_FILE})

find_package(FCCEDM)
find_package(PODIO)
find_package(ROOT COMPONENTS Tree Physics EG)
find_package(papas)

gaudi_add_library(PapasUtils
                 src/lib/*.cpp
                 INCLUDE_DIRS FWCore papas 
                 LINK_LIBRARIES GaudiAlgLib papas
                 PUBLIC_HEADERS SimPapas)


gaudi_add_module(SimPapas
                 src/*.cpp
                 INCLUDE_DIRS FWCore ROOT FCCEDM PODIO papas SimG4Common SimPapasDetector
                 LINK_LIBRARIES GaudiAlgLib ROOT FCCEDM PODIO ROOT papas)

gaudi_install_headers(SimPapas)
gaudi_install_python_modules()


include(CTest)
gaudi_add_test(simple_papastool
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/simple_papastool.py)
