from Gaudi.Configuration import *

from GaudiKernel import SystemOfUnits as units 
# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc", input="output.root")

from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["allGenVertices", "allGenParticles", "hits", "positionedHits"])

from Configurables import CASeedAlg
seedalg = CASeedAlg(deltaPhi=0.2*units.rad, deltaZ=20*units.mm, deltaT=100*units.ns)
seedalg.positionedTrackHits.Path = "positionedHits"

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename="output.root"

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [podioinput, seedalg],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent],
                OutputLevel=DEBUG
 )
