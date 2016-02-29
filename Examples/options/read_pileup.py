from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")
# PODIO algorithm
from Configurables import PodioOutput, PodioInput

pilein = PodioInput("in", OutputLevel=DEBUG)
pilein.inputCommands = ["keep * "]
pilein.DataOutputs.trackClusters.Path = "clusters"
#pilein.DataOutputs.trackHits.Path = "hits"
#pilein.DataOutputs.trackHitsClusters.Path = "hitClusterAssociation"
out = PodioOutput("out",
                  filename="output.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [pilein, out],
               EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent],
                OutputLevel=DEBUG
 )
