from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, AlbersWrite, AlbersOutput

albersevent   = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import HepMCReader, HepMCDumper, PoissonPileUp, HepMCFileReader
genpileup = PoissonPileUp(name="ConstPileUp", Filename="example_MyPythia.dat", numPileUpEvents=1)
reader = HepMCReader("Reader", Filename="example_MyPythia.dat", PileUpTool=genpileup)
# have a look at the source code of HepMCReader, in Generation/src/HepMCReader
# In the following line,
#   reader.DataOutputs.YYY.Path = "ZZZ"
# YYY matches the string passed to declareOutput in the constructor of the algorithm
# XXX declares a name for the product (the HepMC::GenEvent)
reader.DataOutputs.hepmc.Path = "hepmc"


dumper = HepMCDumper()
dumper.DataInputs.hepmc.Path="hepmc"

# reads an HepMC::GenEvent from the data service and writes
# a collection of EDM Particles
from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
# the input product name matches the output product name of the previous module
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="all_genparticles"
hepmc_converter.DataOutputs.genvertices.Path="all_genvertices"

from Configurables import JetClustering_MCParticleCollection_GenJetCollection_GenJetParticleAssociationCollection_ as JetClustering
genjet_clustering = JetClustering(
    "GenJetClustering",
    verbose = False
    )
genjet_clustering.DataInputs.particles.Path='all_genparticles'
# giving a meaningful name for the output product
genjet_clustering.DataOutputs.jets.Path='genjets'
genjet_clustering.DataOutputs.constituents.Path='genjets_particles'

out = AlbersOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [reader, dumper,hepmc_converter,
              genjet_clustering,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 1,
    ExtSvc = [albersevent],
    #                EventLoop = eventloopmgr,
    #                OutputLevel=DEBUG
 )

