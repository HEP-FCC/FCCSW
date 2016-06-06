from Gaudi.Configuration import *

from Configurables import ParticleGunAlg, MomentumRangeParticleGun, Gaudi__ParticlePropertySvc
pgun = MomentumRangeParticleGun("PGun",
                                PdgCodes=[11], # electron
                                MomentumMin = 1, # GeV
                                MomentumMax = 1, # GeV
                                ThetaMin = -0.45, # rad
                                ThetaMax = -0.45, # rad
                                PhiMin = 1.6, # rad
                                PhiMax = 1.6) # rad
gen = ParticleGunAlg("ParticleGun", ParticleGunTool=pgun)
gen.DataOutputs.hepmc.Path = "hepmc"
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="../../../Generation/data/ParticleTable.txt")

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import HepMCDumper
hepmc_dump = HepMCDumper("hepmc")
hepmc_dump.DataInputs.hepmc.Path="hepmc"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:compact/Box_simpleTrackerSD.xml'], OutputLevel = DEBUG)

from Configurables import G4SimSvc
geantservice = G4SimSvc("G4SimSvc",
                        detector='G4DD4hepDetector',
                        physicslist="G4FtfpBert",
                        actions="G4FullSimActions")

from Configurables import G4SimAlg, G4SaveTrackerHits, G4PrimariesFromEdmTool
savetrackertool = G4SaveTrackerHits("G4SaveTrackerHits")
savetrackertool.DataOutputs.trackClusters.Path = "clusters"
savetrackertool.DataOutputs.trackHits.Path = "hits"
savetrackertool.DataOutputs.trackHitsClusters.Path = "hitClusterAssociation"

particle_converter = G4PrimariesFromEdmTool("EdmConverter")
particle_converter.DataInputs.genParticles.Path = "allGenParticles"
geantsim = G4SimAlg("G4SimAlg",
                    outputs=["G4SaveTrackerHits/G4SaveTrackerHits",
                             "InspectHitsCollectionsTool"],
                    eventGenerator=particle_converter)

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", OutputLevel=DEBUG, filename="out_simpleTrackerSD_2cm.root")
out.outputCommands = ["keep *"]


# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [gen, hepmc_converter, hepmc_dump, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podiosvc, ppservice, geoservice, geantservice],
                OutputLevel=DEBUG
 )
