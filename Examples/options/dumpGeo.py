import os
from Gaudi.Configuration import *

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:../Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'], OutputLevel = DEBUG)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc",
                        detector='SimG4DD4hepDetector',
                        physicslist="SimG4FtfpBert",
                        actions="SimG4FullSimActions")

export_fname = "TestHCAL.gdml"
# check if file exists and delete it:
if os.path.isfile(export_fname):
    os.remove(export_fname)

from Configurables import GeoToGdmlDumpSvc
geodumpservice = GeoToGdmlDumpSvc("GeoDump", gdml=export_fname)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [geoservice, geantservice, geodumpservice],
                OutputLevel=DEBUG
 )
