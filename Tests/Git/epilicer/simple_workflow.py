"""
./run gaudirun.py simple_workflow.py
"""
from Gaudi.Configuration import *

from Configurables import ApplicationMgr, HepMCReader, HepMCConverter, GenParticleFilter, JetClustering, FCCDataSvc, AlbersWrite, AlbersOutput

albersevent   = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
reader = HepMCReader("Reader", Filename="pythia_Z_91_hepmc.dat")
# have a look at the source code of HepMCReader, in Generation/src/HepMCReader
# In the following line, 
#   reader.Outputs.YYY.Path = "ZZZ"
# YYY matches the string passed to declareOutput in the constructor of the algorithm
# XXX declares a name for the product (the HepMC::GenEvent)    
reader.Outputs.hepmc.Path = "hepmc"

# reads an HepMC::GenEvent from the data service and writes 
# a collection of EDM Particles 
hepmc_converter = HepMCConverter("Converter")
# the input product name matches the output product name of the previous module
hepmc_converter.Inputs.hepmc.Path="hepmc"
hepmc_converter.Outputs.genparticles.Path="genparticles"

# GenParticleFilter to select stable particles
genpart_filter = GenParticleFilter("Filter")
# input from HepMC generated particles
genpart_filter.Inputs.genparticles.Path="genparticles"
# output to filtered particles which will be input to JetClustering
genpart_filter.Outputs.filterparticles.Path="filterparticles"

# reads EDM Particles and creates EDM jets
jet_clustering = JetClustering("JetClustering", PtMin=40.)
# input from GenParticleFilter
jet_clustering.Inputs.genparticles.Path='filterparticles'
# giving a meaningful name for the output product
jet_clustering.Outputs.jets.Path='genjets'
out = AlbersOutput("out",OutputLevel=DEBUG)

ApplicationMgr( TopAlg = [reader,hepmc_converter,genpart_filter,jet_clustering],
                EvtSel = 'NONE',
                EvtMax = 1000,
                ExtSvc = [albersevent],
                #EventLoop = eventloopmgr,
                OutputLevel=DEBUG
)

