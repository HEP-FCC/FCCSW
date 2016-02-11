################################################################################
# Package: Examples
################################################################################
gaudi_subdir(Examples v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects FWCore)

find_package(ROOT COMPONENTS RIO Tree)

gaudi_add_module(Examples
                 src/*.cpp
                 LINK_LIBRARIES GaudiAlgLib DataObjects FWCore ROOT)

gaudi_add_test(DummyGen
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/tests
               FRAMEWORK tests/options/examplejob.py)

gaudi_add_test(DummyGenCheck
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/tests
			   COMMAND python ./scripts/dummy_gen_test.py
			   DEPENDS DummyGen)

gaudi_add_test(DummyCheck
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/tests
			   COMMAND python ./scripts/test.py
			   FAILS)
