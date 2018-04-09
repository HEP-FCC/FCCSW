import os
from Gaudi.Configuration import *

from Configurables import GeoSvc

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectMaster.xml'], OutputLevel = DEBUG)

from Configurables import MaterialScan
materialservice = MaterialScan("GeoDump", filename="DD4hep_material_scan_ECAL_noshielding.root", etaBinning=0.05, etaMax=6, nPhiTrials=10) #, envelopeName="BoundaryPostCalorimetry")

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
