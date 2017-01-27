import os
from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc", input="output_trk_singlemuons.root")
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["positionedHits"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'])
from Configurables import TrackingGeoSvc
trkgeoservice = TrackingGeoSvc("TrackingGeometryService")

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc",
                        detector='SimG4DD4hepDetector',
                        physicslist="SimG4FtfpBert",
                        actions="SimG4FullSimActions")

from Configurables import TrkGeoToGdmlDumpSvc
trkgeodumpservice = TrkGeoToGdmlDumpSvc("TrkGeoDump", gdml="TrkGeoDump2.gdml")


from Configurables import TrackFit
trackFitAlg = TrackFit()
trackFitAlg.DataInputs.positionedTrackHits.Path = "positionedHits"
#trackFitAlg.DataInputs.trackHits.Path = "hits"


from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [podioinput, trackFitAlg],
                EvtSel = 'NONE',
                EvtMax   = 20,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice, trkgeoservice, trkgeodumpservice],
                OutputLevel=DEBUG
 )
