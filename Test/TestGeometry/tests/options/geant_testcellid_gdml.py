from Gaudi.Configuration import *

from Configurables import HepMCReader
reader = HepMCReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.7/testsamples/testHepMCborders.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import HepMCDumper
hepmc_dump = HepMCDumper("hepmc")
hepmc_dump.DataInputs.hepmc.Path="hepmc"

from Configurables import SimG4Svc, SimG4GdmlTestDetector
det = SimG4GdmlTestDetector("SimG4GdmlTestDetector", gdml = "../data/TestBoxCaloSD.gdml")
geantservice = SimG4Svc("SimG4Svc", detector=det, physicslist='SimG4TestPhysicsList', actions='SimG4FullSimActions')

from Configurables import SimG4Alg, SimG4SaveTestCalHits
savecaltool = SimG4SaveTestCalHits("saveECalHits")
savecaltool.DataOutputs.caloClusters.Path = "caloClusters"
savecaltool.DataOutputs.caloHits.Path = "caloHits"
geantsim = SimG4Alg("SimG4Alg", outputs= ["SimG4SaveTestCalHits/saveECalHits"])

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
