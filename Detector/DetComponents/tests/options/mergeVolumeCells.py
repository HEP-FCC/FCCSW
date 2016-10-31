from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper

from Configurables import ParticleGunAlg, MomentumRangeParticleGun
pgun = MomentumRangeParticleGun("PGun",
                                PdgCodes=[11], # electron
                                MomentumMin = 20, # GeV
                                MomentumMax = 20, # GeV
                                ThetaMin = 90., # rad
                                ThetaMax = 90., # rad
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
geoservice = GeoSvc("GeoSvc",  detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Mockup.xml' ],
                    OutputLevel = DEBUG)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc")

from Configurables import SimG4Alg, SimG4SaveCalHits, InspectHitsCollectionsTool, SimG4PrimariesFromEdmTool
inspecttool = InspectHitsCollectionsTool("inspect", readoutNames=["ECalHitsPhiEta"], OutputLevel = INFO)
savecaltool = SimG4SaveCalHits("saveECalHits", readoutNames = ["ECalHitsPhiEta"], OutputLevel = DEBUG)
savecaltool.DataOutputs.caloClusters.Path = "caloClusters"
savecaltool.DataOutputs.caloHits.Path = "caloHits"
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
geantsim = SimG4Alg("SimG4Alg", outputs= ["SimG4SaveCalHits/saveECalHits","InspectHitsCollectionsTool/inspect"], eventProvider=particle_converter)

from Configurables import MergeVolumeCells
merge = MergeVolumeCells("mergeVolumeCells",
                   # take the bitfield description from the geometry service
                   readout ="ECalHitsPhiEta",
                   # cells in which field should be merged
                   identifier = "active_layer",
                   # how many cells to merge
                   # merge first 20 into new cell (id=0), next 50 into second cell (id=1), ...
                   merge = [20,50,47],
                   volumeName = "LAr",
                   OutputLevel = DEBUG)
merge.DataInputs.inhits.Path = "caloHits"
merge.DataOutputs.outhits.Path = "newCaloHits"

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", filename="testMergeCells.root")
out.outputCommands = ["keep *"]

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[gen, hepmc_converter, geantsim, merge, out],
               ExtSvc = [podiosvc, geoservice, geantservice, ppservice],
               OutputLevel=DEBUG)
