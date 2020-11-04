################################################################################
# Package: Examples
################################################################################
gaudi_subdir(Examples v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Sim/SimG4Interface Detector/DetInterface Detector/DetCommon)

find_package(ROOT COMPONENTS MathCore GenVector Geom)



install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/options DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/Examples)

gaudi_add_module(Examples
                 src/*.cpp
		             INCLUDE_DIRS ROOT GaudiKernel Geant4 DD4hep SimG4Interface DetInterface DetCommon FCCEDM
                 LINK_LIBRARIES GaudiAlgLib FWCore DetCommon ROOT GaudiKernel DD4hep FCCEDM
                 )
target_link_libraries(Examples ${Geant4_LIBRARIES})


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
               COMMAND fccrun Examples/options/pythia.py)
gaudi_add_test(ExampleOptions
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/example_options.py)
gaudi_add_test(DumpGDML
               FRAMEWORK options/dumpGeo.py)
gaudi_add_test(PileupOverlay
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/overlaySignalAndPileup.py
               DEPENDS GeantFullSimTrackerHitsWithField GeantFullSimParticleGun)
gaudi_add_test(CheckMergeCollection
               ENVIRONMENT PYTHONPATH+=${PODIO_PYTHON_DIR}
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND python FWCore/tests/scripts/check_coll_after_merge.py
               DEPENDS PileupOverlay)
gaudi_add_test(GeantUserLimits
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/geant_userLimits.py)
gaudi_add_test(GeantFullSimParticleGunFCCee
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/geant_fullsim_fccee_pgun.py)
