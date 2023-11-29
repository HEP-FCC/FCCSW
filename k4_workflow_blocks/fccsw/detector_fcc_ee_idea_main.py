
import os
from Gaudi.Configuration import *

# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
path_to_detectors = os.environ.get("FCCDETECTORS", "")
geoservice = GeoSvc("GeoSvc")
geoservice.detectors = [
                          os.path.join(path_to_detectors, 'Detector/DetFCCeeIDEA/compact/FCCee_DectMaster.xml'),
                       ]
geoservice.OutputLevel = INFO 
ApplicationMgr().ExtSvc += [geoservice]

from Configurables import SimG4Alg
SimG4Alg("SimG4Alg").outputs = []

from Configurables import SimG4SaveTrackerHits
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits_Barrel")
savetrackertool.readoutNames = ["VertexBarrelCollection"]
savetrackertool.SimTrackHits.Path = "TrackerHits_barrel"
SimG4Alg("SimG4Alg").outputs += [savetrackertool]

from Configurables import SimG4SaveTrackerHits
savetrackertool_endcap = SimG4SaveTrackerHits("saveTrackerHits_Endcap")
savetrackertool_endcap.readoutNames = ["VertexEndcapCollection"]
savetrackertool_endcap.SimTrackHits.Path = "positionedHits_endcap"
SimG4Alg("SimG4Alg").outputs += [savetrackertool_endcap]


from Configurables import SimG4SaveTrackerHits
savetrackertool_DCH = SimG4SaveTrackerHits("saveTrackerHits_DCH")
saveTrackerHits_DCH.readoutNames = ["SimplifiedDriftChamberCollection"]
savetrackertool_DCH.SimTrackHits.Path = "positionedHits_DCH"
SimG4Alg("SimG4Alg").outputs += [savetrackertool_DCH]
