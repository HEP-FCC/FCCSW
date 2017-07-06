import os
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")
from Configurables import PodioInput

from Configurables import GenAlg, MomentumRangeParticleGun, PoissonPileUp 
pileuptool = PoissonPileUp("PileUp", numPileUpEvents=12)
pgun_tool = MomentumRangeParticleGun(PdgCodes=[13], PhiMin=0., PhiMax=constants.pi*0.5, ThetaMin=constants.pi / 2., ThetaMax=constants.pi*0.9, MomentumMin=10000, MomentumMax=100000)
pgun_tool2 = MomentumRangeParticleGun(PdgCodes=[13], PhiMin=0., PhiMax=constants.pi*0.5, ThetaMin=constants.pi / 2., ThetaMax=constants.pi*0.9, MomentumMin=10000, MomentumMax=100000)
gen = GenAlg("ParticleGun", SignalProvider=pgun_tool, PileUpProvider=pgun_tool2, PileUpTool=pileuptool)
gen.hepmc.Path = "hepmc"

from Configurables import Gaudi__ParticlePropertySvc
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="Generation/data/ParticleTable.txt")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
  ],
  )

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, IntegratorStepper="ClassicalRK4")

from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["TrackerBarrelReadout"])
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/saveTrackerHits"],
                    eventProvider=particle_converter)



from Configurables import CombinatorialSeedingTest, CombinatorialSeedingTool

seed_tool = CombinatorialSeedingTool()
seed_tool.seedingLayerIndices0=(0,0)
seed_tool.seedingLayerIndices1=(0,1)
seed_tool.seedingLayerIndices2=(0,2)
seed_tool.trackSeeds.Path = "trackSeeds"
seed_tool.readoutName = "TrackerBarrelReadout"

combi_seeding = CombinatorialSeedingTest()
combi_seeding.TrackSeedingTool = seed_tool
combi_seeding.positionedTrackHits.Path = "positionedHits"


# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename="combiSeeding_Example.root"

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [gen, hepmc_converter, geantsim, combi_seeding, out],
                EvtSel = 'NONE',
                EvtMax   = 2,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice, ppservice,],
                OutputLevel=DEBUG
 )
