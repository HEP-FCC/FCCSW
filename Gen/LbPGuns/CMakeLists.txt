################################################################################
# Package: LbPGuns
################################################################################
gaudi_subdir(LbPGuns v6r0p1)

find_package(ROOT COMPONENTS MathCore GenVector)

gaudi_depends_on_subdirs(
                         Gen/Event
                         GaudiAlg
                         Gen/Generators)

gaudi_add_module(LbPGuns
                 src/*.cpp
                 LINK_LIBRARIES GaudiAlgLib GeneratorsLib ROOT)

gaudi_install_headers(LbPGuns)

