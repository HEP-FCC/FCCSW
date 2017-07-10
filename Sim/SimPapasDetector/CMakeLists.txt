################################################################################
#Package : SimPapasDetector
################################################################################
gaudi_subdir(SimPapasDetector v1r0)
gaudi_depends_on_subdirs(GaudiKernel
                         FWCore
                         )

find_package(ROOT COMPONENTS MathCore GenVector Geom Tree  Physics EG)
find_package(papas)
find_package(papas)
find_package(Geant4)
include(${Geant4_USE_FILE})

gaudi_install_headers(SimPapasDetector)
gaudi_install_python_modules()

gaudi_add_module(SimPapasDetector
                 src/*.cpp
                 INCLUDE_DIRS GaudiKernel ROOT Geant4 FWCore papas
                 LINK_LIBRARIES GaudiKernel ROOT Geant4 papas)


