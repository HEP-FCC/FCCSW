
import os
from Gaudi.Configuration import *
from Gaudi import Configurables


# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
path_to_detectors = os.environ.get("FCCDETECTORS", "")
geoservice = GeoSvc("GeoSvc")
detectors = [ 'Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
              'Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
              ]
geoservice.detectors = [ os.path.join(path_to_detectors, d) for d in detectors ]
geoservice.OutputLevel = INFO 
ApplicationMgr().ExtSvc += [geoservice]

from Configurables import SimG4SaveTrackerHits
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits")
savetrackertool.readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"]
from Configurables import SimG4Alg
SimG4Alg("SimG4Alg").outputs += [savetrackertool]
