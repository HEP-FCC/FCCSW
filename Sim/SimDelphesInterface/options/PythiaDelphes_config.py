###########################################################
#
# FCCSw Pythia-->Delphes simulation options file
#
#  author: Z. Drasal (CERN)
#
#  - GAUDI run in a mode: Pythia + Delphes or Delphes only (if delphesHepMCInFile defined)
#  - define variables & delphes sim outputs
#
#  - inputs: 
#    * Define pythiaConfFile -> configure Pythia parameters & input (simulate or read LHE file)
#    * Define delphesCard -> describe detector response & configure Delphes - process modules, detector parameters etc.
#    * Define delphesHepMCInFile -> read Delphes input from hepMC file (if Pythia not used)
#    * Undefine ("") delphesHepMCInFile -> read Delphes input from GAUDI data store
#
#  - outputs:
#    * Define delphesRootOutFile -> write output using Delphes I/O library (Delphes objects)
#    * Undefine ("") delphesRootOutFile -> no output using Delphes I/O library
#    * Define out module to write output using FCC-EDM lib (standard FCC output)
#   
#  - run: 
#    * ./run gaudirun.py config/PythiaDelphes_config.py
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
# User: Configure variables
#
############################################################

## N-events
nEvents=10

## Message level
messageLevelPythia =INFO
messageLevelDelphes=DEBUG
messageLevelOut    =DEBUG

## Define either pythia configuration file to generate events
#pythiaConfFile="Generation/data//Pythia_standard.cmd"

## or pythia configuration file to read in LHE file & generate events
pythiaConfFile="Generation/data/Pythia_LHEinput.cmd"

## Define Delphes card
delphesCard="Sim/SimDelphesInterface/data/FCChh_DelphesCard_WithDipole_v00.tcl"

## Define Delphes input HepMC and optionaly (non-standard) ROOT output
##  - if hepMC file not defined --> data read-in from Gaudi data store (Inputs)
##  - if ROOT file not defined --> data written-out to Gaudi data store (Ouputs)
delphesHepMCInFile=""
delphesRootOutFile=""
#delphesHepMCInFile="data/ZLEP_toAll.hepmc"
#delphesRootOutFile="DelphesOutput.root"

## Define Delphes output arrays to be processed as FCC-EDM ??? particles (muons, electrons, etc.) -> 
## various Delphes modules provide the same type of particle with different level of processing ...
## Needed due to required output to FCC-EDM!
delphesMuonsOutArray    ="MuonMomentumSmearing/muons"
delphesElectronsOutArray="ElectronEnergySmearing/electrons"
delphesChargedOutArray  ="ChargedHadronMomentumSmearing/chargedHadrons"
delphesNeutralOutArray  ="Hcal/eflowNeutralHadrons"
delphesPhotonsOutArray  ="PhotonEfficiency/photons"
delphesJetsOutArray     ="JetEnergyScale/jets"
delphesMETsOutArray     ="MissingET/momentum"
delphesSHTsOutArray     ="ScalarHT/energy"

## Data event model based on Podio
podioEvent=FCCDataSvc("EventDataSvc")

############################################################
#
# Expert: Configure individual modules (algorithms)
#
############################################################

## Pythia generator
from Configurables import PythiaInterface

pythia8gen = PythiaInterface(Filename=pythiaConfFile, OutputLevel=messageLevelPythia)
## Write the HepMC::GenEvent to the data service
pythia8gen.DataOutputs.hepmc.Path = "hepmc"

## Delphes simulator -> define objects to be written out
from Configurables import DelphesSimulation
delphessim = DelphesSimulation(DelphesCard=delphesCard,
                               HepMCInputFile=delphesHepMCInFile,
                               ROOTOutputFile=delphesRootOutFile,
                               MuonsOutArray=delphesMuonsOutArray,
                               ElectronsOutArray=delphesElectronsOutArray,
                               ChargedOutArray=delphesChargedOutArray,
                               NeutralOutArray=delphesNeutralOutArray,
                               PhotonsOutArray=delphesPhotonsOutArray,
                               JetsOutArray=delphesJetsOutArray,
                               METsOutArray=delphesMETsOutArray,
                               SHTsOutArray=delphesSHTsOutArray,  
                               OutputLevel=messageLevelDelphes)
delphessim.DataInputs.hepmc.Path               = "hepmc"
delphessim.DataOutputs.genParticles.Path       = "genParticles"
delphessim.DataOutputs.genVertices.Path        = "genVertices"
delphessim.DataOutputs.recMuons.Path           = "recMuons"
delphessim.DataOutputs.recElectrons.Path       = "recElectrons"
delphessim.DataOutputs.recCharged.Path         = "recCharged"
delphessim.DataOutputs.recNeutral.Path         = "recNeutral"
delphessim.DataOutputs.recPhotons.Path         = "recPhotons"
delphessim.DataOutputs.recJets.Path            = "recJets"
delphessim.DataOutputs.recBTags.Path           = "recBTags"
delphessim.DataOutputs.recTauTags.Path         = "recTauTags"
delphessim.DataOutputs.recMETs.Path            = "recMETs"
delphessim.DataOutputs.recMuonsToMC.Path       = "recMuonsToMC"
delphessim.DataOutputs.recElectronsToMC.Path   = "recElectronsToMC"
delphessim.DataOutputs.recChargedToMC.Path     = "recChargedToMC"
delphessim.DataOutputs.recNeutralToMC.Path     = "recNeutralToMC"
delphessim.DataOutputs.recPhotonsToMC.Path     = "recPhotonsToMC"
delphessim.DataOutputs.recJetsToMC.Path        = "recJetsToMC"
delphessim.DataOutputs.recJetsToBTags.Path     = "recJetsToBTags"
delphessim.DataOutputs.recJetsToTauTags.Path   = "recJetsToTauTags" 

## FCC event-data model output -> define objects to be written out
from Configurables import PodioOutput

out = PodioOutput("out",OutputLevel=messageLevelOut)
out.filename       = "FCCDelphesOutput.root"
out.outputCommands = ["drop *",
                      "keep genParticles",
                      "keep genVertices",
                      "keep recMuons",
                      "keep recElectrons",
                      "keep recCharged",
                      "keep recPhotons",
                      "keep recJets",
                      "keep recBTags",
                      "keep recTauTags",
                      "keep recMETs",
                      "keep recMuonsToMC",
                      "keep recElectronsToMC",
                      "keep recChargedToMC",
                      "keep recPhotonsToMC",
                      "keep recJetsToMC",
                      "keep recJetsToBTags",
                      "keep recJetsToTauTags"]
#out.outputCommands = ["keep *"]

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
                  ExtSvc = [podioEvent])
# Run only Delphes - hepmc input file provided
else:
 ApplicationMgr( TopAlg = [ delphessim, out ],
                  EvtSel = 'NONE',
                  EvtMax = nEvents,
                  ExtSvc = [podioEvent])

