################################################################################
# Package: Detectors
################################################################################
gaudi_subdir(Detectors v1r0)

gaudi_depends_on_subdirs(GaudiKernel
			 DataObjects
			 DetectorDescription/DetExtensions)


find_package(DD4hep)

find_package(Geant4)

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake ) 
include( DD4hep )

find_package(ROOT COMPONENTS Reflex MathCore GenVector Geom)

gaudi_add_module(Detectors
		  src/*.cxx
		  INCLUDE_DIRS  DD4hep ROOT DetExtensions Geant4
		  LINK_LIBRARIES GaudiKernel DD4hep ROOT Geant4)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(Detectors)

