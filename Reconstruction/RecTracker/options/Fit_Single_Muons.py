import os
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Gaudi.Configuration import *

from FWCore.joboptions import parse_standard_job_options
args = parse_standard_job_options()

inputfile = "muons_for_seeding_single.root"
if args.inputfile:
  inputfile = args.inputfile

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc", input=inputfile)

from Configurables import PodioInput
podioinput = PodioInput("PodioReader", 
                        collections=[
                                      "allGenParticles",
                                      "allGenVertices",
                                      "simParticles", 
                                      "simVertices", 
                                      "hits", 
                                      "positionedHits", 
                                      "trajectory", 
                                      "trajectoryPoints",
                                      ], 
                          OutputLevel=DEBUG,
                          )

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", 
                    detectors=[
                                'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                              ],
                    OutputLevel=INFO,
                   )




# TrickTrack Seeding Configuration
from Configurables import FastHitFilterTool
hitfiltertool = FastHitFilterTool("FastHitFilterTool")

from Configurables import BarrelLayerGraphTool4
layergraphtool = BarrelLayerGraphTool4()

from Configurables import DoubletCreationTool
doublet_tool = DoubletCreationTool()

from Configurables import TrickTrackSeedingTool
tricktrack_seed_tool = TrickTrackSeedingTool()
tricktrack_seed_tool.LayerGraphTool = layergraphtool
tricktrack_seed_tool.deltaZ=36
tricktrack_seed_tool.deltaT=13
tricktrack_seed_tool.deltaPhi=0.4

# Alternative: TruthSeeding
from Configurables import TruthSeedingTool
truth_seeds = TruthSeedingTool()

from Configurables import RecTrackAlg
RecTrackAlg = RecTrackAlg()
RecTrackAlg.Errors = True
RecTrackAlg.hitRes = 5*1e-9
RecTrackAlg.TrackSeedingTool = truth_seeds #tricktrack_seed_tool
RecTrackAlg.positionedTrackHits.Path = "positionedHits"


# PODIO algorithm
outputfile = "single_particle_resolutions.root"
if args.outputfile:
  outputfile = args.outputfile
from Configurables import PodioOutput
out = PodioOutput("out",
                   filename=outputfile,
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]


# get number of events from arguments
nEvents = 2
if args.nevents:
    nEvents = args.nevents

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [podioinput, RecTrackAlg, out],
                EvtSel = 'NONE',
                EvtMax   = nEvents,
                ExtSvc = [podioevent],
                OutputLevel=DEBUG,
 )
