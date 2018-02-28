from Gaudi.Configuration import *

from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")
from Configurables import GenAlg, ConstPtParticleGun
## Particle Gun using MomentumRangeParticleGun tool and FlatSmearVertex
# MomentumRangeParticleGun generates particles of given type(s) within given momentum, phi and theta range
# FlatSmearVertex smears the vertex with uniform distribution

from Configurables import ConstPileUp, GaussSmearVertex
smeartool = GaussSmearVertex("smeartoolname")

smeartool.tVertexSigma = 150*units.cm

pileuptool = ConstPileUp(numPileUpEvents=4)
pgun_tool = ConstPtParticleGun(PdgCodes=[123456], PhiMin=0., PhiMax=constants.pi*0.5, EtaMin=-0.5, EtaMax=0.5, PtList=[ 100*units.GeV])
pgun_tool2 = ConstPtParticleGun(PdgCodes=[123456], PhiMin=0., PhiMax=constants.pi*0.5, EtaMin=-0.5, EtaMax=0.5, PtList=[ 100*units.GeV])

gen = GenAlg("ParticleGun", PileUpTool=pileuptool, SignalProvider=pgun_tool, PileUpProvider=pgun_tool2, VertexSmearingTool=smeartool)
gen.hepmc.Path = "hepmc"
from Configurables import HepMCToEDMConverter
## Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc, SimG4SingleParticleGeneratorTool
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'],
                    OutputLevel = DEBUG)


# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4GeantinoDeposits", actions="SimG4FullSimActions")


from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, IntegratorStepper="ClassicalRK4")


from Configurables import SimG4SaveTrajectory 
savetrajectorytool = SimG4SaveTrajectory("saveTrajectory")
savetrajectorytool.trajectory.Path = "trajectory"
savetrajectorytool.trajectoryPoints.Path = "trajectoryPoints"


geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
#pgun = SimG4SingleParticleGeneratorTool("GeantinoGun", phiMin=0, phiMax=constants.pi*2, etaMin=0, etaMax=5, PtList=[1000, 2000, 5000, 10000, 100000, 1000000, 1e7 ], energyMin=100000, energyMax=100000, particleName="chargedgeantino", saveEdm=True)
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs= ["SimG4SaveTrackerHits/saveTrackerHits", "SimG4SaveTrajectory/saveTrajectory"],
                    eventProvider=particle_converter)

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename = "geantinosForSeeding2.root"

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [gen, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 100,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
