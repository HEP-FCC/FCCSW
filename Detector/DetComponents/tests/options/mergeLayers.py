from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper

from Configurables import ParticleGunAlg, MomentumRangeParticleGun
pgun = MomentumRangeParticleGun("PGun",
                                PdgCodes=[11], # electron
                                MomentumMin = 20, # GeV
                                MomentumMax = 20, # GeV
                                ThetaMin = 1.58, # rad
                                ThetaMax = 1.58, # rad
                                PhiMin = 0, # rad
                                PhiMax = 0) # rad
gen = ParticleGunAlg("ParticleGun", ParticleGunTool=pgun, VertexSmearingToolPGun="FlatSmearVertex")
gen.DataOutputs.hepmc.Path = "hepmc"

from Configurables import Gaudi__ParticlePropertySvc
## Particle service
# list of possible particles is defined in ParticlePropertiesFile
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="Generation/data/ParticleTable.txt")

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc",  detectors=['file:Test/TestGeometry/data/Barrel_testCaloSD_rphiz.xml'],
                    OutputLevel = DEBUG)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc")

from Configurables import SimG4Alg, SimG4SaveCalHits, InspectHitsCollectionsTool, SimG4PrimariesFromEdmTool
inspecttool = InspectHitsCollectionsTool("inspect", readoutNames=["ECalHits"], OutputLevel = INFO)
savecaltool = SimG4SaveCalHits("saveECalHits", readoutNames = ["ECalHits"], OutputLevel = DEBUG)
savecaltool.DataOutputs.positionedCaloHits.Path = "positionedCaloHits"
savecaltool.DataOutputs.caloHits.Path = "caloHits"
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
geantsim = SimG4Alg("SimG4Alg", outputs= ["SimG4SaveCalHits/saveECalHits","InspectHitsCollectionsTool/inspect"], eventProvider=particle_converter)

from Configurables import MergeLayers
merge = MergeLayers("mergeLayers",
                    # common part of the name of the group of volumes to merge
                    volumeName = "slice",
                    # corresponding identifier in the readout for the volumes that should be merged
                    identifier = "z",
                    # readout - bitfield description from the geometry service
                    readout ="ECalHits",
                    # specify how many volumes to merge
                    # if it is a constant number (e.g. merge every second volume with previous one), 'MergeCells' can be used
                    # below: merge first 3k volumes into new one (id=0), next 10k into second one (id=1) and last 3k into third volume (id=2)
                    merge = [3000,10001,3000],
                    OutputLevel = DEBUG)
merge.DataInputs.inhits.Path = "caloHits"
merge.DataOutputs.outhits.Path = "newCaloHits"

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", filename="testMergeLayers.root")
out.outputCommands = ["keep *"]

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[gen, hepmc_converter, geantsim, merge, out],
               ExtSvc = [podiosvc, geoservice, geantservice, ppservice],
               OutputLevel=DEBUG)
