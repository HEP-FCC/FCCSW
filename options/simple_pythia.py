from Gaudi.Configuration import *

# Example of pythia configuration file to generate events
pythiafile="options/Pythia_standard.cmd"
# Example of pythia configuration file to read LH event file
#pythiafile="options/Pythia_LHEinput.cmd"

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# PYTHIA algorithm
from Configurablese import PythiaInterface
pythia8gen = PythiaInterface("Pythia8Interface", Filename=pythiafile)
pythia8gen.DataOutputs.hepmc.Path = "hepmcevent"

# Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmcevent"
hepmc_converter.DataOutputs.genparticles.Path="all_genparticles"
hepmc_converter.DataOutputs.genvertices.Path="all_genvertices"

# Filters generated particles
from Configurables import GenParticleFilter
genfilter = GenParticleFilter("StableParticles")
genfilter.DataInputs.genparticles.Path = "all_genparticles"
genfilter.DataOutputs.genparticles.Path = "genparticles"

from Configurables import JetClustering_fcc__MCParticleCollection_fcc__GenJetCollection_fcc__GenJetParticleAssociationCollection_ as JetClustering
genjet_clustering = JetClustering("GenJetClustering", verbose = False)
genjet_clustering.DataInputs.particles.Path='genparticles'
genjet_clustering.DataOutputs.jets.Path='genjets'
genjet_clustering.DataOutputs.constituents.Path='genjets_particles'

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [ pythia8gen, hepmc_converter, genfilter, genjet_clustering, out ],
                EvtSel = 'NONE',
                EvtMax = 100,
                ExtSvc = [podioevent],
                OutputLevel=DEBUG
)

