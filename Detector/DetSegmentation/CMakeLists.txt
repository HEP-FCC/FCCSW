################################################################################
# Package: DetSegmentation
################################################################################
gaudi_subdir(DetSegmentation v1r0)

gaudi_depends_on_subdirs(GaudiKernel)

find_package(DD4hep COMPONENTS DDG4 DDSegmentation REQUIRED)
include(DD4hep)

find_package(ROOT COMPONENTS MathCore Physics GenVector Geom REQUIRED)

gaudi_install_headers(DetSegmentation)

gaudi_add_library(DetSegmentation
                  src/*.cpp
                  INCLUDE_DIRS DD4hep ROOT
                  LINK_LIBRARIES GaudiKernel DD4hep ROOT ${DD4hep_COMPONENT_LIBRARIES}
                  PUBLIC_HEADERS DetSegmentation)

include(CTest)
gaudi_add_test(TestSegmentationPhiEta
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               ENVIRONMENT LD_PRELOAD+=build.$BINARY_TAG/lib/libDetSegmentation.so
               FRAMEWORK tests/options/phiEtaSegmentation.py)
