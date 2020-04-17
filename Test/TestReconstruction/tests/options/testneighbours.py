from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCFileReader, GenAlg, HepMCDumper

# reads HepMC text file and write the HepMC::GenEvent to the data service
readertool = HepMCFileReader()
readertool.Filename = "Test/TestGeometry/data/testHepMCrandom.dat"
reader = GenAlg("Reader")
reader.SignalProvider = readertool
reader.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc")
geoservice.detectors = ['file:Test/TestGeometry/data/TestBoxCaloSD_segmentation.xml']

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc")
geantservice.detector='SimG4DD4hepDetector'
geantservice.physicslist='SimG4TestPhysicsList'
geantservice.actions='SimG4FullSimActions'

from Configurables import SimG4SaveCalHits
savecaltool = SimG4SaveCalHits("saveECalHits")
savecaltool.readoutNames = ["ECalHits"]
savecaltool.positionedCaloHits.Path = "positionedCaloHits"
savecaltool.caloHits.Path = "caloHits"
from Configurables import SimG4Alg
geantsim = SimG4Alg("SimG4Alg")
geantsim.outputs = [
                    "SimG4SaveCalHits/saveECalHits",
                    "InspectHitsCollectionsTool",
                    ]

from Configurables import TestNeighbours
neighbours = TestNeighbours("dbgNeighb")
neighbours.readout = 'ECalHits'
neighbours.inhits.Path = "caloHits"

from Configurables import FCCDataSvc
podiosvc = FCCDataSvc("EventDataSvc")
from Configurables import PodioOutput
out = PodioOutput("out")
out.filename = "testCellIdNeighbours.root"
out.outputCommands = ["keep *"]

# ApplicationMgr
ApplicationMgr(EvtSel='NONE',
               EvtMax=10,
               TopAlg=[reader, hepmc_converter, geantsim, neighbours, out],
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podiosvc, geoservice, geantservice],
                OutputLevel=DEBUG)
