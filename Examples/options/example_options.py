
### \file
### \ingroup BasicExamples
### | **input (alg)**                                      | other algorithms                   |                                                           |                                       | **output (alg)**                                |
### | ---------------------------------------------------- | ---------------------------------- | ---------------------                                     | ------------------------------------- | ----------------------------------------------- |
### | read events from a HepMC file                        | dump `HepMC::GenEvent`             | use dummy algorithm that creates EDM `fcc::JetCollection` |                                       | write the EDM output to ROOT file using PODIO   |

from Gaudi.Configuration import *

# data service
from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")

from Configurables import HepMCFileReader, GenAlg
readertool = HepMCFileReader("ReaderTool", Filename="/eos/project/f/fccsw-web/testsamples/FCC_minbias_100TeV.dat")
reader = GenAlg("Reader", SignalProvider=readertool)
# In the following line,
#   reader.YYY.Path = "XXX"
# YYY matches the string passed to declareOutput in the constructor of the algorithm (here "hepmc")
# XXX declares a name for the product (here the product is HepMC::GenEvent: "hepmcevent")
reader.hepmc.Path = "hepmcevent"

from Configurables import HepMCDumper
## dumps the HepMC::GenEvent
dumper = HepMCDumper("Dumper")
# the input product name matches the output product name of the previous module ("hepmcevent")
# the string "hepmc" is passed in the constructor of HepMCDumper in DeclareProperty method
#     note that it is only by coincidence the same as in HepMCReader
dumper.hepmc.Path="hepmcevent"

from Configurables import CreateSampleJet, PodioOutput
## dummy algorithm creating a sample jet
podiowrite = CreateSampleJet("SampleJet",OutputLevel=DEBUG)
podiowrite.podioJets.Path = "podioJets"

from Configurables import PodioOutput
## PODIO algorithm
out = PodioOutput("out", OutputLevel=DEBUG)

from Configurables import ApplicationMgr
##
ApplicationMgr(
    # all algorithms should be put here
    TopAlg=[reader, dumper, podiowrite, out],
    EvtSel='NONE',
    # number of events
    EvtMax=1,
    # all services should be put here
    ExtSvc=[podioevent],
    # possible: DEBUG, INFO, WARNING, ERROR, FATAL
    OutputLevel=INFO
)
