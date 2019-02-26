from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc", input="output_trkExtrapolationTest.root")

from Configurables import PodioInput
podioinput = PodioInput("PodioReader", 
                        collections= [
                                      "ExtrapolatedTrackstates",
                                      "SimTrackerPositionedHits",
                                      ],
                          OutputLevel=DEBUG,
                          )


## create DD4hep geometry
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                         'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'],
                                         OutputLevel=INFO)
## create tracking geometry
from Configurables import TrackingGeoSvc
trkgeoservice = TrackingGeoSvc("TrackingGeometryService")

## configure the extrapolation tool
from Configurables import ActsExtrapolationTool
extrapolationTool = ActsExtrapolationTool("TrackExtrapolationTool")
extrapolationTool.trackingGeometrySvc   = trkgeoservice
extrapolationTool.collectSensitive      = TRUE
extrapolationTool.collectPassive        = TRUE
extrapolationTool.collectBoundary       = TRUE
extrapolationTool.collectMaterial       = TRUE
extrapolationTool.sensitiveCurvilinear  = FALSE
extrapolationTool.searchMode            = 1
extrapolationTool.pathLimit             = -1.
extrapolationTool.bFieldZ = 4.

from Configurables import KalmanFilter
kalman = KalmanFilter()
kalman.FittedTracks.Path = "FittedTracks"
kalman.TrackSeeds.Path = "ExtrapolatedTrackstates"
kalman.TrackerHits.Path = "SimTrackerPositionedHits"


from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [podioinput, kalman],
                EvtSel = 'NONE',
                EvtMax   = 10,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, trkgeoservice,],
                OutputLevel=DEBUG,
 )
