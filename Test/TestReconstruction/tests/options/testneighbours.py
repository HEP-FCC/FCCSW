from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCFileReader, GenAlg, HepMCDumper

# reads HepMC text file and write the HepMC::GenEvent to the data service
readertool = HepMCFileReader("ReaderTool", Filename="/eos/project/f/fccsw-web/testsamples/testHepMCrandom.dat")
reader = GenAlg("Reader", SignalProvider=readertool)
reader.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:../../../Test/TestGeometry/data/TestBoxCaloSD_segmentation.xml'], OutputLevel = DEBUG)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist='SimG4TestPhysicsList', actions='SimG4FullSimActions')

from Configurables import SimG4Alg, SimG4SaveCalHits
savecaltool = SimG4SaveCalHits("saveECalHits", readoutNames = ["ECalHits"], OutputLevel = DEBUG)
savecaltool.positionedCaloHits.Path = "positionedCaloHits"
savecaltool.caloHits.Path = "caloHits"
geantsim = SimG4Alg("SimG4Alg", outputs= ["SimG4SaveCalHits/saveECalHits","InspectHitsCollectionsTool"])

from Configurables import TestNeighbours
neighbours = TestNeighbours("dbgNeighb", readout = 'ECalHits', OutputLevel = DEBUG)
neighbours.inhits.Path = "caloHits"

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", filename="testCellIdNeighbours.root")
out.outputCommands = ["keep *"]

# ApplicationMgr
ApplicationMgr(EvtSel='NONE',
               EvtMax=10,
               TopAlg=[reader, hepmc_converter, geantsim, neighbours, out],
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podiosvc, geoservice, geantservice],
                OutputLevel=DEBUG)
