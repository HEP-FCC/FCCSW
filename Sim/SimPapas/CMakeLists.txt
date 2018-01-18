################################################################################
# Package: SimPapas
################################################################################
gaudi_subdir(SimPapas v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg Sim/SimG4Common FWCore GaudiKernel)

find_package(Geant4)
include(${Geant4_USE_FILE})

find_package(FCCEDM)
find_package(PODIO)
find_package(ROOT COMPONENTS MathCore GenVector Tree Physics EG)
find_package(papas)
find_package(Geant4)
include(${Geant4_USE_FILE})

gaudi_add_library(PapasUtils
                 src/lib/*.cpp
                 INCLUDE_DIRS FWCore papas 
                 LINK_LIBRARIES GaudiAlgLib papas
                 PUBLIC_HEADERS SimPapas)


gaudi_add_module(SimPapas
                 src/*.cpp
                 INCLUDE_DIRS  GaudiKernel ROOT Geant4 FWCore FCCEDM PODIO papas SimG4Common SimPapasDetector
                 LINK_LIBRARIES GaudiKernel Geant4  GaudiAlgLib ROOT FCCEDM PODIO ROOT papas)

gaudi_add_module(CMSDetector
                 src/CMSDetector/*.cpp
                 INCLUDE_DIRS  GaudiKernel ROOT Geant4 FWCore FCCEDM PODIO papas SimG4Common SimPapasDetector
                 LINK_LIBRARIES GaudiKernel Geant4  GaudiAlgLib ROOT FCCEDM PODIO ROOT papas)

gaudi_add_module(ClicDetector
                 src/ClicDetector/*.cpp
                 INCLUDE_DIRS  GaudiKernel ROOT Geant4 FWCore FCCEDM PODIO papas SimG4Common SimPapasDetector
                 LINK_LIBRARIES GaudiKernel Geant4  GaudiAlgLib ROOT FCCEDM PODIO ROOT papas)
gaudi_install_headers(SimPapas)
gaudi_install_headers(CMSDetector)
gaudi_install_headers(ClicDetector)
gaudi_install_python_modules()


include(CTest)
gaudi_add_test(simple_papastool
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/simple_papastool.py)
gaudi_add_test(simple_papas_CLIC
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND /bin/bash Sim/SimPapas/scripts/pdebug_CLIC.sh)
gaudi_add_test(simple_papas_CMS
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND /bin/bash Sim/SimPapas/scripts/pdebug_CMS.sh)
