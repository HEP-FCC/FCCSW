
from Gaudi.Configuration import *
from k4FWCore import IOSvc

# PODIO output
iosvc = IOSvc()
iosvc.outputCommands = ["keep *"]
iosvc.Output = "out_geant_fullsim.root"

