from Gaudi.Configuration import *

pythiafile="Generation/data/Pythia_standard.cmd"

from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import PythiaInterface, GenAlg
### PYTHIA algorithm
pythia8gentool = PythiaInterface("Pythia8Interface", Filename=pythiafile)
pythia8gen = GenAlg("Pythia8", SignalProvider=pythia8gentool)
pythia8gen.hepmc.Path = "hepmcevent"

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmcevent"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

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

