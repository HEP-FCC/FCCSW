from Gaudi.Configuration import *

from FCCPileupScenarios import FCCPhase1Pileup
from Configurables import ApplicationMgr, FCCDataSvc

albersevent   = FCCDataSvc("EventDataSvc")
# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import HepMCReader, HepMCDumper, PoissonPileUp, HepMCFileReader, FlatSmearVertex

pileupscenario = FCCPhase1Pileup('PileupScenario')
smeartool = FlatSmearVertex(
     **pileupscenario.VertexSmearingConfig)
genpileup = PoissonPileUp(name="Pileup", Filename="/afs/cern.ch/exp/fcc/sw/0.6/testsamples/example_MyPythia.dat",
    **pileupscenario.PileupToolConfig) 

reader = HepMCReader("Reader", 
    Filename="/afs/cern.ch/exp/fcc/sw/0.6/testsamples/example_MyPythia.dat",
    PileUpTool=genpileup,
    VertexSmearingTool = smeartool)
reader.DataOutputs.hepmc.Path = "hepmc"

# printout of the hepmc stuff
dumper = HepMCDumper()
dumper.DataInputs.hepmc.Path="hepmc"

ApplicationMgr(
    TopAlg = [reader, dumper],
    EvtSel = 'NONE',
    EvtMax   = 1,
    ExtSvc = [albersevent],
 )

