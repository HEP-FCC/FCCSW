################################################################################
# Package: DataObjects
################################################################################
gaudi_subdir(DataObjects v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg)

find_package(HepMC)

gaudi_add_library(DataObjects
		  src/*.cpp
                  INCLUDE_DIRS HepMC 
                  LINK_LIBRARIES HepMC GaudiAlgLib
		  PUBLIC_HEADERS DataObjects)
