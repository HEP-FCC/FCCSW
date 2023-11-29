
import os
from Gaudi.Configuration import *

# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
path_to_detectors = os.environ.get("FCCDETECTORS", "")
geoservice = GeoSvc("GeoSvc")
geoservice.detectors = [
                          os.path.join(path_to_detectors, 'Detector/DetFCCeeCLD/compact/FCCee_DectMaster.xml'),
                       ]


from Configurables import SimG4Alg
SimG4Alg("SimG4Alg").outputs = []

from Configurables import SimG4SaveTrackerHits
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits_Barrel")
savetrackertool.readoutNames = ["VertexBarrelCollection"]
savetrackertool.SimTrackHits.Path = "hits_VXD_barrel"
SimG4Alg("SimG4Alg").outputs += [savetrackertool]

from Configurables import SimG4SaveTrackerHits
savetrackertool_endcap = SimG4SaveTrackerHits("saveTrackerHits_Endcap")
savetrackertool_endcap.readoutNames = ["VertexEndcapCollection"]
savetrackertool_endcap.SimTrackHits.Path = "hits_VXD_endcap"
SimG4Alg("SimG4Alg").outputs += [savetrackertool_endcap]

from Configurables import SimG4SaveTrackerHits
savetrackertool_ITB = SimG4SaveTrackerHits("saveTrackerHits_IT_Barrel")
savetrackertool_ITB.readoutNames = ["InnerTrackerBarrelCollection"]
savetrackertool_ITB.SimTrackHits.Path = "hits_IT_barrel"
SimG4Alg("SimG4Alg").outputs += [savetrackertool_ITB]

from Configurables import SimG4SaveTrackerHits
savetrackertool_ITE = SimG4SaveTrackerHits("saveTrackerHits_IT_Endcap")
savetrackertool_ITE.readoutNames = ["InnerTrackerEndcapCollection"]
savetrackertool_ITE.SimTrackHits.Path = "hits_IT_endcap"
SimG4Alg("SimG4Alg").outputs += [savetrackertool_ITE]

from Configurables import SimG4SaveTrackerHits
savetrackertool_OTB = SimG4SaveTrackerHits("saveTrackerHits_OT_Barrel")
savetrackertool_OTB.readoutNames = ["OuterTrackerBarrelCollection"]
savetrackertool_OTB.SimTrackHits.Path = "hits_OT_barrel"
SimG4Alg("SimG4Alg").outputs += [savetrackertool_OTB]

from Configurables import SimG4SaveTrackerHits
savetrackertool_OTE = SimG4SaveTrackerHits("saveTrackerHits_OT_Endcap")
savetrackertool_OTE.readoutNames = ["OuterTrackerEndcapCollection"]
savetrackertool_OTE.SimTrackHits.Path = "hits_OT_endcap"
SimG4Alg("SimG4Alg").outputs += [savetrackertool_OTE]

from Configurables import SimG4SaveTrackerHits
savelumicaltool = SimG4SaveCalHits("saveLumicalHits")
savelumicaltool.readoutNames = ["LumiCalCollection"]
savelumicaltool.CaloHits.Path = "hits_Lumical"
SimG4Alg("SimG4Alg").outputs += [savelumicaltool]

from Configurables import SimG4SaveCalHits
saveEcalBarreltool = SimG4SaveCalHits("saveEcalBarrelHits")
saveEcalBarreltool.readoutNames = ["ECalBarrelCollection"]
saveEcalBarreltool.CaloHits.Path = "hits_EcalBarrel"
SimG4Alg("SimG4Alg").outputs += [saveEcalBarreltool]

from Configurables import SimG4SaveCalHits
saveEcalEndcaptool = SimG4SaveCalHits("saveEcalEndcapHits")
saveEcalEndcaptool.readoutNames = ["ECalEndcapCollection"]
saveEcalEndcaptool.CaloHits.Path = "hits_EcalEndcap"
SimG4Alg("SimG4Alg").outputs += [saveEcalEndcaptool]

from Configurables import SimG4SaveCalHits
saveHcalBarreltool = SimG4SaveCalHits("saveHcalBarrelHits")
saveHcalBarreltool.readoutNames = ["HCalBarrelCollection"]
saveHcalBarreltool.CaloHits.Path = "hits_HcalBarrel"
SimG4Alg("SimG4Alg").outputs += [saveHcalBarreltool]

from Configurables import SimG4SaveCalHits
saveHcalEndcaptool = SimG4SaveCalHits("saveHcalEndcapHits")
saveHcalEndcaptool.readoutNames = ["HCalEndcapCollection"]
saveHcalEndcaptool.CaloHits.Path = "hits_HcalEndcap"
SimG4Alg("SimG4Alg").outputs += [saveHcalEndcaptool]

