## \file
## \ingroup SimulationExamples
## \author Z. Drasal (CERN)
## \brief FCCSw Pythia-->Delphes simulation options file
##
##
##  - GAUDI run in a mode: Pythia + Delphes or Delphes only (if delphesHepMCInFile defined)
##  - define variables & delphes sim outputs
##
##  - inputs:
##    * Define pythiaConfFile -> configure Pythia parameters & input (simulate or read LHE file)
##    * Define delphesCard -> describe detector response & configure Delphes - process modules, detector parameters etc.
##    * Define delphesHepMCInFile -> read Delphes input from hepMC file (if Pythia not used)
##    * Undefine ("") delphesHepMCInFile -> read Delphes input from GAUDI data store
##
##  - outputs:
##    * Define delphesRootOutFile -> write output using Delphes I/O library (Delphes objects)
##    * Undefine ("") delphesRootOutFile -> no output using Delphes I/O library
##    * Define out module to write output using FCC-EDM lib (standard FCC output)
##
##  - run:
##    * ./run fccrun.py Sim/SimDelphesInterface/options/PythiaDelphes_config.py
##

"""
To run Pythia together with Delphes
> ./run gaudirun.py Sim/SimDelphesInterface/options/PythiaDelphes_config.py
"""
import sys
from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc
from Configurables import DelphesSaveGenJets, DelphesSaveJets, DelphesSaveMet
from Configurables import DelphesSaveNeutralParticles, DelphesSaveChargedParticles


def apply_paths(obj, names):
  """ Applies the collection names to the Paths of DataOutputs """
  for attr, name in names.iteritems():
    getattr(obj, attr).Path = name


import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--delphescard', type=str, default=None, help='specify an input delphes card')
delphes_args, _ = parser.parse_known_args()

from FWCore.joboptions import parse_standard_job_options
args = parse_standard_job_options()

############################################################
#
# User: Configure variables
#
############################################################

## N-events
nEvents=100
if args.nevents is not None:
    nEvents = args.nevents

## Message level
messageLevelPythia =INFO
messageLevelDelphes=INFO
messageLevelOut    =INFO

## Define either pythia configuration file to generate events
pythiaConfFile="Generation/data/Pythia_ttbar.cmd"
if args.inputfile != '':
    pythiaConfFile = args.inputfile

## or pythia configuration file to read in LHE file & generate events
#pythiaConfFile="Generation/data/Pythia_LHEinput.cmd"

## Define Delphes card
delphesCard="Sim/SimDelphesInterface/data/FCChh_DelphesCard_Baseline_v01.tcl"
if delphes_args.delphescard != None:
    delphesCard = delphes_args.delphescard


## Define Delphes input HepMC and optionaly (non-standard) ROOT output
##  - if ROOT file not defined --> data written-out to Gaudi data store (Ouputs)
# delphesRootOutFile="delphesOutput.root"
delphesRootOutFile=""

## This map defines the names of the output collections. The key of the top level dict corresponds to the output tool name
# The second level key - value corresponds to output-type - collection-name. NOTE: Do only change the values, not the keys.
out_names = {

    # Particle-Flow Charged hadron output tool
    "pfcharged": {"particles": "pfcharged", "mcAssociations": "chargedToMC"},
    # Particle-Flow Neutral hadron output tool
    "pfneutrals": {"particles": "pfneutrals", "mcAssociations": "pfneutralsToMC"},
    # Particle-Flow photon output tool
    "pfphotons": {"particles": "pfphotons", "mcAssociations": "pfphotonsToMC"},
    # Muon output tool
    "muons": {"particles": "muons", "mcAssociations": "muonsToMC", "isolationTags": "muonITags"},
    # Electron output tool
    "electrons": {"particles": "electrons", "mcAssociations": "electronsToMC", "isolationTags": "electronITags"},
    # Photons output tool
    "photons": {"particles": "photons", "mcAssociations": "photonsToMC", "isolationTags": "photonITags"},
    # GenJets output tool
    "genJets": {"genJets": "genJets", "genJetsFlavorTagged": "genJetsFlavor"},
    # Jets output tool
    "jets": {"jets": "jets", "jetConstituents": "jetParts", "jetsFlavorTagged": "jetsFlavor",
              "jetsBTagged": "bTags", "jetsCTagged": "cTags", "jetsTauTagged": "tauTags"},
    # FatJets output tool
    "fatjets": {"jets": "fatjets", "jetConstituents": "fatjetParts", 
                "jetsOneSubJettinessTagged": "jetsOneSubJettiness", 
                "jetsTwoSubJettinessTagged": "jetsTwoSubJettiness", 
                "jetsThreeSubJettinessTagged": "jetsThreeSubJettiness",
                "subjetsTrimmingTagged": "subjetsTrimmingTagged", "subjetsTrimming": "subjetsTrimming", 
                "subjetsPruningTagged": "subjetsPruningTagged", "subjetsPruning": "subjetsPruning", 
                "subjetsSoftDropTagged": "subjetsSoftDropTagged", "subjetsSoftDrop": "subjetsSoftDrop", 
                },
    # Missing transverse energy output tool
    "met": {"missingEt": "met"}
    }

## Data event model based on Podio
podioEvent=FCCDataSvc("EventDataSvc")

############################################################
#
# Expert: Configure individual modules (algorithms)
#
############################################################
# Define all output tools that convert the Delphes collections to FCC-EDM:

muonSaveTool = DelphesSaveChargedParticles("muons", delphesArrayName="MuonMomentumSmearing/muons")
apply_paths(muonSaveTool, out_names["muons"])

eleSaveTool = DelphesSaveChargedParticles("electrons", delphesArrayName="ElectronFilter/electrons")
apply_paths(eleSaveTool, out_names["electrons"])

chhadSaveTool = DelphesSaveChargedParticles("pfcharged", delphesArrayName="ChargedHadronFilter/chargedHadrons", saveIsolation=False)
apply_paths(chhadSaveTool, out_names["pfcharged"])

neuthadSaveTool = DelphesSaveNeutralParticles("pfneutrals", delphesArrayName="HCal/eflowNeutralHadrons", saveIsolation=False)
apply_paths(neuthadSaveTool, out_names["pfneutrals"])

pfphotonsSaveTool = DelphesSaveNeutralParticles("pfphotons", delphesArrayName="ECal/eflowPhotons", saveIsolation=False)
apply_paths(pfphotonsSaveTool, out_names["pfphotons"])

photonsSaveTool = DelphesSaveNeutralParticles("photons", delphesArrayName="PhotonEfficiency/photons")
apply_paths(photonsSaveTool, out_names["photons"])

genJetSaveTool = DelphesSaveGenJets("genJets", delphesArrayName="GenJetFinder/jets")
apply_paths(genJetSaveTool, out_names["genJets"])

jetSaveTool = DelphesSaveJets("jets", delphesArrayName="JetEnergyScale/jets")
apply_paths(jetSaveTool, out_names["jets"])

fatjetSaveTool = DelphesSaveJets("fatjets", delphesArrayName="FatJetFinder/jets", saveSubstructure=True)
apply_paths(fatjetSaveTool, out_names["fatjets"])

metSaveTool = DelphesSaveMet("met", delphesMETArrayName="MissingET/momentum", delphesSHTArrayName="ScalarHT/energy")
apply_paths(metSaveTool, out_names["met"])

## Pythia generator
from Configurables import PythiaInterface

pythia8gentool = PythiaInterface(Filename=pythiaConfFile, OutputLevel=messageLevelPythia)
## Write the HepMC::GenEvent to the data service
from Configurables import GenAlg
pythia8gen = GenAlg("Pythia8", SignalProvider=pythia8gentool)
pythia8gen.hepmc.Path = "hepmc"

## Delphes simulator -> define objects to be written out
from Configurables import DelphesSimulation
delphessim = DelphesSimulation(DelphesCard=delphesCard,
                               ROOTOutputFile=delphesRootOutFile,
                               ApplyGenFilter=True,
                               OutputLevel=messageLevelDelphes,
                               outputs=["DelphesSaveChargedParticles/muons",
                                        "DelphesSaveChargedParticles/electrons",
                                        "DelphesSaveNeutralParticles/photons",
                                        "DelphesSaveChargedParticles/pfcharged",
                                        "DelphesSaveNeutralParticles/pfphotons",
                                        "DelphesSaveNeutralParticles/pfneutrals",
                                        "DelphesSaveGenJets/genJets",
                                        "DelphesSaveJets/jets",
                                        "DelphesSaveJets/fatjets",                                        
					"DelphesSaveMet/met"])
delphessim.hepmc.Path                = "hepmc"
delphessim.genParticles.Path        = "skimmedGenParticles"
delphessim.mcEventWeights.Path      = "mcEventWeights"

### Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="genParticles"
hepmc_converter.genvertices.Path="genVertices"

## FCC event-data model output -> define objects to be written out
from Configurables import PodioOutput

out = PodioOutput("out",OutputLevel=messageLevelOut)
out.filename       = "FCCDelphesOutput.root"
if args.outputfile != '':
    out.filename = args.outputfile

#out.outputCommands = ["drop *",
#                      "keep genParticles",
#                      "keep genVertices",
#                      "keep genJets",
#                      "keep genJetsToMC"]
out.outputCommands = ["keep *", "drop genParticles", "drop genVertices"]

############################################################
#
# Run modules
#
############################################################
# Run Pythia + Delphes
ApplicationMgr( TopAlg = [ pythia8gen, hepmc_converter, delphessim, out ],
                EvtSel = 'NONE',
                EvtMax = nEvents,
                ExtSvc = [podioEvent])

ApplicationMgr.EvtMax=10
