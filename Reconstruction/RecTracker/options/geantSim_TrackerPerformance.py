
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
genTypeGroup.add_argument("--useVertexSmearTool", action='store_true', help="Use the Gaudi Vertex Smearing Tool")

from math import pi
singlePartGroup = simparser.add_argument_group('Single particles')
import sys


singlePartGroup.add_argument('--pt',  type=float, nargs="+", help="list of pt values [MeV]!!!")
ptList=[1*units.GeV, 2*units.GeV,5 * units.GeV,10*units.GeV, 100*units.GeV, 1000*units.GeV, 10000*units.GeV]

singlePartGroup.add_argument('--energy', type=float, default=1000., help='energy')
singlePartGroup.add_argument('--etaMin', type=float, default=0., help='Minimal pseudorapidity')
singlePartGroup.add_argument('--etaMax', type=float, default=0., help='Maximal pseudorapidity')
singlePartGroup.add_argument('--phiMin', type=float, default=0, help='Minimal azimuthal angle')
singlePartGroup.add_argument('--phiMax', type=float, default=2*pi, help='Maximal azimuthal angle')
singlePartGroup.add_argument('--particle', type=int, required='--singlePart' in sys.argv, help='Particle type (PDG)')

pythiaGroup = simparser.add_argument_group('Pythia','Common for min bias and LHE')
pythiaGroup.add_argument('-c', '--card', type=str, default='Generation/data/Pythia_minbias_pp_100TeV.cmd', help='Path to Pythia card (default: PythiaCards/default.cmd)')

simargs, _ = simparser.parse_known_args()
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
    energy = simargs.energy
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
    print "energy: ", energy, "GeV"
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
path_to_detector = simargs.pathToDetector #"/afs/cern.ch/work/v/vavolkl/public/fcc.cern.ch/sw/pre0.9.1/FCCSW/InstallArea/" #TODO: update with cvmfs installation

if simargs.tripletTracker:
  detectors_to_use=[path_to_detector+'Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                    path_to_detector+'Detector/DetFCChhTrackerTkLayout/triplet/FCCtriplet_1barrel30mm.xml',
                    ]
else:
  detectors_to_use=[path_to_detector+'Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
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

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions=actions)
# range cut
geantservice.g4PostInitCommands += ["/run/setCut 0.1 mm"]

from Configurables import SimG4Alg, SimG4SingleParticleGeneratorTool, SimG4SaveTrackerHits, SimG4SaveParticleHistory

savehisttool = SimG4SaveParticleHistory("saveHistory")
savehisttool.mcParticles.Path = "SimParticles"
savehisttool.genVertices.Path = "SimVertices"

savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"]) 
savetrackertool.positionedTrackHits.Path = "TrackerPositionedHits"
savetrackertool.trackHits.Path = "TrackerHits"
savetrackertool.digiTrackHits.Path = "TrackerDigiPostPoint"
outputHitsTools = []
outputHitsTools += [ "SimG4SaveTrackerHits/saveTrackerHits"]
outputHitsTools += [ "SimG4SaveParticleHistory/saveHistory"]

geantsim = SimG4Alg("SimG4Alg", outputs = outputHitsTools)

from Configurables import GaussSmearVertex, ConstPtParticleGun, GenAlg
smeartool = GaussSmearVertex("GaussSmearVertex")
if simargs.useVertexSmearTool:
  smeartool.xVertexSigma = 0.5*units.mm
  smeartool.yVertexSigma = 0.5*units.mm
  smeartool.zVertexSigma = 40*units.mm
  smeartool.tVertexSigma = 180*units.picosecond

if simargs.singlePart:
    pgun_tool = ConstPtParticleGun(PdgCodes=[pdg], PhiMin=phiMin, PhiMax=phiMax, EtaMin=etaMin, EtaMax=etaMax, PtList=ptList)
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
geantsim.eventProvider = particle_converter

# Magnetic field
from Configurables import SimG4ConstantMagneticFieldTool
if magnetic_field:
    field = SimG4ConstantMagneticFieldTool("bField", FieldOn=True, FieldZMax=20*units.m, IntegratorStepper="ClassicalRK4")
else:
    field = SimG4ConstantMagneticFieldTool("bField", FieldOn=False)





# PODIO algorithm
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput
podioevent = FCCDataSvc("EventDataSvc")
out = PodioOutput("out")
out.outputCommands = ["drop *",
                      "keep GenParticles",
                      "keep GenVertices",
                      "keep SimParticles",
                      "keep SimVertices",
                      "keep TrackerHits",
                      "keep TrackerPositionedHits",
                      "keep TrackerDigiPostPoint",
                      ]
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
)

