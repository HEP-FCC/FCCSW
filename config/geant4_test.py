
from Gaudi.Configuration import *
from Configurables import ApplicationMgr, Geant4Test, GeoSvc

#geant4  = Geant4GeoConverterTool("Geant4GeoConverterTool")

#detservice = DD4HepDetDesSvc("DD4HepDetDesSvc", OutputLevel = VERBOSE)

detservice = GeoSvc("GeoSvc", OutputLevel = VERBOSE)

test = Geant4Test("Geant4Test")

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               ExtSvc = [detservice],
               TopAlg=[test])
