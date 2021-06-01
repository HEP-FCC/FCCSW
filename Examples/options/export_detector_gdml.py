import os
from Gaudi.Configuration import *

from Configurables import ApplicationMgr
app = ApplicationMgr()
app.TopAlg = []
app.EvtSel = 'NONE'
app.EvtMax = 1
app.ExtSvc = []
app.OutputLevel = DEBUG

# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
path_to_detectors = os.environ.get("FCCDETECTORS", "")
geoservice = GeoSvc("GeoSvc")
geoservice.detectors = [
                          os.path.join(path_to_detectors, 'Detector/DetFCCeeCLD/compact/FCCee_DectMaster.xml'),
                       ]
ApplicationMgr().ExtSvc += [geoservice]


from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc")
ApplicationMgr().ExtSvc += [geantservice]


from Configurables import GeoToGdmlDumpSvc
geodumpservice = GeoToGdmlDumpSvc()
geodumpservice.gdml = "DetFCCeeCLD.gdml"
ApplicationMgr().ExtSvc += [geodumpservice]


