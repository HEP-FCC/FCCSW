################################################################################
#Package : DetFCChhECalInclined
################################################################################
gaudi_subdir(DetFCChhECalInclined v1r0)

gaudi_depends_on_subdirs(GaudiKernel)


set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake )
include( DD4hep )


#include_directories(BEFORE SYSTEM ${DD4hep_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/Detector/DetFCChhECalInclined)

gaudi_add_module(DetFCChhECalInclined
                 src/*.cpp
                 INCLUDE_DIRS DD4hep ROOT
                 LINK_LIBRARIES GaudiKernel DD4hep ROOT)

set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
dd4hep_generate_rootmap(DetFCChhECalInclined)
