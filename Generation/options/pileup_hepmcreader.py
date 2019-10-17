
### \file
### \ingroup BasicExamples
### | **input (alg)**                  | other algorithms                                    |                       |                                       | **output (alg)**                                |
### | ---------------------------------| ----------------------------------                  | --------------------- | ------------------------------------- | ----------------------------------------------- |
### | reading events from a HepMC file | load pileup configuration from external python dict | smear Vertices        | dump `HepMC::GenEvent`                | write the EDM output to ROOT file using PODIO   |


from Gaudi.Configuration import *

from Configurables import ApplicationMgr
from FCCPileupScenarios import FCCPhase1PileupConf as pileupconf

from Configurables import HepMCDumper, PoissonPileUp, HepMCFileReader, FlatSmearVertex, HepMCFullMerge, GenAlg

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
    numPileUpEvents=pileupconf['numPileUpEvents'])

mergetool = HepMCFullMerge()

readertool = HepMCFileReader("ReaderTool", 
    Filename="/eos/project/f/fccsw-web/testsamples/FCC_minbias_100TeV.dat")
readertool_pileup = HepMCFileReader("ReaderToolPileup", 
    Filename="/eos/project/f/fccsw-web/testsamples/FCC_minbias_100TeV.dat")
reader = GenAlg("Reader", SignalProvider=readertool, PileUpProvider=readertool_pileup, PileUpTool=genpileup, HepMCMergeTool=mergetool, VertexSmearingTool=smeartool)
reader.hepmc.Path = "hepmc"

dumper = HepMCDumper()
dumper.hepmc.Path="hepmc"

ApplicationMgr(
    TopAlg=[reader, dumper],
    EvtSel='NONE',
    EvtMax=1,
    ExtSvc=[albersevent],
 )

