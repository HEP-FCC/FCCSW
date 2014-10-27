################################################################################
# Package: albers
################################################################################
gaudi_subdir(albers v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg)

find_package(ROOT COMPONENTS RIO Tree)

gaudi_add_library(albers
		  src/*.cc
                  INCLUDE_DIRS ROOT 
                  LINK_LIBRARIES ROOT
		  PUBLIC_HEADERS albers)
