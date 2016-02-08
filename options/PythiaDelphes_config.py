###########################################################
#
# FCCSw Pythia-->Delphes simulation options file
#
#  author: Z. Drasal (CERN)
#
#  - define variables & delphes sim outputs
#  - run: ./run gaudirun.py options/PythiaDelphes_config.py
#
###########################################################

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

## N-events
nEvents=100

## Define either pythia configuration file to generate events
#pythiaConfFile="options/Pythia_standard.cmd"

## or pythia configuration file to read in LHE file & generate events
pythiaConfFile="options/Pythia_LHEinput.cmd"

## Define Delphes card
delphesCard="options/FCChh_DelphesCard_WithDipole_v00.tcl"

## Define Delphes objects (allParticles, partons and stable particles are flushed automatically)
delphesOutCol=[]
#delphesoutcol=["muons","electrons","photons","jets","mets","hts"]

## Define Delphes input HepMC and optionaly (non-standard) ROOT output
##  - if hepMC file not defined --> data read-in from Gaudi data store (Inputs)
##  - if ROOT file not defined --> data written-out to Gaudi data store (Ouputs)
delphesHepMCInFile=""
#delphesHepMCInFile="data/ZLEP_toAll.hepmc"
delphesRootOutFile="DelphesOutput.root"
#delphesRootOutFile=""

## Data event model based on Podio
podioEvent=FCCDataSvc("EventDataSvc")

############################################################
#
# Configure individual modules (algorithms)
#
############################################################

## Pythia generator
from Configurables import PythiaInterface

pythia8gen = PythiaInterface(Filename=pythiaConfFile, OutputLevel=INFO)
## Write the HepMC::GenEvent to the data service
pythia8gen.DataOutputs.hepmc.Path = "hepmc"

## Delphes simulator
from Configurables import DelphesSimulation
delphessim = DelphesSimulation(DelphesCard=delphesCard,
                               OutputCollections=delphesOutCol,
                               HepMCInputFile=delphesHepMCInFile,
                               ROOTOutputFile=delphesRootOutFile,
                               OutputLevel=DEBUG)
delphessim.DataInputs.hepmc.Path               = "hepmc"
delphessim.DataOutputs.allParticles.Path       = "allParticles"
delphessim.DataOutputs.genPartons.Path         = "genPartons"
delphessim.DataOutputs.genStableParticles.Path = "genStableParticles"

## FCC event-data model output
from Configurables import PodioOutput

out = PodioOutput("out",OutputLevel=DEBUG)
#out.outputCommands = ["keep *"]
out.outputCommands = ["drop *",
                      "keep allParticles",
                      "keep genPartons",
                      "keep genStableParticles"]

############################################################
#
# Run modules
#
############################################################

# Run Pythia + Delphes
if delphesHepMCInFile == "":
  ApplicationMgr( TopAlg = [ pythia8gen, delphessim, out ],
                  EvtSel = 'NONE',
                  EvtMax = nEvents,
                  ExtSvc = [podioEvent],
                  OutputLevel=INFO)
# Run only Delphes - hepmc input file provided
else:
 ApplicationMgr( TopAlg = [ delphessim, out ],
                  EvtSel = 'NONE',
                  EvtMax = nEvents,
                  ExtSvc = [podioEvent],
                  OutputLevel=DEBUG)

