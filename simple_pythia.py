"""
export PYTHIA8_XML=/afs/cern.ch/sw/lcg/releases/MCGenerators/pythia8/186-33765/x86_64-slc6-gcc48-opt/xmldoc
./run gaudirun.py simple_pythia.py
"""
from Gaudi.Configuration import *

from Configurables import ApplicationMgr, PythiaInterface, HepMCConverter, JetClustering, FCCDataSvc, AlbersWrite, AlbersOutput

albersevent   = FCCDataSvc("EventDataSvc")

# PythiaInterface parameter 
pythia8gen = PythiaInterface(Filename="main03.cmnd")
# write the HepMC::GenEvent to the data service
pythia8gen.Outputs.hepmc.Path = "hepmc"

# reads an HepMC::GenEvent from the data service and writes 
# a collection of EDM Particles 
hepmc_converter = HepMCConverter("Converter")
# the input product name matches the output product name of the previous module
hepmc_converter.Inputs.hepmc.Path="hepmc"
hepmc_converter.Outputs.genparticles.Path="genparticles"

# reads EDM Particles and creates EDM jets
jet_clustering = JetClustering("JetClustering")
# input from GenParticleFilter
jet_clustering.Inputs.genparticles.Path='genparticles'
# giving a meaningful name for the output product
jet_clustering.Outputs.jets.Path='genjets'
out = AlbersOutput("out",OutputLevel=DEBUG)

ApplicationMgr( TopAlg = [ pythia8gen, hepmc_converter, jet_clustering ],
                EvtSel = 'NONE',
                EvtMax = 1,
                ExtSvc = [albersevent],
                OutputLevel=DEBUG
)

