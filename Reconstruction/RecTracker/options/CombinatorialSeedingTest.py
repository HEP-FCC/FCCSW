import os
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Gaudi.Configuration import *

from FWCore.joboptions import parse_standard_job_options
args = parse_standard_job_options()

inputfile = "muons_for_seeding.root"
if args.inputfile:
  inputfile = args.inputfile

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc", input=inputfile)

from Configurables import PodioInput
podioinput = PodioInput("PodioReader", 
                        collections=[
                                      "GenParticles",
                                      "GenVertices",
                                      "SimParticles", 
                                      "SimVertices", 
                                      "TrackerHits", 
                                      "TrackerPositionedHits", 
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





from Configurables import  CombinatorialSeedingTool

seed_tool = CombinatorialSeedingTool()
seed_tool.seedingLayerIndices0=(0,0)
seed_tool.seedingLayerIndices1=(0,1)
seed_tool.seedingLayerIndices2=(0,2)
seed_tool.trackSeeds.Path = "trackSeeds"
seed_tool.readoutName = "TrackerBarrelReadout"


from Configurables import RecTrackAlg
RecTrackAlg = RecTrackAlg()
RecTrackAlg.TrackerPositionedHits.Path = "TrackerPositionedHits"


# PODIO algorithm
outputfile = "combiSeeding_Example.root"
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
                ExtSvc = [podioevent, geoservice],
                OutputLevel=DEBUG,
 )
