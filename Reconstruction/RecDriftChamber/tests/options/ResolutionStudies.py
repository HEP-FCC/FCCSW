from Gaudi.Configuration import *



from Configurables import ApplicationMgr



# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['Detector/DetFCCeeIDEA/compact/FCCee_DectEmptyMaster.xml',
                                         'Detector/DetFCCeeIDEA/compact/DriftChamber.xml'], 
                    OutputLevel = DEBUG)

from Configurables import ResolutionStudies
resStudies = ResolutionStudies("ResolutionStudies",
                               readoutName = "DriftChamberCollection",)




ApplicationMgr( TopAlg = [resStudies],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [
                          geoservice
                          ],
                OutputLevel = DEBUG
 )


