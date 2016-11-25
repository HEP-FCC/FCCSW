from Gaudi.Configuration import *

pythiafile="Generation/data/Pythia_standard.cmd"

from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import PythiaInterface
pythia8gen = PythiaInterface("Pythia8Interface", Filename=pythiafile)
pythia8gen.DataOutputs.hepmc.Path = "hepmcevent"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmcevent"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import PodioOutput
### PODIO algorithm
out = PodioOutput("out", OutputLevel=DEBUG)
out.filename = "pythia_test.root"
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[ pythia8gen, hepmc_converter, out ],
                EvtSel='NONE',
                EvtMax=5,
                ExtSvc=[podioevent],
                OutputLevel=DEBUG
)

