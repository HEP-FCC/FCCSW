################################################################################
# Package: DataObjects
################################################################################
gaudi_subdir(DataObjects v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg albers)

find_package(HepMC)
gaudi_add_dictionary(DataObjects dict/dictionary.h  dict/dictionary.xml
                     INCLUDE_DIRS DataObjects
                     LINK_LIBRARIES DataObjects) 

gaudi_add_library(DataObjects
		  src/*.cc
                  INCLUDE_DIRS HepMC 
                  LINK_LIBRARIES HepMC GaudiAlgLib albers
		  PUBLIC_HEADERS DataObjects)
