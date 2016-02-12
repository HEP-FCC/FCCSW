from Gaudi.Configuration import *

# data service
from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import HepMCReader
reader = HepMCReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.6/testsamples/example_MyPythia.dat")
# In the following line,
#   reader.DataOutputs.YYY.Path = "XXX"
# YYY matches the string passed to declareOutput in the constructor of the algorithm (here "hepmc")
# XXX declares a name for the product (here the product is HepMC::GenEvent: "hepmcevent")
reader.DataOutputs.hepmc.Path = "hepmcevent"

# dumps the HepMC::GenEvent
from Configurables import HepMCDumper
dumper = HepMCDumper("Dumper")
# the input product name matches the output product name of the previous module ("hepmcevent")
# the string "hepmc" is passed in the constructor of HepMCDumper in DeclareProperty method
#     note that it is only by coincidence the same as in HepMCReader
dumper.DataInputs.hepmc.Path="hepmcevent"

# dummy algorithm creating a sample jet
from Configurables import CreateSampleJet, PodioOutput
podiowrite = CreateSampleJet("SampleJet",OutputLevel=DEBUG)
podiowrite.DataOutputs.podioJets.Path = "podioJets"

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out", OutputLevel=DEBUG)

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr(
    # all algorithms should be put here
    TopAlg = [reader,dumper,podiowrite,out],
    EvtSel = 'NONE',
    # number of events
    EvtMax = 1,
    # all services should be put here
    ExtSvc = [podioevent],
    # possible: DEBUG, INFO, WARNING, ERROR, FATAL
    OutputLevel=INFO
)
