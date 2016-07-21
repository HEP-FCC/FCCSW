
### \file
### \ingroup BasicExamples
### | **input (alg)**                                      | other algorithms                   |                       |                                       | **output (alg)**                                |
### | ---------------------------------------------------- | ---------------------------------- | --------------------- | ------------------------------------- | ----------------------------------------------- |
### | generate Pythia events and save them to HepMC file   | convert `HepMC::GenEvent` to EDM   | filter MC Particles   | use sample jet clustering algorithm   | write the EDM output to ROOT file using PODIO   |

from Gaudi.Configuration import *

### Example of pythia configuration file to generate events
pythiafile="Generation/data/Pythia_standard.cmd"
# Example of pythia configuration file to read LH event file
#pythiafile="options/Pythia_LHEinput.cmd"

from Configurables import FCCDataSvc
#### Data service
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import ConstPileUp

pileuptool = ConstPileUp(numPileUpEvents=2, Filename="Generation/data/Pythia_minbias_pp_100TeV.cmd")

from Configurables import PythiaInterface
### PYTHIA algorithm
pythia8gen = PythiaInterface("Pythia8Interface", Filename=pythiafile)
pythia8gen.PileUpTool = pileuptool
pythia8gen.DataOutputs.hepmc.Path = "hepmcevent"


from Configurables import HepMCConverter
### Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmcevent"
hepmc_converter.DataOutputs.genparticles.Path="all_genparticles"
hepmc_converter.DataOutputs.genvertices.Path="all_genvertices"

from Configurables import GenParticleFilter
### Filters generated particles
genfilter = GenParticleFilter("StableParticles")
genfilter.DataInputs.genparticles.Path = "all_genparticles"
genfilter.DataOutputs.genparticles.Path = "genparticles"

from Configurables import JetClustering_fcc__MCParticleCollection_fcc__GenJetCollection_fcc__GenJetParticleAssociationCollection_ as JetClustering
genjet_clustering = JetClustering("GenJetClustering", verbose = False)
genjet_clustering.DataInputs.particles.Path='genparticles'
genjet_clustering.DataOutputs.jets.Path='genjets'
genjet_clustering.DataOutputs.constituents.Path='genjets_particles'

from Configurables import PodioOutput
### PODIO algorithm
out = PodioOutput("out",OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[ pythia8gen, hepmc_converter, genfilter, genjet_clustering, out ],
                EvtSel='NONE',
                EvtMax=2,
                ExtSvc=[podioevent],
                OutputLevel=DEBUG
)

