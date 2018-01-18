################################################################################
# Package: DetSensitive
################################################################################
gaudi_subdir(DetSensitive v1r0)

gaudi_depends_on_subdirs(GaudiKernel Detector/DetCommon)

find_package(DD4hep COMPONENTS DDG4 REQUIRED)
find_package(Geant4)
include(${Geant4_USE_FILE})
include(DD4hep)

find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

gaudi_install_headers(DetSensitive)

gaudi_add_library(DetSensitive
                  src/*.cpp
                  INCLUDE_DIRS Geant4 DD4hep ROOT DetCommon
                  LINK_LIBRARIES GaudiKernel DetCommon Geant4 DD4hep ROOT ${DD4hep_COMPONENT_LIBRARIES}
                  PUBLIC_HEADERS DetSensitive)

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake )

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
dd4hep_generate_rootmap(DetSensitive)

include(CTest)
gaudi_add_test(InternalTrackerSD
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Detector/DetSensitive/tests/
               FRAMEWORK tests/options/testDd4hepTrackerSD.py)
gaudi_add_test(ExternalTrackerSD
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Detector/DetSensitive/tests/
               FRAMEWORK tests/options/testSimpleTrackerSD.py)
gaudi_add_test(CmpTrackerSDNoHits
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Detector/DetSensitive/tests/
               COMMAND python ./scripts/cmpTrackerSdNoHits.py
               ENVIRONMENT PYTHONPATH+=$ENV{PODIO}/python
               DEPENDS InternalTrackerSD ExternalTrackerSD)
gaudi_add_test(CmpTrackerSDDetailed
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Detector/DetSensitive/tests/
               COMMAND python ./scripts/cmpTrackerSdDetailed.py
               ENVIRONMENT PYTHONPATH+=$ENV{PODIO}/python
               DEPENDS InternalTrackerSD ExternalTrackerSD)
gaudi_add_test(CmpTrackerSDHitsAndClusters
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Detector/DetSensitive/tests/
               COMMAND python ./scripts/cmpTrackerSdHitsAndClusters.py
               ENVIRONMENT PYTHONPATH+=$ENV{PODIO}/python
               DEPENDS InternalTrackerSD ExternalTrackerSD)
gaudi_add_test(InternalCalorimeterSD
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Detector/DetSensitive/tests/
               FRAMEWORK tests/options/testDd4hepCaloSD.py)
gaudi_add_test(ExternalCalorimeterSD
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Detector/DetSensitive/tests/
               FRAMEWORK tests/options/testSimpleCaloSD.py)
gaudi_add_test(FullParticleAbsorptionSD
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Detector/DetSensitive/tests/
               FRAMEWORK tests/options/testDd4hepFullParticleAbsorptionSD.py)
