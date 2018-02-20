import os
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc", input="output_geantinos.root")
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=[ "hits", "positionedHits", "trajectory", "trajectoryPoints"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
  ],
  )


from Configurables import TrackFitTest


fittest = TrackFitTest()
fittest.positionedTrackHits.Path = "positionedHits"


# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename="fitTest_Example.root"

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [podioinput, fittest, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent],
                OutputLevel=DEBUG
 )
