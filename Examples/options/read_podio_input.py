from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc", input="tracker_with_field.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput, ReadTestConsumer
podioinput = PodioInput("PodioReader", collections=["allGenVertices", "allGenParticles", "hits"], OutputLevel=DEBUG)
checker = ReadTestConsumer()

out = PodioOutput("out", filename="out2.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [podioinput, checker,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 3,
    ExtSvc = [podioevent],
 )

