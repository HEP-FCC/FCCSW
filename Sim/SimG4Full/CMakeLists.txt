################################################################################
# Package: SimG4Full
################################################################################
gaudi_subdir(SimG4Full v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg Sim/SimG4Components Sim/SimG4Interface Sim/SimG4Common)


include(${Geant4_USE_FILE})
find_package(ROOT)

gaudi_add_library(SimG4Full
                 src/lib/*.cpp
                 INCLUDE_DIRS SimG4Common FWCore SimG4Components SimG4Interface Geant4 ROOT
                 LINK_LIBRARIES GaudiAlgLib SimG4Common ROOT
                 PUBLIC_HEADERS SimG4Full)
target_link_libraries(SimG4Full ${Geant4_LIBRARIES})

gaudi_add_module(SimG4FullPlugins
                 src/components/*.cpp
                 INCLUDE_DIRS Geant4 SimG4Common SimG4Full SimG4Interface
                 LINK_LIBRARIES GaudiAlgLib SimG4Full SimG4Common)
target_link_libraries(SimG4FullPlugins ${Geant4_LIBRARIES})
