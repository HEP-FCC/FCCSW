
from GaudiKernel import SystemOfUnits as units
import argparse
simparser = argparse.ArgumentParser()
simparser.add_argument("--bFieldOff", action='store_true', help="Switch OFF magnetic field (default: B field ON)")

simparser.add_argument('-s','--seed', type=int, help='Seed for the random number generator', required=True)
simparser.add_argument('-N','--numEvents', type=int, help='Number of simulation events to run', required=True)
simparser.add_argument('--outName', type=str, help='Name of the output file', required=True)
simparser.add_argument('--tripletTracker', action="store_true", help="Use the triplet trigger tracker instead of the BaseLine")
simparser.add_argument('--pathToDetector', type=str, help='Name of the output file', default="")



genTypeGroup = simparser.add_mutually_exclusive_group(required = True) # Type of events to generate
genTypeGroup.add_argument("--singlePart", action='store_true', help="Single particle events")
genTypeGroup.add_argument("--pythia", action='store_true', help="Events generated with Pythia")
simparser.add_argument("--noVertexSmearTool", action='store_true', help="DONT use the Gaudi Vertex Smearing Tool")
simparser.add_argument("--displacedVertex", action='store_true', help="Smear the vertices in a test configuration")
simparser.add_argument("--geantinos", action="store_true", help="do geantino sim")

from math import pi
singlePartGroup = simparser.add_argument_group('Single particles')
import sys


singlePartGroup.add_argument('--pt',  type=float, nargs="+", help="list of pt values [MeV]!!!")
singlePartGroup.add_argument('--discretePtSpectrum', action="store_true", help='use continuous pt spectrum')
singlePartGroup.add_argument('--linSpacedPt', action="store_true", help='use linspaced pt spectrum')
singlePartGroup.add_argument('--ptMin', type=float, default=500., help='Minimal pt')
singlePartGroup.add_argument('--ptMax', type=float, default=100000000., help='Maximal pt')
singlePartGroup.add_argument('--etaMin', type=float, default=0., help='Minimal pseudorapidity')
singlePartGroup.add_argument('--etaMax', type=float, default=0., help='Maximal pseudorapidity')
singlePartGroup.add_argument('--phiMin', type=float, default=0, help='Minimal azimuthal angle')
singlePartGroup.add_argument('--phiMax', type=float, default=2*pi, help='Maximal azimuthal angle')
singlePartGroup.add_argument('--particle', type=int, required='--singlePart' in sys.argv, help='Particle type (PDG)')
singlePartGroup.add_argument('--trajectories', action="store_true", help='store trajectories for tracks, WARNING: big datavol!')

pythiaGroup = simparser.add_argument_group('Pythia','Common for min bias and LHE')
pythiaGroup.add_argument('-c', '--card', type=str, default='Generation/data/Pythia_minbias_pp_100TeV.cmd', help='Path to Pythia card')

simargs, _ = simparser.parse_known_args()

logSpacedPt = not simargs.linSpacedPt
ptList = []
if simargs.discretePtSpectrum:
  ptList=[1*units.GeV, 2*units.GeV,5 * units.GeV,10*units.GeV, 100*units.GeV, 1000*units.GeV, 10000*units.GeV]
  if simargs.pt:
    ptList = simargs.pt

print "=================================="
print "==      GENERAL SETTINGS       ==="
print "=================================="
magnetic_field = not simargs.bFieldOff
num_events = simargs.numEvents
seed = simargs.seed
output_name = simargs.outName
print "B field: ", magnetic_field
print "number of events = ", num_events
print "seed: ", seed
print "output name: ", output_name
if simargs.singlePart:
    etaMin = simargs.etaMin
    etaMax = simargs.etaMax
    phiMin = simargs.phiMin
    phiMax = simargs.phiMax
    pdg = simargs.particle
    particle_geant_names = {11: 'e-', -11: 'e+', -13: 'mu+', 13: 'mu-', 22: 'gamma', 111: 'pi0', 211: 'pi+', -211: 'pi-', 130: 'kaon0L', 0: "geantino"}
    print "=================================="
    print "==       SINGLE PARTICLES      ==="
    print "=================================="
    print "particle PDG, name: ", pdg, " ", particle_geant_names[pdg]
    if etaMin == etaMax:
        print "eta: ", etaMin
    else:
        print "eta: from ", etaMin, " to ", etaMax
    if phiMin == phiMax:
        print "phi: ", phiMin
    else:
        print "phi: from ", phiMin, " to ", phiMax
elif simargs.pythia:
    card = simargs.card
    print "=================================="
    print "==            PYTHIA           ==="
    print "=================================="
    print "card = ", card
print "=================================="


from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units
##############################################################################################################
#######                                         GEOMETRY                                         #############
##############################################################################################################
path_to_detector = simargs.pathToDetector

if simargs.tripletTracker:
  detectors_to_use=[path_to_detector+'Detector/DetFCChhTrackerTkLayout/compact/FCChh_DectEmptyTracker.xml',
                    path_to_detector+'Detector/DetFCChhTrackerTkLayout/triplet/FCCtriplet_1barrel30mm.xml',
                    ]
else:
  detectors_to_use=[path_to_detector+'Detector/DetFCChhTrackerTkLayout/compact/FCChh_DectEmptyTracker.xml',
                    path_to_detector+'Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                    ]

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors = detectors_to_use, OutputLevel = WARNING)

##############################################################################################################
#######                                        SIMULATION                                        #############
##############################################################################################################
# Setting random seed, will be propagated to Geant
from Configurables import  RndmGenSvc
from GaudiSvc.GaudiSvcConf import HepRndm__Engine_CLHEP__RanluxEngine_
randomEngine = eval('HepRndm__Engine_CLHEP__RanluxEngine_')
randomEngine = randomEngine('RndmGenSvc.Engine')
randomEngine.Seeds = [seed]

from Configurables import SimG4FullSimActions
actions = SimG4FullSimActions()
actions.enableHistory=True

# Magnetic field
from Configurables import SimG4ConstantMagneticFieldTool
if magnetic_field:
    field = SimG4ConstantMagneticFieldTool("bField", FieldOn=True, FieldZMax=20*units.m, IntegratorStepper="ClassicalRK4")
else:
    field = SimG4ConstantMagneticFieldTool("bField", FieldOn=False)

from Configurables import SimG4Svc
if simargs.geantinos:
  from Configurables import SimG4UserLimitRegion, SimG4UserLimitPhysicsList, SimG4GeantinoPhysicsList
  regiontool = SimG4UserLimitRegion("limits", volumeNames=["world"],
                                                     maxStep = 0.1*units.mm)
  
  physicslist = SimG4UserLimitPhysicsList("Physics", fullphysics="SimG4GeantinoPhysicsList")
  regions = [regiontool]
  #physicslist = SimG4GeantinoPhysicsList()
  #regions = []
else:
  from Configurables import SimG4UserLimitRegion, SimG4UserLimitPhysicsList
  regiontool = SimG4UserLimitRegion("limits", volumeNames=["world"],
                                                     maxStep = 0.1*units.mm)
  physicslist = SimG4UserLimitPhysicsList("Physics", fullphysics="SimG4FtfpBert")
  regions = [regiontool]
  



geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist=physicslist, regions=regions, actions=actions, magneticField=field)
# range cut
geantservice.g4PostInitCommands += ["/run/setCut 0.1 mm"]
if simargs.trajectories:
  geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"]

from Configurables import SimG4Alg, SimG4SingleParticleGeneratorTool, SimG4SaveTrackerHits, SimG4SaveParticleHistory

outputHitsTools = []
if simargs.trajectories:
  from Configurables import SimG4SaveTrajectory
  savetrajectorytool = SimG4SaveTrajectory("saveTrajectory")
  savetrajectorytool.trajectory.Path = "trajectory"
  savetrajectorytool.trajectoryPoints.Path = "trajectoryPoints"
  outputHitsTools += [ "SimG4SaveTrajectory/saveTrajectory"]

savehisttool = SimG4SaveParticleHistory("saveHistory")
savehisttool.mcParticles.Path = "SimParticles"
savehisttool.genVertices.Path = "SimVertices"

savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"]) 
savetrackertool.positionedTrackHits.Path = "TrackerPositionedHits"
savetrackertool.trackHits.Path = "TrackerHits"
savetrackertool.digiTrackHits.Path = "TrackerDigiPostPoint"
outputHitsTools += [ "SimG4SaveTrackerHits/saveTrackerHits"]
outputHitsTools += [ "SimG4SaveParticleHistory/saveHistory"]


geantsim = SimG4Alg("SimG4Alg", outputs = outputHitsTools)

from Configurables import FlatSmearVertex, ConstPtParticleGun, GenAlg, GaussSmearVertex
smeartool = GaussSmearVertex("GaussSmearVertex")
if not simargs.noVertexSmearTool:
  smeartool.xVertexSigma = 0.5*units.mm
  smeartool.yVertexSigma = 0.5*units.mm
  smeartool.zVertexSigma = 40*units.mm
  smeartool.tVertexSigma = 180*units.picosecond
elif simargs.displacedVertex:
  smeartool.xVertexMin = 0*units.mm
  smeartool.xVertexMax = 600*units.mm


if simargs.singlePart:
    pgun_tool = ConstPtParticleGun(PdgCodes=[pdg], PhiMin=phiMin, PhiMax=phiMax, EtaMin=etaMin, EtaMax=etaMax, PtList=ptList, PtMin=simargs.ptMin, PtMax=simargs.ptMax, logSpacedPt=logSpacedPt )
    genalg= GenAlg("Pythia8", SignalProvider=pgun_tool, VertexSmearingTool=smeartool)
    genalg.hepmc.Path = "hepmc"
else:
    from Configurables import PythiaInterface, GenAlg, GaussSmearVertex

    pythia8gentool = PythiaInterface("Pythia8",Filename=card)
    genalg= GenAlg("Pythia8", SignalProvider=pythia8gentool, VertexSmearingTool=smeartool)
    genalg.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="GenVertices"
from Configurables import GenParticleFilter
### Filters generated particles
# accept is a list of particle statuses that should be accepted
genfilter = GenParticleFilter("StableParticles", accept=[1], OutputLevel=DEBUG)
genfilter.allGenParticles.Path = "allGenParticles"
genfilter.filteredGenParticles.Path = "GenParticles"
from Configurables import SimG4PrimariesFromEdmTool
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "GenParticles"
if simargs.geantinos:
  from Configurables import SimG4GeantinosFromEdmTool
  geantino_converter = SimG4GeantinosFromEdmTool()
  geantsim.eventProvider = geantino_converter
else:
  geantsim.eventProvider = particle_converter






# PODIO algorithm
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput
podioevent = FCCDataSvc("EventDataSvc")
out = PodioOutput("out")

out.outputCommands = ["keep *"]
out.filename = output_name

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True
out.AuditExecute = True


list_of_algorithms = [genalg, hepmc_converter, genfilter, geantsim,  out]

ApplicationMgr(
    TopAlg = list_of_algorithms,
    EvtSel = 'NONE',
    EvtMax   = num_events,
    ExtSvc = [podioevent, geoservice, geantservice],
    OutputLevel=INFO,
)

