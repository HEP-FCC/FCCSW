import os
from Gaudi.Configuration import *

from Configurables import ApplicationMgr
app = ApplicationMgr()
app.TopAlg = []
app.EvtSel = 'NONE'
app.EvtMax = 1
app.ExtSvc = []
app.OutputLevel = INFO

# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
path_to_detectors = os.environ.get("FCCDETECTORS", "")
geoservice = GeoSvc("GeoSvc")
geoservice.detectors = [
                          os.path.join(path_to_detectors, 'Detector/DetFCCeeCLD/compact/FCCee_o2_v02/FCCee_o2_v02.xml'),
                       ]
ApplicationMgr().ExtSvc += [geoservice]


from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc")
ApplicationMgr().ExtSvc += [geantservice]


from Configurables import GeoToGdmlDumpSvc
geodumpservice = GeoToGdmlDumpSvc()
geodumpservice.gdml = "DetFCCeeCLD.gdml"
ApplicationMgr().ExtSvc += [geodumpservice]

# avoid error and remove output file if it already exists
if os.path.isfile(geodumpservice.gdml):
      os.remove(geodumpservice.gdml)


