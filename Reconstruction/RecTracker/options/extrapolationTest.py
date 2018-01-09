from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")

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



from Configurables import ExtrapolationTest
exTest = ExtrapolationTest()
exTest.positionedTrackHits.Path="positionedHits"
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename = 'output_trkExtrapolationTest.root'


from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [exTest, out],
                EvtSel = 'NONE',
                EvtMax   = 1000,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice, trkgeoservice, ],
                OutputLevel=DEBUG,
 )
