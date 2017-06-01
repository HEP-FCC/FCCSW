
### \file
### \ingroup BasicExamples
### | **input (alg)**                                      | other algorithms                   |                       |                                       | **output (alg)**                                |
### | ---------------------------------------------------- | ---------------------------------- | --------------------- | ------------------------------------- | ----------------------------------------------- |
### | generate Pythia events and save them to HepMC file   | convert `HepMC::GenEvent` to EDM   | filter MC Particles   | use sample jet clustering algorithm   | write the EDM output to ROOT file using PODIO   |

from GaudiKernel import SystemOfUnits as units
from Gaudi.Configuration import *

### Example of pythia configuration file to generate events
pythiafile="Generation/data/Pythia_standard.cmd"
# Example of pythia configuration file to read LH event file
#pythiafile="options/Pythia_LHEinput.cmd"

from Configurables import FCCDataSvc
#### Data service
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import ConstPileUp, HepMCFileReader, GaussSmearVertex

smeartool = GaussSmearVertex(
     xVertexMean=0. * units.mm,
     xVertexSigma=0.5 * units.mm,
     yVertexMean=0 * units.mm,
     yVertexSigma=0.5 * units.mm,
     zVertexMean=0* units.mm,
     zVertexSigma=70*units.mm,
     tVertexMean = 0 * units.picosecond,
     tVertexSigma = 30 * units.picosecond)

pileuptool = ConstPileUp(numPileUpEvents=2)
pileupreader = HepMCFileReader(Filename="/eos/project/f/fccsw-web/testsamples/FCC_minbias_100TeV.dat")

from Configurables import PythiaInterface, GenAlg
### PYTHIA algorithm
pythia8gentool = PythiaInterface("Pythia8Interface", Filename=pythiafile)
pythia8gen = GenAlg("Pythia8", SignalProvider=pythia8gentool, PileUpProvider=pileupreader, VertexSmearingTool=smeartool)
pythia8gen.PileUpTool = pileuptool
pythia8gen.hepmc.Path = "hepmcevent"


from Configurables import HepMCToEDMConverter
### Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmcevent"
hepmc_converter.genparticles.Path="all_genparticles"
hepmc_converter.genvertices.Path="all_genvertices"

from Configurables import GenParticleFilter
### Filters generated particles
# accept is a list of particle statuses that should be accepted
genfilter = GenParticleFilter("StableParticles", accept=[1], OutputLevel=DEBUG)
genfilter.allGenParticles.Path = "all_genparticles"
genfilter.filteredGenParticles.Path = "genparticles"

from Configurables import JetClustering_fcc__MCParticleCollection_fcc__GenJetCollection_ as JetClustering
genjet_clustering = JetClustering("GenJetClustering", OutputLevel=DEBUG)
genjet_clustering.particles.Path='genparticles'
genjet_clustering.jets.Path='genjets'

from Configurables import PodioOutput
### PODIO algorithm
out = PodioOutput("out",OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[ pythia8gen, hepmc_converter, genfilter, genjet_clustering, out ],
                EvtSel='NONE',
                EvtMax=2,
                ExtSvc=[podioevent],
                OutputLevel=INFO
)

