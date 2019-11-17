from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCDumper


from Configurables import MomentumRangeParticleGun
from GaudiKernel import PhysicalConstants as constants
guntool = MomentumRangeParticleGun("Gun2")
guntool.ThetaMin = 0 
guntool.ThetaMax = 2 * constants.pi 
guntool.PdgCodes = [11]
guntool2 = MomentumRangeParticleGun("Gun1")
guntool2.ThetaMin = 0 
guntool2.ThetaMax = 2 * constants.pi 
guntool2.PdgCodes = [11]
from Configurables import ConstPileUp
pileuptool = ConstPileUp()
pileuptool.numPileUpEvents = 10
from Configurables import GenAlg
gen = GenAlg()
gen.SignalProvider=guntool
gen.PileUpProvider=guntool2
gen.PileUpTool=pileuptool
gen.hepmc.Path = "hepmc"

 

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

dumper = HepMCDumper("Dumper")
dumper.hepmc.Path="hepmc"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Test/TestGeometry/data/TestBoxCaloSD_3readouts.xml'], OutputLevel = DEBUG)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", physicslist='SimG4TestPhysicsList')

from Configurables import SimG4Alg, SimG4SaveCalHits, InspectHitsCollectionsTool
inspecttool = InspectHitsCollectionsTool("inspect", readoutNames=["ECalHits"], OutputLevel = DEBUG)
savecaltool = SimG4SaveCalHits("saveECalHits", readoutNames = ["ECalHits"], OutputLevel = DEBUG)
savecaltool.positionedCaloHits.Path = "CaloHitsPositions"
savecaltool.caloHits.Path = "CaloHits"
geantsim = SimG4Alg("SimG4Alg", outputs= ["SimG4SaveCalHits/saveECalHits","InspectHitsCollectionsTool/inspect"])

from Configurables import MergeCells
merge = MergeCells("mergeCells",
                   # take the bitfield description from the geometry service
                   readout ="ECalHits",
                   # cells in which field should be merged
                   identifier = "x",
                   # how many cells to merge
                   # for signed fields (segmentation cells) this needs to be odd to keep middle cell centred in 0
                   # for unsigned field (volumes) this may be any number
                   merge = 3,
                   OutputLevel = DEBUG)
merge.inhits.Path = "CaloHits"
merge.outhits.Path = "CaloHitsNew"

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", filename="output_test_mergeCells.root")
out.outputCommands = ["keep *"]

ApplicationMgr(EvtSel='NONE',
               EvtMax=30,
               TopAlg=[gen, hepmc_converter, geantsim, merge, out],
               ExtSvc = [podiosvc, geoservice, geantservice],
               OutputLevel=INFO)
