################################################################################
# Package: Event
################################################################################
gaudi_subdir(Event v1r0)

find_package(HepMC)
find_package(Boost)
find_package(ROOT COMPONENTS MathCore GenVector)

gaudi_depends_on_subdirs(GaudiKernel)

gaudi_add_library(Event
                  src/*.cpp
                  PUBLIC_HEADERS Event
                  INCLUDE_DIRS HepMC Boost
                  LINK_LIBRARIES HepMC Boost GaudiKernel ROOT)
