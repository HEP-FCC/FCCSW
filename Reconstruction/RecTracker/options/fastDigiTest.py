import os
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")
from Configurables import PodioInput

from Configurables import GenAlg, MomentumRangeParticleGun
pgun_tool = MomentumRangeParticleGun(PdgCodes=[13], PhiMin=0., PhiMax=constants.pi*0.5, ThetaMin=constants.pi / 2., ThetaMax=constants.pi*0.9, MomentumMin=10000, MomentumMax=100000)
gen = GenAlg("ParticleGun", SignalProvider=pgun_tool)
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



from Configurables import FastGaussSmearDigi

fastdigi = FastGaussSmearDigi()
fastdigi.trackHits.Path = "hits"
fastdigi.smearedHits.Path = "smearedHits"
fastdigi.readoutName = "TrackerBarrelReadout"


# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename="fastDigi_Example.root"

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [gen, hepmc_converter, geantsim, fastdigi, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice, ppservice,],
                OutputLevel=DEBUG
 )
