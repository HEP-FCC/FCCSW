#import os
from Gaudi.Configuration import *

# geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                         'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'])

# tracking geomery sercive
from Configurables import TrackingGeoSvc
trkgeoservice = TrackingGeoSvc("TrackingGeometryService")




inputHits  = "puTrackerDigiPostPoint"
output_name ="test.root"

num_events = 1

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc", input="/eos/experiment/fcc/hh/simulation/samples/v03/physics/MinBias/bFieldOn/etaFull/simuPUTracker1000/output_486797548.root")
# the input
from Configurables import PodioInput
podioInput = PodioInput("PodioInput", collections=[inputHits])

# the cluster writer
# todo option to turn off cluster writer
from Configurables import ModuleClusterWriter
clusterWriter = ModuleClusterWriter("ClusteWriter")
clusterWriter.filename = ("Clusters_" + output_name)
# the digitizer
from Configurables import GeometricTrackerDigitizer
digitizer = GeometricTrackerDigitizer()
digitizer.digiTrackHitAssociation.Path=inputHits
digitizer.trackClusters.Path="trackClusters"
digitizer.clusterTrackHits.Path="clusterTrackHits"
digitizer.analogReadout=FALSE
digitizer.clusterWriter = clusterWriter
digitizer.writeClusterInfo = TRUE
digitizer.commonCorner=TRUE
digitizer.cosThetaLocMin = 0

# PODIO algorithm
#from Configurables import PodioOutput
#out = PodioOutput("out")
#out.outputCommands = ["drop *", "keep trackClusters", "keep clusterTrackHits"]
#out.filename = output_name

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[podioInput,digitizer],
               EvtSel='NONE',
               EvtMax=num_events,
               ExtSvc=[geoservice, trkgeoservice, podioevent]
               )

