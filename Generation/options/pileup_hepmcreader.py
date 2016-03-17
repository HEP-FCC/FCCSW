from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc
from FCCPileupScenarios import FCCPhase1PileupConf as pileupconf

albersevent   = FCCDataSvc("EventDataSvc")
# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import HepMCReader, HepMCDumper, PoissonPileUp, HepMCFileReader, FlatSmearVertex

smeartool = FlatSmearVertex(
     xVertexMin=pileupconf['xVertexMin'],
     xVertexMax=pileupconf['xVertexMax'],
     yVertexMin=pileupconf['yVertexMin'],
     yVertexMax=pileupconf['yVertexMax'],
     zVertexMin=pileupconf['zVertexMin'],
     zVertexMax=pileupconf['zVertexMax'])

genpileup = PoissonPileUp(name="Pileup", 
    Filename="/afs/cern.ch/exp/fcc/sw/0.6/testsamples/example_MyPythia.dat",
    numPileUpEvents=pileupconf['numPileUpEvents']) 

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

