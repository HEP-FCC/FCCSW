################################################################################
# Package: FWCore
################################################################################
gaudi_subdir(FWCore v1r0)

find_package(FCCEDM)
find_package(ALBERS)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg GaudiKernel)

gaudi_add_library(FWCore
		  src/*.cpp
                  LINK_LIBRARIES GaudiAlgLib GaudiKernel FCCEDM ALBERS
                  INCLUDE_DIRS FCCEDM ALBERS
                  PUBLIC_HEADERS FWCore)

gaudi_add_module(FWCorePlugins
                 components/*.cpp
                 LINK_LIBRARIES GaudiAlgLib GaudiKernel FWCore)
