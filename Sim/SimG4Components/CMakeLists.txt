################################################################################
# Package: SimG4Components
################################################################################
gaudi_subdir(SimG4Components v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Sim/SimG4Common Detector/DetInterface Sim/SimG4Interface Detector/DetCommon)


include(${Geant4_USE_FILE})
find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)

gaudi_install_python_modules()

gaudi_add_module(SimG4Components
                 src/*.cpp
                 INCLUDE_DIRS Geant4 FWCore SimG4Common SimG4Interface DetCommon  DD4hep ROOT
                 LINK_LIBRARIES GaudiAlgLib FWCore SimG4Common DetCommon  DD4hep ROOT)
target_link_libraries(SimG4Components ${Geant4_LIBRARIES})


include(CTest)
gaudi_add_test(GeantFullSimGdml
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/geant_fullsim_gdml.py)
gaudi_add_test(GeantFullSimHCal
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/geant_fullsim_hcal.py)
gaudi_add_test(GeantFullSimMoreEvents
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Sim/SimG4Components/tests/
               FRAMEWORK tests/options/geant_fullsim_moreEvents.py)
gaudi_add_test(GeantFullSimMoreEventsCheckNumEvents
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Sim/SimG4Components/tests/
               COMMAND python ./scripts/geant_fullsim_moreEvents_checkNumEvents.py
               DEPENDS GeantFullSimMoreEvents)
gaudi_add_test(GeantFullSimMoreEventsCheckNumParticles
               ENVIRONMENT PYTHONPATH+=${PODIO_PYTHON_DIR}
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Sim/SimG4Components/tests/
               COMMAND python ./scripts/geant_fullsim_moreEvents_checkNumParticles.py
               DEPENDS GeantFullSimMoreEvents)
gaudi_add_test(GeantFastSimSimpleSmearing
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Sim/SimG4Components/tests/
               FRAMEWORK tests/options/geant_fastsim_simple.py)
gaudi_add_test(GeantFastSimSimpleSmearingCheckSigma
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Sim/SimG4Components/tests/
               COMMAND python ./scripts/geant_fastsim_simple_checkSigma.py
               DEPENDS GeantFastSimSimpleSmearing)
gaudi_add_test(GeantFastSimSimpleSmearingCheckNumParticles
               ENVIRONMENT PYTHONPATH+=${PODIO_PYTHON_DIR}
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Sim/SimG4Components/tests/
               COMMAND python ./scripts/geant_fastsim_checkNumParticles.py
               DEPENDS GeantFastSimSimpleSmearing)

