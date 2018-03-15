################################################################################
# Package: DetActs
################################################################################
gaudi_subdir(DetActs v1r0)

gaudi_depends_on_subdirs(GaudiKernel Detector/DetSegmentation)

find_package(DD4hep COMPONENTS DDG4 DDSegmentation REQUIRED)
find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)
find_package(ACTS COMPONENTS Core Examples TGeoPlugin DD4hepPlugin)

gaudi_install_headers(DetActs)
gaudi_add_library(DetActs
                 src/*.cpp
                 INCLUDE_DIRS DD4hep ROOT ACTS
                 LINK_LIBRARIES GaudiKernel DD4hep ROOT ACTS::ACTSCore
                 PUBLIC_HEADERS DetActs)

install(DIRECTORY DESTINATION Detector/DetActs)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")