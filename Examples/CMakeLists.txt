################################################################################
# Package: Examples
################################################################################
gaudi_subdir(Examples v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Sim/SimG4Interface Detector/DetInterface)

find_package(ROOT COMPONENTS MathCore GenVector Geom)
find_package(DD4hep COMPONENTS DDG4 DDSegmentation REQUIRED)
include(DD4hep)
find_package(Geant4)

gaudi_add_module(Examples
                 src/*.cpp
		           INCLUDE_DIRS ROOT GaudiKernel Geant4 DD4hep SimG4Interface DetInterface
                 LINK_LIBRARIES GaudiAlgLib FWCore ROOT GaudiKernel DD4hep ${DD4hep_COMPONENT_LIBRARIES} Geant4)


include(CTest)

# These are the simple examples defined for doc/AddingTestsToFCCSW.md
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
# End of simple examples

# Tests of example configs
gaudi_add_test(GeantFullSimTrackerHits
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/geant_fullsim.py)
gaudi_add_test(GeantFullSimTrackerHitsWithField
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/geant_fullsim_field.py)
gaudi_add_test(ReadFullSimTrackerHits
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/read_podio_input.py
               DEPENDS GeantFullSimTrackerHitsWithField)
gaudi_add_test(GeantFullSimWithGeantino
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/geant_fullsim_pgun_geantino.py)
gaudi_add_test(GeantFullSimParticleGun
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/geant_pgun_fullsim.py)
gaudi_add_test(GeantFastSimTrackerECalGFlash
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/geant_fastsim.py)
gaudi_add_test(GeantFastSimTrackerTkLayout
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/geant_fastsim_tklayout.py)
gaudi_add_test(SimplePythia
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/simple_pythia.py)
gaudi_add_test(ExampleOptions
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/example_options.py)
gaudi_add_test(DumpGDML
               FRAMEWORK options/dumpGeo.py)

# Test of the format-checker:
gaudi_add_test(FormatCheckerOneFile
     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
     COMMAND ./checkformat ./Examples/src/CreateSampleJet.cpp)
gaudi_add_test(FormatCheckerSeveralFiles
     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
     COMMAND ./checkformat ./Examples/src/CreateSampleJet.cpp ./Examples/src/CreateSampleJet.h)
gaudi_add_test(FormatCheckerDirectory
     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
     COMMAND ./checkformat ./Examples/src/)
gaudi_add_test(FormatCheckerInPlace
     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
     COMMAND ./checkformat -if ./Examples/src/CreateSampleJet.cpp)
