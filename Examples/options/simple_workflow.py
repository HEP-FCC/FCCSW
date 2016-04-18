
### \file
### \ingroup BasicExamples
### | **input (alg)**                                      | other algorithms                   |                                  |                                       | **output (alg)**                                |
### | ---------------------------------------------------- | ---------------------------------- | ---------------------            | ------------------------------------- | ----------------------------------------------- |
### | generate Pythia events and save them to HepMC file   | add sample pileup                  | convert `HepMC::GenEvent` to EDM | use sample jet clustering algorithm   | write the EDM output to ROOT file using PODIO   |

from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import HepMCReader, HepMCDumper, PoissonPileUp, HepMCFileReader
### Pileup tool. A number of additional events are merg with the signal event in HepMCReader.
genpileup = PoissonPileUp(name="ConstPileUp",
                          Filename="/afs/cern.ch/exp/fcc/sw/0.6/testsamples/example_MyPythia.dat",
                          numPileUpEvents=1)
## reads HepMC text file and write the HepMC::GenEvent to the data service
reader = HepMCReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.6/testsamples/example_MyPythia.dat",
                     PileUpTool=genpileup)
# have a look at the source code of HepMCReader, in Generation/src/HepMCReader
# In the following line,
#   reader.DataOutputs.YYY.Path = "XXX"
# YYY matches the string passed to declareOutput in the constructor of the algorithm (here "hepmc")
# XXX declares a name for the product (here the product is HepMC::GenEvent: "hepmcevent")
reader.DataOutputs.hepmc.Path = "hepmcevent"

from Configurables import HepMCConverter
## reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
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

from Configurables import PodioOutput
out = PodioOutput("out",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr(
    ## all algorithms should be put here
    TopAlg=[reader, hepmc_converter, genjet_clustering, out],
    EvtSel='NONE',
    ## number of events
    EvtMax=10,
    ## all services should be put here
    ExtSvc = [podioevent],
 )

