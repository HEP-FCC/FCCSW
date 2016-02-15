################################################################################
# Package: TestDD4hep
################################################################################
gaudi_subdir(TestDD4hep v1r0)

#this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg GaudiKernel)

find_package(ROOT COMPONENTS MathCore GenVector Geom)
find_package(DD4hep COMPONENTS DDG4)
find_package(Geant4)

gaudi_add_module(TestDD4hep
		 src/*.cpp
		 INCLUDE_DIRS ROOT GaudiKernel Geant4 DD4hep
       LINK_LIBRARIES GaudiAlgLib ROOT GaudiKernel DD4hep Geant4)

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake )
include( DD4hep )

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(TestDD4hep)

include(CTest)
gaudi_add_test(TestDD4hepDumpGDML
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Test/TestDD4hep/tests
               FRAMEWORK options/dumpGeo.py)
