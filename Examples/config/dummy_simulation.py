from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, AlbersWrite, AlbersOutput

albersevent   = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import HepMCReader
reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
# have a look at the source code of HepMCReader, in Generation/src/HepMCReader
# In the following line, 
#   reader.Outputs.YYY.Path = "ZZZ"
# YYY matches the string passed to declareOutput in the constructor of the algorithm
# XXX declares a name for the product (the HepMC::GenEvent)    
reader.Outputs.hepmc.Path = "hepmc"

# reads an HepMC::GenEvent from the data service and writes 
# a collection of EDM Particles 
from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
# the input product name matches the output product name of the previous module
hepmc_converter.Inputs.hepmc.Path="hepmc"
hepmc_converter.Outputs.genparticles.Path="all_genparticles"

from Configurables import GenParticleFilter
genfilter = GenParticleFilter("StableParticles")
genfilter.Inputs.genparticles.Path = "all_genparticles"
genfilter.Outputs.genparticles.Path = "genparticles"

from Configurables import JetClustering_MCParticleCollection_GenJetCollection_GenJetParticleAssociationCollection_ as GenJetClustering
genjet_clustering = GenJetClustering(
    "GenJetClustering",
    verbose=False
    )
genjet_clustering.Inputs.particles.Path='genparticles'
# giving a meaningful name for the output product
genjet_clustering.Outputs.jets.Path='genjets'
genjet_clustering.Outputs.constituents.Path='genjets_particles'

from Configurables import DummySimulation
dummysimulation = DummySimulation("Simulation")
dummysimulation.Inputs.genparticles.Path = "genparticles"
dummysimulation.Outputs.particles.Path = "particles"

# reads EDM Particles and creates EDM jets
from Configurables import JetClustering_ParticleCollection_JetCollection_JetParticleAssociationCollection_ as JetClustering
jet_clustering = JetClustering(
    "JetClustering",
    verbose=False
    )
jet_clustering.Inputs.particles.Path='particles'
# giving a meaningful name for the output product
jet_clustering.Outputs.jets.Path='jets'
jet_clustering.Outputs.constituents.Path='jets_particles'

out = AlbersOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["drop *",
                      "keep *jets*",
                      "keep particles"]

ApplicationMgr( 
    TopAlg = [reader,hepmc_converter,genfilter,
#              genjet_clustering,
              dummysimulation,
              jet_clustering, 
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 10,
    ExtSvc = [albersevent],
    #                EventLoop = eventloopmgr,
    #                OutputLevel=DEBUG
 )

