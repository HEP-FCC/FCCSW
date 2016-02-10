################################################################################
# Package: Examples
################################################################################
gaudi_subdir(Examples v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg DataObjects FWCore)

gaudi_add_module(Examples
                 src/*.cpp
                 LINK_LIBRARIES GaudiAlgLib DataObjects FWCore)


include(CTest)
gaudi_add_test(GeantFullSimTrackerHits
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/geant_fullsim.py)
gaudi_add_test(GeantFullSimParticleGun
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/geant_pgun_fullsim.py)
gaudi_add_test(GeantFastSimTracker
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/geant_fastsim.py)
gaudi_add_test(SimplePythia
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/simple_pythia.py)
gaudi_add_test(SimpleWorkflow
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/simple_workflow.py)
gaudi_add_test(ExampleOptions
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/example_options.py)
