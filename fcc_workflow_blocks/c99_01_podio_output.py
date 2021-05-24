
from Gaudi.Configuration import *

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput()
out.outputCommands = ["keep *"]
out.filename = "out_geant_fullsim.root"
ApplicationMgr().TopAlg += [out]

