
from Gaudi.Configuration import *
from Configurables import ApplicationMgr, RecoGeoTest, DD4HepDetDesSvc, RecoGeoSvc, RecoGeoConverterTool, Geant4GeoConverterTool, RndmGenSvc


#genservice = RndmGenSvc("RndmGenSvc")

reco    = RecoGeoConverterTool("RecoGeoConverterTool")#, RndmGenSvc = genservice)
geant4  = Geant4GeoConverterTool("Geant4GeoConverterTool")

detservice = DD4HepDetDesSvc("DD4HepDetDesSvc", OutputLevel = VERBOSE)
recoservice= RecoGeoSvc("RecoGeoSvc", OutputLevel = VERBOSE)
                             
                             #  DD4HepXMLFile = "file:DetectorDescription/Detectors/compact/TestTracker1.xml",
# DD4HepXMLFile = "file:DetectorDescription/Detectors/compact/test_FCC_HcalBarrel.xml"
                         #    GeoConverterTools = "RecoGeoConverterTool"
                         
                             
                             #   RecoConverterTool = reco, G4ConverterTool = geant4)



#surface = SurfaceTest("SurfaceTest")

test = RecoGeoTest("RecoGeoTest")



#ServiceMgr(ExtSvc = [detsvc])

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               ExtSvc = [recoservice, detservice],
               TopAlg=[test])
