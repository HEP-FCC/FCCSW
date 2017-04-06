
### \file
### \ingroup BasicExamples
### | **input (alg)**                                      | other algorithms                   |                                  |                                       | **output (alg)**                                |
### | ---------------------------------------------------- | ---------------------------------- | ---------------------            | ------------------------------------- | ----------------------------------------------- |
### | generate Pythia events and save them to HepMC file   | add sample pileup                  | convert `HepMC::GenEvent` to EDM | use sample jet clustering algorithm   | write the EDM output to ROOT file using PODIO   |

from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import HepMCFileReader, GenAlg
readertool = HepMCFileReader("ReaderTool", Filename="/eos/project/f/fccsw-web/testsamples/FCC_minbias_100TeV.dat")
reader = GenAlg("Reader", SignalProvider=readertool)
reader.hepmc.Path = "hepmcevent"

from Configurables import HepMCConverter
## reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCConverter("Converter")
# the input product name matches the output product name of the previous module
hepmc_converter.hepmc.Path="hepmcevent"
# giving a meaningful name for the output product
hepmc_converter.genparticles.Path="all_genparticles"
hepmc_converter.genvertices.Path="all_genvertices"

# In the name below the "_" replace special C++ characters, such as ":" or "<" and ">", the actual C++ class name is:
# JetClustering<fcc::MCParticleCollection, fcc::GenJetCollection>
from Configurables import JetClustering_fcc__MCParticleCollection_fcc__GenJetCollection_ as JetClustering
genjet_clustering = JetClustering(
    "GenJetClustering",
    OutputLevel=INFO
)
# the input product name matches the output product name of the previous module
genjet_clustering.particles.Path='all_genparticles'
# giving a meaningful name for the output product
genjet_clustering.jets.Path='genjets'

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

