
from Gaudi.Configuration import *
from Configurables import ApplicationMgr, GeoSvc , ClassicalRecoGeoSvc, RecoGeoTest, SmearingTool, RecoGeoWrite, GaussDigitizer, PrintHits

#services
detservice = GeoSvc("GeoSvc", OutputLevel = VERBOSE)
recoservice= ClassicalRecoGeoSvc("ClassicalRecoGeoSvc", OutputLevel = VERBOSE)
#tools
smeartool = SmearingTool("SmearingTool", OutputLevel = VERBOSE)
gausssmear= GaussDigitizer("GaussDigitizer", OutputLevel = VERBOSE)
printhits = PrintHits("PrintHis", OutputLevel = VERBOSE)
#smeartool.Inputs.trackHits.Path = "trackHits" das geht nicht
#algorithms
test = RecoGeoTest("RecoGeoTest")
#test.Outputs.particles.Path = "particles"
#test.Inputs.trackHits.Path = "trackHits"

write = RecoGeoWrite("RecoGeoWrite")
#write.Inputs.particles.Path = "particles"

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               ExtSvc = [recoservice, detservice],
               TopAlg=[test, write])
