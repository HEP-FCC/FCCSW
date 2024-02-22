
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

from Configurables import SimG4Alg
geantsim = SimG4Alg("SimG4Alg")

# Set up SaveTools that write 

geantsim.outputs = []

from Configurables import SimG4SaveTrackerHits
savetrackerbarreltool = SimG4SaveTrackerHits("saveTrackerHits")
savetrackerbarreltool.readoutName = "TrackerBarrelReadout"
savetrackerbarreltool.SimTrackHits.Path = "TrackerBarrelReadout"
geantsim.outputs += [savetrackerbarreltool]
savetrackerendcaptool = SimG4SaveTrackerHits("saveTrackerHits")
savetrackerendcaptool.readoutName = "TrackerEndcapReadout"
savetrackerendcaptool.SimTrackHits.Path = "TrackerEndcapReadout"
geantsim.outputs += [savetrackerendcaptool]

from Configurables import SimG4SaveCalHits
saveecaltool = SimG4SaveCalHits("saveECalBarrelHits")
saveecaltool.readoutNames = ["ECalBarrelEta"]
saveecaltool.CaloHits.Path = "ECalBarrelHits"
geantsim.outputs += [saveecaltool]

from Configurables import SimG4SaveCalHits
saveendcaptool = SimG4SaveCalHits("saveECalEndcapHits")
saveendcaptool.readoutNames = ["EMECPhiEta"]
saveendcaptool.CaloHits.Path = "ECalEndcapHits"
geantsim.outputs += [saveendcaptool]

from Configurables import SimG4SaveCalHits
savefwdtool = SimG4SaveCalHits("saveECalFwdHits")
savefwdtool.readoutNames = ["EMFwdPhiEta"]
savefwdtool.CaloHits.Path = "ECalFwdHits"
geantsim.outputs += [savefwdtool]

from Configurables import SimG4SaveCalHits
savehcaltool = SimG4SaveCalHits("saveHCalHits")
savehcaltool.readoutNames = ["HCalBarrelReadout"]
savehcaltool.CaloHits.Path = "HCalBarrelHits"
geantsim.outputs += [savehcaltool]

