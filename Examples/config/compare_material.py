
from Gaudi.Configuration import *
from Configurables import ApplicationMgr, CompareMaterial, GeoSvc, ClassicalRecoGeoSvc

detservice = GeoSvc("GeoSvc", OutputLevel = VERBOSE)
recoservice= ClassicalRecoGeoSvc("ClassicalRecoGeoSvc", OutputLevel = VERBOSE)

test = CompareMaterial("CompareMaterial")

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               ExtSvc = [detservice, recoservice],
               TopAlg=[test])

