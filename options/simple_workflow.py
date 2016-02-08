from Gaudi.Configuration import *

# data service
from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import HepMCReader
reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
# have a look at the source code of HepMCReader, in Generation/src/HepMCReader
# In the following line,
#   reader.DataOutputs.YYY.Path = "XXX"
# YYY matches the string passed to declareOutput in the constructor of the algorithm (here "hepmc")
# XXX declares a name for the product (here the product is HepMC::GenEvent: "hepmcevent")
reader.DataOutputs.hepmc.Path = "hepmcevent"

# reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
# the input product name matches the output product name of the previous module
hepmc_converter.DataInputs.hepmc.Path="hepmcevent"
# giving a meaningful name for the output product
hepmc_converter.DataOutputs.genparticles.Path="all_genparticles"
hepmc_converter.DataOutputs.genvertices.Path="all_genvertices"

from Configurables import JetClustering_fcc__MCParticleCollection_fcc__GenJetCollection_fcc__GenJetParticleAssociationCollection_ as JetClustering
genjet_clustering = JetClustering(
    "GenJetClustering",
    verbose = False
)
# the input product name matches the output product name of the previous module
genjet_clustering.DataInputs.particles.Path='all_genparticles'
# giving a meaningful name for the output product
genjet_clustering.DataOutputs.jets.Path='genjets'
genjet_clustering.DataOutputs.constituents.Path='genjets_particles'

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr(
    # all algorithms should be put here
    TopAlg = [reader,hepmc_converter,
              genjet_clustering,
              out
    ],
    EvtSel = 'NONE',
    # number of events
    EvtMax   = 10,
    # all services should be put here
    ExtSvc = [podioevent],
    # OutputLevel=DEBUG
 )

