#################################################################################
##Package : DetSegmentation
#################################################################################
gaudi_subdir(DetSegmentation v1r0)

gaudi_depends_on_subdirs(GaudiKernel)

find_package(DD4hep COMPONENTS DDG4 REQUIRED)
include(DD4hep)

find_package(ROOT COMPONENTS MathCore Physics GenVector Geom REQUIRED)

gaudi_install_headers(DetSegmentation)

gaudi_add_library(DetSegmentation
                 src/*.cpp
                 INCLUDE_DIRS DD4hep ROOT
                 LINK_LIBRARIES GaudiKernel DD4hep ROOT ${DD4hep_COMPONENT_LIBRARIES}
                 PUBLIC_HEADERS DetSegmentation)

gaudi_add_module(DetSegmentationPlugin
                 src/plugins/*.cpp
                 INCLUDE_DIRS DD4hep ROOT
                 LINK_LIBRARIES GaudiKernel DD4hep ROOT ${DD4hep_COMPONENT_LIBRARIES} DetSegmentation)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
dd4hep_generate_rootmap(DetSegmentationPlugin)

include(CTest)
gaudi_add_test(TestSegmentationPhiEta
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/phiEtaSegmentation.py)
