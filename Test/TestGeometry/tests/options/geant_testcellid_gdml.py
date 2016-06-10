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

from Configurables import G4SimSvc, G4GdmlTestDetector
det = G4GdmlTestDetector("G4GdmlTestDetector", gdml = "../data/TestBoxCaloSD.gdml")
geantservice = G4SimSvc("G4SimSvc", detector=det, physicslist='G4TestPhysicsList', actions='G4FullSimActions')

from Configurables import G4SimAlg, G4SaveTestCalHits
savecaltool = G4SaveTestCalHits("saveECalHits", caloType = "ECal", OutputLevel = DEBUG)
savecaltool.DataOutputs.caloClusters.Path = "caloClusters"
savecaltool.DataOutputs.caloHits.Path = "caloHits"
geantsim = G4SimAlg("G4SimAlg", outputs= ["G4SaveTestCalHits/saveECalHits","InspectHitsCollectionsTool"])

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", filename="testCellId_gdml.root")
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, hepmc_converter, hepmc_dump, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 32,
                ExtSvc = [podiosvc, geantservice],
                OutputLevel=INFO
 )
