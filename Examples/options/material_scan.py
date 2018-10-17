import os
from Gaudi.Configuration import *

from Configurables import GeoSvc

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectMaster.xml'], OutputLevel = DEBUG)

from Configurables import MaterialScan
# Material scan is done from the interaction point to the end of world volume.
# In order to use other end boundary, please provide the name of a thin, e.g. cylindrical volume.
# For instance adding envelopeName="BoundaryPostCalorimetry" will perform the scan only till the end of calorimetry.
# BoundaryPostCalorimetry is defined in Detector/DetFCChhECalInclined/compact/envelopePreCalo.xml
materialservice = MaterialScan("GeoDump", filename="DD4hep_material_scan.root", etaBinning=0.05, etaMax=6, nPhiTrials=10)

from Configurables import PodioOutput
## PODIO algorithm
out = PodioOutput("out", OutputLevel=DEBUG)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, materialservice],
                OutputLevel=DEBUG
 )
