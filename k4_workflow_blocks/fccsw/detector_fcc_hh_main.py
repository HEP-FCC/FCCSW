
import os
from Gaudi.Configuration import *

# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
path_to_detectors = os.environ.get("FCCDETECTORS", "")
geoservice = GeoSvc("GeoSvc")
geoservice.detectors = [
                          os.path.join(path_to_detectors, 'Detector/DetFCChhBaseline1/compact/FCChh_DectMaster.xml'),
                       ]
geoservice.OutputLevel = INFO 
ApplicationMgr().ExtSvc += [geoservice]

# Set up SaveTools that write 

from Configurables import SimG4SaveTrackerHits
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits")
savetrackertool.readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"]
from Configurables import SimG4Alg
SimG4Alg("SimG4Alg").outputs += [savetrackertool]

from Configurables import SimG4SaveCalHits
saveecaltool = SimG4SaveCalHits("saveECalBarrelHits")
saveecaltool.readoutNames = ["ECalBarrelEta"]
saveecaltool.CaloHits.Path = "ECalBarrelHits"
SimG4Alg("SimG4Alg").outputs += [saveecaltool]

from Configurables import SimG4SaveCalHits
saveendcaptool = SimG4SaveCalHits("saveECalEndcapHits")
saveendcaptool.readoutNames = ["EMECPhiEta"]
saveendcaptool.CaloHits.Path = "ECalEndcapHits"
SimG4Alg("SimG4Alg").outputs += [saveendcaptool]

from Configurables import SimG4SaveCalHits
savefwdtool = SimG4SaveCalHits("saveECalFwdHits")
savefwdtool.readoutNames = ["EMFwdPhiEta"]
savefwdtool.CaloHits.Path = "ECalFwdHits"
SimG4Alg("SimG4Alg").outputs += [savefwdtool]

from Configurables import SimG4SaveCalHits
savehcaltool = SimG4SaveCalHits("saveHCalHits")
savehcaltool.readoutNames = ["HCalBarrelReadout"]
savehcaltool.CaloHits.Path = "HCalBarrelHits"
SimG4Alg("SimG4Alg").outputs += [savehcaltool]

