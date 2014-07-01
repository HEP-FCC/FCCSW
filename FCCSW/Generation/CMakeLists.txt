################################################################################
# Package: Generation
################################################################################
gaudi_subdir(Generation v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects)

find_package(HepMC)

gaudi_add_module(Generation
                 src/*.cpp
		 INCLUDE_DIRS HepMC
                 LINK_LIBRARIES GaudiAlgLib DataObjects)
