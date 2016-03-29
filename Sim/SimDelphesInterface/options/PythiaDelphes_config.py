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
from Configurables import DelphesSaveGenJets, DelphesSaveJets, DelphesSaveMet
from Configurables import DelphesSaveNeutralParticles, DelphesSaveChargedParticles

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
##  - if ROOT file not defined --> data written-out to Gaudi data store (Ouputs)
# delphesRootOutFile="delphesOutput.root"
delphesRootOutFile=""

# Define all output tools that convert the Delphes collections to FCC-EDM:
muonSaveTool = DelphesSaveChargedParticles("muons", delphesArrayName="MuonIsolation/muons")
muonSaveTool.DataOutputs.particles.Path = "muons"
muonSaveTool.DataOutputs.mcAssociations.Path = "muonsToMC"
muonSaveTool.DataOutputs.isolationTags.Path = "muonITags"
muonSaveTool.DataOutputs.isolationAssociations.Path = "muonsToITags"

eleSaveTool = DelphesSaveChargedParticles("electrons", delphesArrayName="ElectronIsolation/electrons")
eleSaveTool.DataOutputs.particles.Path = "electrons"
eleSaveTool.DataOutputs.mcAssociations.Path = "electronsToMC"
eleSaveTool.DataOutputs.isolationTags.Path = "electronITags"
eleSaveTool.DataOutputs.isolationAssociations.Path = "electronsToITags"

chhadSaveTool = DelphesSaveChargedParticles("charged", delphesArrayName="ChargedHadronMomentumSmearing/chargedHadrons", saveIsolation=False)
chhadSaveTool.DataOutputs.particles.Path = "charged"
chhadSaveTool.DataOutputs.mcAssociations.Path = "chargedToMC"

neuthadSaveTool = DelphesSaveNeutralParticles("neutral", delphesArrayName="Hcal/eflowNeutralHadrons", saveIsolation=False)
neuthadSaveTool.DataOutputs.particles.Path = "neutral"
neuthadSaveTool.DataOutputs.mcAssociations.Path = "neutralToMC"

photonsSaveTool = DelphesSaveNeutralParticles("photons", delphesArrayName="PhotonIsolation/photons")
photonsSaveTool.DataOutputs.particles.Path = "photons"
photonsSaveTool.DataOutputs.mcAssociations.Path = "photonsToMC"
photonsSaveTool.DataOutputs.isolationTags.Path = "photonITags"
photonsSaveTool.DataOutputs.isolationAssociations.Path = "photonsToITags"

genJetSaveTool = DelphesSaveGenJets("genJets", delphesArrayName="GenJetFinder/jets")
genJetSaveTool.DataOutputs.genJets.Path = "genJets"
genJetSaveTool.DataOutputs.mcAssociations.Path = "genJetsToMC"
genJetSaveTool.DataOutputs.jetFlavorTags.Path = "genJetsFlavor"
genJetSaveTool.DataOutputs.jetFlavorAssociations.Path = "genJetsToFlavor"

jetSaveTool = DelphesSaveJets("jets", delphesArrayName="JetEnergyScale/jets")
jetSaveTool.DataOutputs.jets.Path = "jets"
jetSaveTool.DataOutputs.jetConstituents.Path = "jetParts"
jetSaveTool.DataOutputs.jetConstituentAssociations.Path = "jetsToParts"
jetSaveTool.DataOutputs.jetFlavorTags.Path = "jetsFlavor"
jetSaveTool.DataOutputs.jetFlavorTagAssociations.Path = "jetsToFlavor"
jetSaveTool.DataOutputs.bTags.Path = "bTags"
jetSaveTool.DataOutputs.cTags.Path = "cTags"
jetSaveTool.DataOutputs.tauTags.Path = "tauTags"
jetSaveTool.DataOutputs.jetBTagAssociations.Path = "jetsToBTags"
jetSaveTool.DataOutputs.jetCTagAssociations.Path = "jetsToCTags"
jetSaveTool.DataOutputs.jetTauTagAssociations.Path = "jetsToTauTags"

metSaveTool = DelphesSaveMet("met", delphesMETArrayName="MissingET/momentum", delphesSHTArrayName="ScalarHT/energy")
metSaveTool.DataOutputs.missingEt.Path = "met"

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
                               ROOTOutputFile=delphesRootOutFile,
                               OutputLevel=messageLevelDelphes,
                               outputs=["DelphesSaveChargedParticles/muons",
                                        "DelphesSaveChargedParticles/electrons",
                                        "DelphesSaveChargedParticles/charged",
                                        "DelphesSaveNeutralParticles/photons",
                                        "DelphesSaveNeutralParticles/neutral",
                                        "DelphesSaveGenJets/genJets",
                                        "DelphesSaveJets/jets",
                                        "DelphesSaveMet/met"])
delphessim.DataInputs.hepmc.Path               = "hepmc"
delphessim.DataOutputs.genParticles.Path       = "genParticles"
delphessim.DataOutputs.genVertices.Path        = "genVertices"
## FCC event-data model output -> define objects to be written out
from Configurables import PodioOutput

out = PodioOutput("out",OutputLevel=messageLevelOut)
out.filename       = "FCCDelphesOutput.root"
#out.outputCommands = ["drop *",
#                      "keep genParticles",
#                      "keep genVertices",
#                      "keep genJets",
#                      "keep genJetsToMC"]
out.outputCommands = ["keep *"]

############################################################
#
# Run modules
#
############################################################

# Run Pythia + Delphes
ApplicationMgr( TopAlg = [ pythia8gen, delphessim, out ],
                EvtSel = 'NONE',
                EvtMax = nEvents,
                ExtSvc = [podioEvent])
