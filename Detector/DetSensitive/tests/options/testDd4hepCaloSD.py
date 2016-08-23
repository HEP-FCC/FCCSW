from Gaudi.Configuration import *

from Configurables import ParticleGunAlg, MomentumRangeParticleGun, Gaudi__ParticlePropertySvc
pgun = MomentumRangeParticleGun("PGun",
                                PdgCodes=[11], # electron
                                MomentumMin = 10, # GeV
                                MomentumMax = 10, # GeV
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
geoservice = GeoSvc("GeoSvc", detectors=['file:compact/Box_dd4hepCaloSD.xml'], OutputLevel = DEBUG)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc",
                        detector='SimG4DD4hepDetector',
                        physicslist="SimG4FtfpBert",
                        actions="SimG4FullSimActions")

from Configurables import SimG4Alg, SimG4SaveCalHits, SimG4PrimariesFromEdmTool, InspectHitsCollectionsTool
inspecttool = InspectHitsCollectionsTool("inspect", readoutNames=["ECalHits"], OutputLevel = DEBUG)

savehcaltool = SimG4SaveCalHits("saveECalHits", readoutNames = ["ECalHits"])
savehcaltool.DataOutputs.caloClusters.Path = "caloClusters"
savehcaltool.DataOutputs.caloHits.Path = "caloHits"

particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.DataInputs.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs=["SimG4SaveCalHits/saveECalHits",
                             "InspectHitsCollectionsTool/inspect"],
                    eventProvider=particle_converter)

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", OutputLevel=DEBUG, filename="out_dd4hepCaloSD_2cm.root")
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
