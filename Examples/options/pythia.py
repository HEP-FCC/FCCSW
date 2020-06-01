
from GaudiKernel import SystemOfUnits as units
from Gaudi.Configuration import *

#### Data service
from Configurables import K4DataSvc
podioevent = K4DataSvc("EventDataSvc")

from Configurables import GaussSmearVertex
smeartool = GaussSmearVertex()
smeartool.xVertexSigma =   0.5*units.mm
smeartool.yVertexSigma =   0.5*units.mm
smeartool.zVertexSigma =  40.0*units.mm
smeartool.tVertexSigma = 180.0*units.picosecond

from Configurables import PythiaInterface
pythia8gentool = PythiaInterface()
### Example of pythia configuration file to generate events
pythiafile="Generation/data/Pythia_standard.cmd"
# Example of pythia configuration file to read LH event file
#pythiafile="options/Pythia_LHEinput.cmd"
pythia8gentool.Filename = pythiafile
pythia8gentool.doEvtGenDecays = False
pythia8gentool.printPythiaStatistics = True

from Configurables import GenAlg
pythia8gen = GenAlg("Pythia8")
pythia8gen.SignalProvider = pythia8gentool
pythia8gen.VertexSmearingTool = smeartool
pythia8gen.hepmc.Path = "hepmc"

### Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter()
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.hepmcStatusList = [] # convert particles with all statuses
hepmc_converter.genparticles.Path="GenParticles"
hepmc_converter.genvertices.Path="GenVertices"

### Filters generated particles
# accept is a list of particle statuses that should be accepted
from Configurables import GenParticleFilter
genfilter = GenParticleFilter("StableParticles")
genfilter.accept=[1]
genfilter.allGenParticles.Path = "GenParticles"
genfilter.filteredGenParticles.Path = "GenpParticlesStable"

from Configurables import PodioOutput
out = PodioOutput("out")
out.OutputLevel=DEBUG
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[ pythia8gen, hepmc_converter, genfilter, out ],
                EvtSel='NONE',
                EvtMax=2,
                ExtSvc=[podioevent],
                OutputLevel=INFO
)

