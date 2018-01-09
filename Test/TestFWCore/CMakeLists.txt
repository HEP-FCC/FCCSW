################################################################################
# Package: TestFWCore
################################################################################
gaudi_subdir(TestFWCore v1r0)

gaudi_depends_on_subdirs(GaudiAlg GaudiKernel FWCore Generation)

gaudi_add_module(TestFWCorePlugins
                 src/components/*.cpp
                 INCLUDE_DIRS FWCore
                 LINK_LIBRARIES GaudiKernel FWCore)


include(CTest)
gaudi_add_test(ProduceForOverlayTest1
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/createExampleEventData1.py)

gaudi_add_test(ProduceForOverlayTest2
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/createExampleEventData2.py)

gaudi_add_test(ProduceForOverlayTest3
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/createExampleEventData3.py)

gaudi_add_test(OverlayTest
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/overlayDummyEvents.py
               DEPENDS ProduceForOverlayTest1 ProduceForOverlayTest2 ProduceForOverlayTest3)
