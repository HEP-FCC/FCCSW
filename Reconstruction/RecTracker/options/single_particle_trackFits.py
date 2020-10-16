import os
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Gaudi.Configuration import *

from FCCFWCore.joboptions import parse_standard_job_options
args = parse_standard_job_options()

inputfile = "muons_for_seeding_discrete_pt.root"
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
                                      "trajectory", 
                                      "trajectoryPoints",
                                      ], 
                          OutputLevel=DEBUG,
                          )






from Configurables import TruthSeedingTool
truth_seeds = TruthSeedingTool()

from Configurables import RiemannFitTool
fittool = RiemannFitTool() 
fittool.hitRes = 1 * 1e-9
fittool.fitOnlyPrimary = True
fittool.doFit = True

from Configurables import RecTrackAlg
RecTrackAlg = RecTrackAlg()
RecTrackAlg.TrackSeedingTool = truth_seeds 
RecTrackAlg.TrackFittingTool = fittool
RecTrackAlg.TrackerPositionedHits.Path = "TrackerPositionedHits"
RecTrackAlg.OutputLevel = DEBUG


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
nEvents = 5000
if args.nevents:
    nEvents = args.nevents

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [podioinput, RecTrackAlg, out],
                EvtSel = 'NONE',
                EvtMax   = nEvents,
                ExtSvc = [podioevent],
                OutputLevel=DEBUG,
 )
