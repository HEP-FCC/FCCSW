from Gaudi.Configuration import *
from Configurables import ApplicationMgr

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'Test/TestGeometry/data/TestBoxCaloSD_segmentation.xml'], OutputLevel = DEBUG)

from Configurables import TestCellCounting
cells = TestCellCounting("cells", readoutName="ECalHits",
                         fieldNames=["system"],
                         fieldValues=[0],
                         volumeMatchName="BoxECal",
                         OutputLevel = DEBUG)
# ApplicationMgr
ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[cells],
               ExtSvc=[geoservice],
               OutputLevel=DEBUG)
