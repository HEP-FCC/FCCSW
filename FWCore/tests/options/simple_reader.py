from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc", input="pythia_test.root", OutputLevel=DEBUG)

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput, ReadTestConsumer
podioinput = PodioInput("PodioReader", collections=["allGenVertices", "allGenParticles"], OutputLevel=DEBUG)
checker = ReadTestConsumer()

out = PodioOutput("out", filename="test.root")
out.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [podioinput, checker,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 3,
    ExtSvc = [podioevent],
    OutputLevel=DEBUG
 )

