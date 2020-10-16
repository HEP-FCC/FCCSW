################################################################################
# Package: SimG4Fast
################################################################################
gaudi_subdir(SimG4Fast v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg Sim/SimG4Components Sim/SimG4Interface Sim/SimG4Common)


include(${Geant4_USE_FILE})
find_package(ROOT COMPONENTS Tree)

gaudi_add_library(SimG4Fast
                 src/lib/*.cpp
                 INCLUDE_DIRS SimG4Common FWCore SimG4Components SimG4Interface Geant4 ROOT FCCEDM
                 LINK_LIBRARIES GaudiAlgLib SimG4Common ROOT FCCEDM
                 PUBLIC_HEADERS SimG4Fast)
target_link_libraries(SimG4Fast ${Geant4_LIBRARIES})

gaudi_add_module(SimG4FastPlugins
                 src/components/*.cpp
                 INCLUDE_DIRS Geant4 SimG4Common SimG4Fast SimG4Interface FCCEDM
                 LINK_LIBRARIES GaudiAlgLib SimG4Fast SimG4Common FCCEDM)
target_link_libraries(SimG4FastPlugins ${Geant4_LIBRARIES})
