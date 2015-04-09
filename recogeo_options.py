
from Gaudi.Configuration import *
from Configurables import ApplicationMgr, GeoSvc , StandardRecoGeoSvc, RecoGeoTest


detservice = GeoSvc("GeoSvc", OutputLevel = VERBOSE)
recoservice= StandardRecoGeoSvc("StandardRecoGeoSvc", OutputLevel = VERBOSE)


test = RecoGeoTest("RecoGeoTest")

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               ExtSvc = [recoservice, detservice],
               TopAlg=[test])
