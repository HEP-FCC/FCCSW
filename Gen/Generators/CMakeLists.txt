################################################################################
# Package: Generators
################################################################################
gaudi_subdir(Generators v15r0)

gaudi_depends_on_subdirs(Gen/Event
                         Gen/MCInterfaces)


find_package(Boost COMPONENTS filesystem system)
find_package(HepMC)
find_package(CLHEP)
find_package(ROOT COMPONENTS MathCore GenVector)

gaudi_add_library(GeneratorsLib
                  src/Lib/*.cpp src/Lib/*.F
                  PUBLIC_HEADERS Generators
                  INCLUDE_DIRS Kernel/MCInterfaces HepMC
                  LINK_LIBRARIES Event HepMC GaudiAlgLib)

gaudi_add_module(Generators
                 src/component/*.cpp
                 INCLUDE_DIRS Boost Kernel/MCInterfaces HepMC CLHEP
                 LINK_LIBRARIES Boost Event GaudiAlgLib GeneratorsLib HepMC CLHEP ROOT)

