"""
to run example
> export PYTHIA8_XML=/afs/cern.ch/sw/lcg/releases/LCG_68/MCGenerators/pythia8/186/x86_64-slc6-gcc48-opt/xmldoc
> ./run gaudirun.py simple_pythia.py
"""
from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc

#example of pythia configuration file to generate events
pythiafile="config/Pythia_standard.cmd"

#example of pythia configuration file to read LH event file
#pythiafile="config/Pythia_LHEinput.cmd"

podioevent   = FCCDataSvc("EventDataSvc")

from Configurables import PythiaInterface
# PythiaInterface parameter
#pythia8gen = PythiaInterface("Pythia8Interface",Filename="main03.cmnd")
pythia8gen = PythiaInterface(Filename=pythiafile)
# write the HepMC::GenEvent to the data service
pythia8gen.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
# reads an HepMC::GenEvent from the data service and writes
# a collection of EDM Particles
hepmc_converter = HepMCConverter("Converter")
# the input product name matches the output product name of the previous module
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="all_genparticles"

from Configurables import GenParticleFilter
genfilter = GenParticleFilter("StableParticles")
genfilter.DataInputs.genparticles.Path = "all_genparticles"
genfilter.DataOutputs.genparticles.Path = "genparticles"

from Configurables import JetClustering_MCParticleCollection_GenJetCollection_GenJetParticleAssociationCollection_ as JetClustering
genjet_clustering = JetClustering("GenJetClustering", verbose = False)
genjet_clustering.DataInputs.particles.Path='genparticles'
# giving a meaningful name for the output product
genjet_clustering.DataOutputs.jets.Path='genjets'
genjet_clustering.DataOutputs.constituents.Path='genjets_particles'

from Configurables import PodioWrite, PodioOutput
out = PodioOutput("out",OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [ pythia8gen, hepmc_converter, genfilter, genjet_clustering, out ],
                EvtSel = 'NONE',
                EvtMax = 100,
                ExtSvc = [podioevent],
                OutputLevel=DEBUG
)

