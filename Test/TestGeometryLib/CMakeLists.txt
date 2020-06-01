if(NOT BUILD_TESTING)
  return()
endif()
################################################################################
# Package: TestGeometry
################################################################################
gaudi_subdir(TestGeometryLib v1r0)

#gaudi_depends_on_subdirs(FCCFWCore Sim/SimG4Interface Sim/SimG4Common Detector/DetInterface Detector/DetCommon)


include(${Geant4_USE_FILE})

find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)



gaudi_add_library(TestGeometryLib
		  src/*.cpp
		  INCLUDE_DIRS Geant4 ROOT TestGeometryLib DD4hep
		  LINK_LIBRARIES GaudiAlgLib ROOT DD4hep
      PUBLIC_HEADERS TestGeometryLib)
target_link_libraries(TestGeometryLib ${Geant4_LIBRARIES})


set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
dd4hep_generate_rootmap(TestGeometryLib)

