import os
from Gaudi.Configuration import *

from Configurables import ApplicationMgr
app = ApplicationMgr()
app.TopAlg = []
app.EvtSel = 'NONE'
app.EvtMax = 1
app.ExtSvc = []
app.OutputLevel = DEBUG

# order is important, as GeoSvc is needed by SimG4Svc
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc")
geoservice.detectors=[
  'file:../Detector/DetSensitive/tests/compact/Box_simpleTrackerSD.xml',
  ]
geoservice.OutputLevel = INFO
app.ExtSvc += [geoservice]

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc")
app.ExtSvc += [geantservice]

export_fname = "TestBox.gdml"
# check if file exists and delete it:
if os.path.isfile(export_fname):
    os.remove(export_fname)

from Configurables import GeoToGdmlDumpSvc
geodumpservice = GeoToGdmlDumpSvc()
geodumpservice.gdml=export_fname
app.ExtSvc += [geodumpservice]


