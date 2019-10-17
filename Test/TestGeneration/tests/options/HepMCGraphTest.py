
from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units

from Configurables import FCCDataSvc
## Data service

podioevent = FCCDataSvc("EventDataSvc")
from Configurables import HepMCFromScratch

hepmc_producer = HepMCFromScratch()
hepmc_producer.hepmc.Path="hepmc"





from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmcStatusList = []
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"


# convert (final state) EDM back to HepMC
from Configurables import EDMToHepMCConverter
edm_converter = EDMToHepMCConverter("BackConverter")
edm_converter.hepmc.Path="hepmcconverted"
edm_converter.genparticles.Path="allGenParticles"
edm_converter.genvertices.Path="allGenVertices"

from Configurables import HepMCDumper
dumper = HepMCDumper("Dumper")
dumper.hepmc.Path="hepmc"

from Configurables import PodioOutput
out = PodioOutput("out", filename = "test_hepmc_graph.root")
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr(
               TopAlg=[hepmc_producer, hepmc_converter, dumper, out],
               EvtSel='NONE',
               ExtSvc = [podioevent],
               EvtMax=1,
               OutputLevel=VERBOSE,
)

