import os
from Gaudi.Configuration import *


from Configurables import DummyTrkGeoSvc
trkgeoservice = DummyTrkGeoSvc("TrackingGeometrySvc") 


from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetSensitive/tests/compact/Box_simpleTrackerSD.xml'], OutputLevel = DEBUG)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc",
                        detector='SimG4DD4hepDetector',
                        physicslist="SimG4FtfpBert",
                        actions="SimG4FullSimActions")

from Configurables import TrkGeoToGdmlDumpSvc
trkgeodumpservice = TrkGeoToGdmlDumpSvc("TrkGeoDump", gdml="TrkGeoDump.gdml")

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [geoservice, geantservice, trkgeoservice, trkgeodumpservice],
                OutputLevel=DEBUG
 )
