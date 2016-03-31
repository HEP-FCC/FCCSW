
### \file
### \ingroup BasicExamples
### | **input (alg)**                  | other algorithms                                    |                       |                                       | **output (alg)**                                |
### | ---------------------------------| ----------------------------------                  | --------------------- | ------------------------------------- | ----------------------------------------------- |
### | reading events from a HepMC file | load pileup configuration from external python dict | smear Vertices        | dump `HepMC::GenEvent`                | write the EDM output to ROOT file using PODIO   |


from Gaudi.Configuration import *

from Configurables import ApplicationMgr
from FCCPileupScenarios import FCCPhase1PileupConf as pileupconf

from Configurables import HepMCReader, HepMCDumper, PoissonPileUp, HepMCFileReader, FlatSmearVertex

from Configurables import FCCDataSvc
albersevent   = FCCDataSvc("EventDataSvc")

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

dumper = HepMCDumper()
dumper.DataInputs.hepmc.Path="hepmc"

ApplicationMgr(
    TopAlg=[reader, dumper],
    EvtSel='NONE',
    EvtMax=1,
    ExtSvc=[albersevent],
 )

