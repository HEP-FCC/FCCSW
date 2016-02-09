
from Gaudi.Configuration import *
from Configurables import ApplicationMgr, GeoSvc , ClassicalRecoGeoSvc, RecoGeoTest, SmearingTool, RecoGeoWrite, GaussDigitizer, PrintHits

#services
detservice = GeoSvc("GeoSvc", detectors=["file:DetectorDescription/Detectors/compact/TestTracker_sameMat.xml"], OutputLevel = VERBOSE)
recoservice= ClassicalRecoGeoSvc("ClassicalRecoGeoSvc", OutputLevel = VERBOSE)
#tools
smeartool = SmearingTool("SmearingTool", OutputLevel = VERBOSE)
gausssmear= GaussDigitizer("GaussDigitizer", OutputLevel = VERBOSE)
printhits = PrintHits("PrintHis", OutputLevel = VERBOSE)
#smeartool.DataInputs.trackHits.Path = "trackHits" das geht nicht
#algorithms
test = RecoGeoTest("RecoGeoTest")
#test.DataOutputs.particles.Path = "particles"
#test.DataInputs.trackHits.Path = "trackHits"

write = RecoGeoWrite("RecoGeoWrite")
#write.DataInputs.particles.Path = "particles"

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               ExtSvc = [recoservice, detservice],
               TopAlg=[test, write])
