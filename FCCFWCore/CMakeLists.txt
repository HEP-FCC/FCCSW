################################################################################
# Package: FCCFWCore
################################################################################
gaudi_subdir(FCCFWCore v1r0)

find_package(podio)
find_package(FCCEDM)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg GaudiKernel FWCore)
gaudi_install_scripts()
gaudi_install_python_modules()

#gaudi_add_library(FCCFWCore
#		  src/*.cpp
#                  INCLUDE_DIRS FCCEDM PODIO ROOT FWCore
#                  LINK_LIBRARIES GaudiAlgLib GaudiKernel ROOT FCCEDM FWCore
#                  PUBLIC_HEADERS FCCFWCore)
#target_link_libraries(FCCFWCore podio::podioRootIO)
#
gaudi_add_module(FCCFWCorePlugins
                 src/components/*.cpp
                 LINK_LIBRARIES GaudiAlgLib GaudiKernel FWCore FCCEDM ROOT FWCore)
target_link_libraries(FCCFWCorePlugins podio::podioRootIO)

gaudi_add_test(ProduceForReadTest
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/simple_producer.py)
gaudi_add_test(ReadTest
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/simple_reader.py
               DEPENDS ProduceForReadTest)
gaudi_add_test(CheckReadCollectionSize
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND python FCCFWCore/tests/scripts/check_coll_after_read.py
               DEPENDS ReadTest)
