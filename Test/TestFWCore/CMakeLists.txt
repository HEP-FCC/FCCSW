if(NOT BUILD_TESTING)
  return()
endif()
################################################################################
# Package: TestFCCFWCore
################################################################################
gaudi_subdir(TestFCCFWCore v1r0)

gaudi_depends_on_subdirs(GaudiAlg GaudiKernel FCCFWCore Generation)

gaudi_add_module(TestFCCFWCorePlugins
                 src/components/*.cpp
                 INCLUDE_DIRS FWCore FCCEDM
                 LINK_LIBRARIES GaudiKernel FWCore FCCEDM)


include(CTest)
gaudi_add_test(ProduceForOverlayTest1
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND fccrun Test/TestFCCFWCore/options/createExampleEventData.py --magicNumberOffset=0 --filename=output_dummyEventData1.root)

gaudi_add_test(ProduceForOverlayTest2
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND fccrun Test/TestFCCFWCore/options/createExampleEventData.py --magicNumberOffset=100 --filename=output_dummyEventData2.root )

gaudi_add_test(ProduceForOverlayTest3
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND fccrun Test/TestFCCFWCore/options/createExampleEventData.py --magicNumberOffset=200 --filename=output_dummyEventData3.root)

gaudi_add_test(OverlayTest
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/overlayDummyEvents.py
               DEPENDS ProduceForOverlayTest1 ProduceForOverlayTest2 ProduceForOverlayTest3)

gaudi_add_test(JobOptionDumpTest
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND fcc_dump_joboptions output_dummyEventData1.root
               DEPENDS ProduceForOverlayTest1
               )

