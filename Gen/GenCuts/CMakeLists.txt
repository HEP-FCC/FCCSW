################################################################################
# Package: GenCuts
################################################################################
gaudi_subdir(GenCuts v3r12)

gaudi_depends_on_subdirs(GaudiAlg
                         Gen/Event
                         Gen/MCInterfaces
                         )

find_package(FastJet)
find_package(HepMC)
find_package(ROOT COMPONENTS MathCore GenVector)

gaudi_add_module(GenCuts
                 src/*.cpp
                 INCLUDE_DIRS FastJet HepMC
                 LINK_LIBRARIES GaudiAlgLib FastJet HepMC Event ROOT)

