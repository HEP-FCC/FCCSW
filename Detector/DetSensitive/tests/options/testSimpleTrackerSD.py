from Gaudi.Configuration import *

from Configurables import GenAlg, MomentumRangeParticleGun, Gaudi__ParticlePropertySvc
pgun = MomentumRangeParticleGun("PGun",
                                PdgCodes=[11], # electron
                                MomentumMin = 1, # GeV
                                MomentumMax = 1, # GeV
                                ThetaMin = -0.45, # rad
                                ThetaMax = -0.45, # rad
                                PhiMin = 1.6, # rad
                                PhiMax = 1.6) # rad
gen = GenAlg("ParticleGun", SignalProvider=pgun)
gen.hepmc.Path = "hepmc"
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="../../../Generation/data/ParticleTable.txt")

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import HepMCDumper
hepmc_dump = HepMCDumper("hepmc")
hepmc_dump.hepmc.Path="hepmc"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:compact/Box_simpleTrackerSD.xml'], OutputLevel = DEBUG)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc",
                        detector='SimG4DD4hepDetector',
                        physicslist="SimG4FtfpBert",
                        actions="SimG4FullSimActions")

from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool, InspectHitsCollectionsTool
inspecttool = InspectHitsCollectionsTool("inspect", readoutNames=["TrackerSiliconHits"], OutputLevel = DEBUG)

savetrackertool = SimG4SaveTrackerHits("SimG4SaveTrackerHits", readoutNames = ["TrackerSiliconHits"])
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"

particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs=["SimG4SaveTrackerHits/SimG4SaveTrackerHits",
                             "InspectHitsCollectionsTool/inspect"],
                    eventProvider=particle_converter)

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", OutputLevel=DEBUG, filename="out_simpleTrackerSD_2cm.root")
out.outputCommands = ["keep *"]


# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [gen, hepmc_converter, hepmc_dump, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podiosvc, ppservice, geoservice, geantservice],
                OutputLevel=DEBUG
 )
