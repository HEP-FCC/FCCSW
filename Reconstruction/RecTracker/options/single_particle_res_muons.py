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

from Configurables import ConstPileUp

pileuptool = ConstPileUp(numPileUpEvents=0)
pgun_tool = ConstPtParticleGun(PdgCodes=[13], PhiMin=0., PhiMax=constants.pi*0.5, EtaMin=0, EtaMax=5, PtList=[1*units.GeV, 2*units.GeV,5 * units.GeV,10*units.GeV, 100*units.GeV, 1000*units.GeV, 10000*units.GeV])
#pgun_tool2 = ConstPtParticleGun(PdgCodes=[13], PhiMin=0., PhiMax=constants.pi*0.5, EtaMin=-0.3, EtaMax=0.3, PtList=[5 * units.GeV,10*units.GeV, 100*units.GeV])

gen = GenAlg("ParticleGun", PileUpTool=pileuptool, SignalProvider=pgun_tool, VertexSmearingTool="FlatSmearVertex")
gen.hepmc.Path = "hepmc"

from Configurables import Gaudi__ParticlePropertySvc
# Particle service
# list of possible particles is defined in ParticlePropertiesFile
ppservice = Gaudi__ParticlePropertySvc(
    "ParticlePropertySvc", ParticlePropertiesFile="Generation/data/ParticleTable.txt")

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'],
                    OutputLevel = INFO)

from Configurables import HepMCToEDMConverter
## Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc, SimG4FullSimActions
actions = SimG4FullSimActions()
actions.enableHistory=True
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector',
                        physicslist="SimG4FtfpBert", actions=actions)

geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"]
from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool(
    "SimG4ConstantMagneticFieldTool", FieldOn=True, IntegratorStepper="ClassicalRK4")

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs
# via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool, SimG4SaveParticleHistory, SimG4SaveTrajectory
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savetrajectorytool = SimG4SaveTrajectory("saveTrajectory")
savetrajectorytool.trajectory.Path = "trajectory"
savetrajectorytool.trajectoryPoints.Path = "trajectoryPoints"
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"

savehisttool = SimG4SaveParticleHistory("saveHistory")
savehisttool.mcParticles.Path = "simParticles"
savehisttool.genVertices.Path = "simVertices"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs=["SimG4SaveTrackerHits/saveTrackerHits",
                             "SimG4SaveTrajectory/saveTrajectory",
                             "SimG4SaveParticleHistory/saveHistory"
                             ],
                    eventProvider=particle_converter)

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename = "muons_for_seeding_single.root"

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr(TopAlg=[gen, hepmc_converter, geantsim, out],
               EvtSel='NONE',
               EvtMax=5000,
               # order is important, as GeoSvc is needed by SimG4Svc
               ExtSvc=[ppservice, podioevent, geoservice, geantservice],
               OutputLevel=DEBUG
               )
