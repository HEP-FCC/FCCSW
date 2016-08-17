from Gaudi.Configuration import *
from Configurables import ApplicationMgr

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Test/TestGeometry/data/Barrel_testCaloSD_rphiz.xml'],
                    OutputLevel = DEBUG)


from Configurables import TestCellCounting
cells = TestCellCounting("cells", readoutName="ECalHits",
                         fieldNames=["system","r"],
                         fieldValues=[0,250], # 250 = rmin/dr
                         volumeMatchName="slice",
                         OutputLevel = DEBUG)

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[cells],
               ExtSvc = [geoservice],
               OutputLevel=DEBUG)
