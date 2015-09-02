"""
To run Pythia together with Delphes
> export PYTHIA8_XML=/afs/cern.ch/sw/lcg/releases/LCG_68/MCGenerators/pythia8/186/x86_64-slc6-gcc48-opt/xmldoc
> ./run gaudirun.py PythiaDelphes_config.py 
"""
from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc

############################################################
#
# Configure variables
#
############################################################

# Define either pythia configuration file to generate events
pythiafile="config/Pythia_standard.cmd"
# or pythia configuration file to read in LHE file & generate events
#pythiafile="config/Pythia_LHEinput.cmd"

# Data event model based on Albers
albersevent   = FCCDataSvc("EventDataSvc")

############################################################
# 
# Configure individual modules (algorithms)
#
############################################################

# Pythia
from Configurables import PythiaInterface 

pythia8gen = PythiaInterface(Filename=pythiafile)
# write the HepMC::GenEvent to the data service
pythia8gen.Outputs.hepmc.Path = "hepmc"

# Final output
from Configurables import AlbersWrite, AlbersOutput

out = AlbersOutput("out",OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

############################################################
#
# Run modules
#
############################################################
ApplicationMgr( TopAlg = [ pythia8gen, hepmc_converter, genfilter, genjet_clustering, out ],
                EvtSel = 'NONE',
                EvtMax = 10,
                ExtSvc = [albersevent],
                OutputLevel=DEBUG
)

