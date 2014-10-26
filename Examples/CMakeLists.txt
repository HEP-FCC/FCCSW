################################################################################
# Package: Examples
################################################################################
gaudi_subdir(Examples v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects)

gaudi_add_module(Examples
                 src/*.cpp
                 LINK_LIBRARIES GaudiAlgLib DataObjects)

