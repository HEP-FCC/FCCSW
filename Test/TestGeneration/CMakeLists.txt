################################################################################
# Package: TestGeneration
################################################################################
gaudi_subdir(TestGeneration v1r0)

gaudi_depends_on_subdirs(GaudiAlg GaudiKernel FWCore Generation)


include(CTest)
gaudi_add_test(EDM2HepMCConverstion
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/edmconvert.py)

gaudi_add_test(GenMergeTest
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/merge.py)
