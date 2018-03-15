################################################################################
# Package: TestGeometry
################################################################################
gaudi_subdir(TestGeometryLib v1r0)

gaudi_depends_on_subdirs(GaudiKernel FWCore Sim/SimG4Interface Sim/SimG4Common Detector/DetInterface Detector/DetCommon)

find_package(Geant4)
include(${Geant4_USE_FILE})
find_package(DD4hep)
find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

gaudi_install_headers(TestGeometryLib)

gaudi_add_library(TestGeometryLib
		  src/*.cpp
		  INCLUDE_DIRS Geant4 ROOT TestGeometryLib DD4hep
		  LINK_LIBRARIES GaudiKernel Geant4 ROOT DD4hep
        PUBLIC_HEADERS TestGeometryLib)


set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
dd4hep_generate_rootmap(TestGeometryLib)

