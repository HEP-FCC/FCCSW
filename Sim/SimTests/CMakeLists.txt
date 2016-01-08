################################################################################
# Package: SimG4Example
################################################################################
gaudi_subdir(SimTests v1r0)

include(CTest)
gaudi_add_test(GeantFullSimTrackerHits
               FRAMEWORK options/geant_fullsim.py)
gaudi_add_test(GeantFullSimHCalHits
               FRAMEWORK options/geant_fullsim_hcal.py)
gaudi_add_test(GeantFullSimGdmlDetector
               FRAMEWORK options/geant_fullsim_gdml.py)
gaudi_add_test(GeantFastSimTracker
               FRAMEWORK options/geant_fastsim.py)
