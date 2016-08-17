from Gaudi.Configuration import *
from Configurables import ApplicationMgr

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Test/TestGeometry/data/Barrel_testCaloSD_phieta.xml'],
                    OutputLevel = DEBUG)


from Configurables import TestCellCounting
cells = TestCellCounting("cells", readoutName="ECalHits",
                         fieldNames=["system"],
                         fieldValues=[0],
                         volumeMatchName="BarrelECal",
                         OutputLevel = DEBUG)

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[cells],
               ExtSvc=[geoservice],
               OutputLevel=DEBUG)
