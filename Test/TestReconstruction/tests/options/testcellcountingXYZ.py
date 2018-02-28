from Gaudi.Configuration import *
from Configurables import ApplicationMgr

#from Configurables import GeoSvc
#geoservice = GeoSvc("GeoSvc", detectors=[ 'Test/TestGeometry/data/TestBoxCaloSD_segmentation.xml'], OutputLevel = DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/Detector/DetFCChhECalDigital/compact/FCChh_DECalBarrel_50Layers_2.1mmW.xml'
],
                    OutputLevel = DEBUG)

from Configurables import TestCellCounting
cells = TestCellCounting("cells", 
                         readoutName="BarDECal_Readout",#ECalHits",
                         fieldNames=["layer"],
                         fieldValues=[50],
                         volumeMatchName="Silicon_sensitive",
                         OutputLevel = DEBUG)
# ApplicationMgr
ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[cells],
               ExtSvc=[geoservice],
               OutputLevel=DEBUG)
