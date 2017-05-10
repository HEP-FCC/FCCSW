################################################################################
#Package : DetFCChhCalEndcapDiscs
################################################################################
gaudi_subdir(DetFCChhCalEndcapDiscs v1r0)

gaudi_depends_on_subdirs(GaudiKernel)

find_package(DD4hep)

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake )
include( DD4hep )

find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

gaudi_add_module(DetFCChhCalEndcapDiscs
                 src/*.cpp
                 INCLUDE_DIRS DD4hep ROOT
                 LINK_LIBRARIES GaudiKernel DD4hep ROOT)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
dd4hep_generate_rootmap(DetFCChhCalEndcapDiscs)
