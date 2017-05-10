from Gaudi.Configuration import *

from Configurables import HepMCFileReader, GenAlg
readertool = HepMCFileReader("ReaderTool", Filename="/eos/project/f/fccsw-web/testsamples/testHepMCborders.dat")
reader = GenAlg("Reader", SignalProvider=readertool)
reader.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import SimG4Svc, SimG4GdmlTestDetector
det = SimG4GdmlTestDetector("SimG4GdmlTestDetector", gdml = "../data/TestBoxCaloSD.gdml")
geantservice = SimG4Svc("SimG4Svc", detector=det, physicslist='SimG4TestPhysicsList', actions='SimG4FullSimActions')

from Configurables import SimG4Alg, SimG4SaveTestCalHits
savecaltool = SimG4SaveTestCalHits("saveECalHits")
savecaltool.caloClusters.Path = "caloClusters"
savecaltool.caloHits.Path = "caloHits"
geantsim = SimG4Alg("SimG4Alg", outputs= ["SimG4SaveTestCalHits/saveECalHits"])

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", filename="testCellId_gdml.root")
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 32,
                ExtSvc = [podiosvc, geantservice],
                OutputLevel=INFO
 )
