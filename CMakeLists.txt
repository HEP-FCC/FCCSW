CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
find_package(Delphes)
#---------------------------------------------------------------

# Declare project name and version
# Syntax:
#    gaudi_project(this_project this_version
#                  USE dep_project_1 version_1 [project_2 version_2 ...]
#                  [DATA pkg1 [VERSION vers1] [pkg2 ...])
gaudi_project(FCCSW HEAD
              USE Gaudi v26r4 )

set(CMAKE_CXX_FLAGS
        "-fmessage-length=0 -pipe -Wall -Wextra -Werror=return-type -pthread -pedantic -Wwrite-strings -Wpointer-arith -Woverloaded-virtual -Wno-long-long -Wno-shadow"
    CACHE STRING "Flags used by the compiler during all build types."
    FORCE)

include_directories(
         ${DELPHES_INCLUDE_DIRS}
                     ${DELPHES_EXTERNALS_INCLUDE_DIRS}
                     ${FCCEDM_INCLUDE_DIRS}
)

include(CTest)
gaudi_add_test(GeantFullSimTrackerHits
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/geant_fullsim.py)
gaudi_add_test(GeantFastSimTracker
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/geant_fastsim.py)
gaudi_add_test(RecoGeo
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/recogeo_options.py)
gaudi_add_test(SimplePythia
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/simple_pythia.py)
gaudi_add_test(SimpleWorkflow
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/simple_workflow.py)
gaudi_add_test(ExampleOptions
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/example_options.py)
gaudi_add_test(PythiaDelphes
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/PythiaDelphes_config.py)
