from Gaudi.Configuration import *

from Configurables import HepMCReader
reader = HepMCReader("Reader", Filename="../data/testHepMC.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import HepMCDumper
hepmc_dump = HepMCDumper("hepmc")
hepmc_dump.DataInputs.hepmc.Path="hepmc"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:../data/TestBoxCaloSD_segmentation.xml'], OutputLevel = DEBUG)

from Configurables import G4SimSvc
geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist='G4TestPhysicsList', actions='G4FullSimActions')

from Configurables import G4SimAlg, G4SaveCalHits
savecaltool = G4SaveCalHits("saveECalHits", caloType = "ECal", OutputLevel = DEBUG)
savecaltool.DataOutputs.caloClusters.Path = "caloClusters"
savecaltool.DataOutputs.caloHits.Path = "caloHits"
geantsim = G4SimAlg("G4SimAlg", outputs= ["G4SaveCalHits/saveECalHits","InspectHitsCollectionsTool"])

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", filename="testCellId_dd4hep_segmentation.root")
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, hepmc_converter, hepmc_dump, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 32,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podiosvc, geoservice, geantservice],
                OutputLevel=INFO
 )
