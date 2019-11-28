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
    "genJets02": {"genJets": "genjets02", "genJetsFlavorTagged": "genjetsFlavor02"},
    # jets output tool
    "pfJets02": {"jets": "pfjets02", "jetConstituents": "pfjetParts02", "jetsFlavorTagged": "pfjetsFlavor02", 
             "jetsBTagged": "pfbTags02", "jetsCTagged": "pfcTags02", "jetsTauTagged": "pftauTags02",
             "jetsOneSubJettinessTagged": "pfjetsOneSubJettiness02", 
             "jetsTwoSubJettinessTagged": "pfjetsTwoSubJettiness02", 
             "jetsThreeSubJettinessTagged": "pfjetsThreeSubJettiness02",
             "subjetsTrimmingTagged": "pfsubjetsTrimmingTagged02", "subjetsTrimming": "pfsubjetsTrimming02", 
             "subjetsPruningTagged": "pfsubjetsPruningTagged02", "subjetsPruning": "pfsubjetsPruning02", 
             "subjetsSoftDropTagged": "pfsubjetsSoftDropTagged02", "subjetsSoftDrop": "pfsubjetsSoftDrop02"
            },
    "caloJets02": {"jets": "calojets02", "jetConstituents": "calojetParts02", "jetsFlavorTagged": "calojetsFlavor02", 
             "jetsBTagged": "calobTags02", "jetsCTagged": "calocTags02", "jetsTauTagged": "calotauTags02",
             "jetsOneSubJettinessTagged": "calojetsOneSubJettiness02", 
             "jetsTwoSubJettinessTagged": "calojetsTwoSubJettiness02", 
             "jetsThreeSubJettinessTagged": "calojetsThreeSubJettiness02",
             "subjetsTrimmingTagged": "calosubjetsTrimmingTagged02", "subjetsTrimming": "calosubjetsTrimming02", 
             "subjetsPruningTagged": "calosubjetsPruningTagged02", "subjetsPruning": "calosubjetsPruning02", 
             "subjetsSoftDropTagged": "calosubjetsSoftDropTagged02", "subjetsSoftDrop": "calosubjetsSoftDrop02"
            },
    "trkJets02": {"jets": "trkjets02", "jetConstituents": "trkjetParts02", "jetsFlavorTagged": "trkjetsFlavor02", 
             "jetsBTagged": "trkbTags02", "jetsCTagged": "trkcTags02", "jetsTauTagged": "trktauTags02",
             "jetsOneSubJettinessTagged": "trkjetsOneSubJettiness02", 
             "jetsTwoSubJettinessTagged": "trkjetsTwoSubJettiness02", 
             "jetsThreeSubJettinessTagged": "trkjetsThreeSubJettiness02",
             "subjetsTrimmingTagged": "trksubjetsTrimmingTagged02", "subjetsTrimming": "trksubjetsTrimming02", 
             "subjetsPruningTagged": "trksubjetsPruningTagged02", "subjetsPruning": "trksubjetsPruning02", 
             "subjetsSoftDropTagged": "trksubjetsSoftDropTagged02", "subjetsSoftDrop": "trksubjetsSoftDrop02"
            },

    # GenJets output tool
    "genJets04": {"genJets": "genjets04", "genJetsFlavorTagged": "genjetsFlavor04"},
    # jets output tool
    "pfJets04": {"jets": "pfjets04", "jetConstituents": "pfjetParts04", "jetsFlavorTagged": "pfjetsFlavor04", 
             "jetsBTagged": "pfbTags04", "jetsCTagged": "pfcTags04", "jetsTauTagged": "pftauTags04",
             "jetsOneSubJettinessTagged": "pfjetsOneSubJettiness04", 
             "jetsTwoSubJettinessTagged": "pfjetsTwoSubJettiness04", 
             "jetsThreeSubJettinessTagged": "pfjetsThreeSubJettiness04",
             "subjetsTrimmingTagged": "pfsubjetsTrimmingTagged04", "subjetsTrimming": "pfsubjetsTrimming04", 
             "subjetsPruningTagged": "pfsubjetsPruningTagged04", "subjetsPruning": "pfsubjetsPruning04", 
             "subjetsSoftDropTagged": "pfsubjetsSoftDropTagged04", "subjetsSoftDrop": "pfsubjetsSoftDrop04"
            },
    "caloJets04": {"jets": "calojets04", "jetConstituents": "calojetParts04", "jetsFlavorTagged": "calojetsFlavor04", 
             "jetsBTagged": "calobTags04", "jetsCTagged": "calocTags04", "jetsTauTagged": "calotauTags04",
             "jetsOneSubJettinessTagged": "calojetsOneSubJettiness04", 
             "jetsTwoSubJettinessTagged": "calojetsTwoSubJettiness04", 
             "jetsThreeSubJettinessTagged": "calojetsThreeSubJettiness04",
             "subjetsTrimmingTagged": "calosubjetsTrimmingTagged04", "subjetsTrimming": "calosubjetsTrimming04", 
             "subjetsPruningTagged": "calosubjetsPruningTagged04", "subjetsPruning": "calosubjetsPruning04", 
             "subjetsSoftDropTagged": "calosubjetsSoftDropTagged04", "subjetsSoftDrop": "calosubjetsSoftDrop04"
            },
    "trkJets04": {"jets": "trkjets04", "jetConstituents": "trkjetParts04", "jetsFlavorTagged": "trkjetsFlavor04", 
             "jetsBTagged": "trkbTags04", "jetsCTagged": "trkcTags04", "jetsTauTagged": "trktauTags04",
             "jetsOneSubJettinessTagged": "trkjetsOneSubJettiness04", 
             "jetsTwoSubJettinessTagged": "trkjetsTwoSubJettiness04", 
             "jetsThreeSubJettinessTagged": "trkjetsThreeSubJettiness04",
             "subjetsTrimmingTagged": "trksubjetsTrimmingTagged04", "subjetsTrimming": "trksubjetsTrimming04", 
             "subjetsPruningTagged": "trksubjetsPruningTagged04", "subjetsPruning": "trksubjetsPruning04", 
             "subjetsSoftDropTagged": "trksubjetsSoftDropTagged04", "subjetsSoftDrop": "trksubjetsSoftDrop04"
            },

    # GenJets output tool
    "genJets08": {"genJets": "genjets08", "genJetsFlavorTagged": "genjetsFlavor08"},
    # jets output tool
    "pfJets08": {"jets": "pfjets08", "jetConstituents": "pfjetParts08", "jetsFlavorTagged": "pfjetsFlavor08", 
             "jetsBTagged": "pfbTags08", "jetsCTagged": "pfcTags08", "jetsTauTagged": "pftauTags08",
             "jetsOneSubJettinessTagged": "pfjetsOneSubJettiness08", 
             "jetsTwoSubJettinessTagged": "pfjetsTwoSubJettiness08", 
             "jetsThreeSubJettinessTagged": "pfjetsThreeSubJettiness08",
             "subjetsTrimmingTagged": "pfsubjetsTrimmingTagged08", "subjetsTrimming": "pfsubjetsTrimming08", 
             "subjetsPruningTagged": "pfsubjetsPruningTagged08", "subjetsPruning": "pfsubjetsPruning08", 
             "subjetsSoftDropTagged": "pfsubjetsSoftDropTagged08", "subjetsSoftDrop": "pfsubjetsSoftDrop08"
            },
    "caloJets08": {"jets": "calojets08", "jetConstituents": "calojetParts08", "jetsFlavorTagged": "calojetsFlavor08", 
             "jetsBTagged": "calobTags08", "jetsCTagged": "calocTags08", "jetsTauTagged": "calotauTags08",
             "jetsOneSubJettinessTagged": "calojetsOneSubJettiness08", 
             "jetsTwoSubJettinessTagged": "calojetsTwoSubJettiness08", 
             "jetsThreeSubJettinessTagged": "calojetsThreeSubJettiness08",
             "subjetsTrimmingTagged": "calosubjetsTrimmingTagged08", "subjetsTrimming": "calosubjetsTrimming08", 
             "subjetsPruningTagged": "calosubjetsPruningTagged08", "subjetsPruning": "calosubjetsPruning08", 
             "subjetsSoftDropTagged": "calosubjetsSoftDropTagged08", "subjetsSoftDrop": "calosubjetsSoftDrop08"
            },
    "trkJets08": {"jets": "trkjets08", "jetConstituents": "trkjetParts08", "jetsFlavorTagged": "trkjetsFlavor08", 
             "jetsBTagged": "trkbTags08", "jetsCTagged": "trkcTags08", "jetsTauTagged": "trktauTags08",
             "jetsOneSubJettinessTagged": "trkjetsOneSubJettiness08", 
             "jetsTwoSubJettinessTagged": "trkjetsTwoSubJettiness08", 
             "jetsThreeSubJettinessTagged": "trkjetsThreeSubJettiness08",
             "subjetsTrimmingTagged": "trksubjetsTrimmingTagged08", "subjetsTrimming": "trksubjetsTrimming08", 
             "subjetsPruningTagged": "trksubjetsPruningTagged08", "subjetsPruning": "trksubjetsPruning08", 
             "subjetsSoftDropTagged": "trksubjetsSoftDropTagged08", "subjetsSoftDrop": "trksubjetsSoftDrop08"
            },

    # GenJets output tool
    "genJets15": {"genJets": "genjets15", "genJetsFlavorTagged": "genjetsFlavor15"},
    # jets output tool
    "pfJets15": {"jets": "pfjets15", "jetConstituents": "pfjetParts15", "jetsFlavorTagged": "pfjetsFlavor15", 
             "jetsBTagged": "pfbTags15", "jetsCTagged": "pfcTags15", "jetsTauTagged": "pftauTags15",
             "jetsOneSubJettinessTagged": "pfjetsOneSubJettiness15", 
             "jetsTwoSubJettinessTagged": "pfjetsTwoSubJettiness15", 
             "jetsThreeSubJettinessTagged": "pfjetsThreeSubJettiness15",
             "subjetsTrimmingTagged": "pfsubjetsTrimmingTagged15", "subjetsTrimming": "pfsubjetsTrimming15", 
             "subjetsPruningTagged": "pfsubjetsPruningTagged15", "subjetsPruning": "pfsubjetsPruning15", 
             "subjetsSoftDropTagged": "pfsubjetsSoftDropTagged15", "subjetsSoftDrop": "pfsubjetsSoftDrop15"
            },
    "caloJets15": {"jets": "calojets15", "jetConstituents": "calojetParts15", "jetsFlavorTagged": "calojetsFlavor15", 
             "jetsBTagged": "calobTags15", "jetsCTagged": "calocTags15", "jetsTauTagged": "calotauTags15",
             "jetsOneSubJettinessTagged": "calojetsOneSubJettiness15", 
             "jetsTwoSubJettinessTagged": "calojetsTwoSubJettiness15", 
             "jetsThreeSubJettinessTagged": "calojetsThreeSubJettiness15",
             "subjetsTrimmingTagged": "calosubjetsTrimmingTagged15", "subjetsTrimming": "calosubjetsTrimming15", 
             "subjetsPruningTagged": "calosubjetsPruningTagged15", "subjetsPruning": "calosubjetsPruning15", 
             "subjetsSoftDropTagged": "calosubjetsSoftDropTagged15", "subjetsSoftDrop": "calosubjetsSoftDrop15"
            },
    "trkJets15": {"jets": "trkjets15", "jetConstituents": "trkjetParts15", "jetsFlavorTagged": "trkjetsFlavor15", 
             "jetsBTagged": "trkbTags15", "jetsCTagged": "trkcTags15", "jetsTauTagged": "trktauTags15",
             "jetsOneSubJettinessTagged": "trkjetsOneSubJettiness15", 
             "jetsTwoSubJettinessTagged": "trkjetsTwoSubJettiness15", 
             "jetsThreeSubJettinessTagged": "trkjetsThreeSubJettiness15",
             "subjetsTrimmingTagged": "trksubjetsTrimmingTagged15", "subjetsTrimming": "trksubjetsTrimming15", 
             "subjetsPruningTagged": "trksubjetsPruningTagged15", "subjetsPruning": "trksubjetsPruning15", 
             "subjetsSoftDropTagged": "trksubjetsSoftDropTagged15", "subjetsSoftDrop": "trksubjetsSoftDrop15"
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

genJet02SaveTool = DelphesSaveGenJets("genJets02", delphesArrayName="GenJetFinder02/jets")
apply_paths(genJet02SaveTool, out_names["genJets02"])

genJet04SaveTool = DelphesSaveGenJets("genJets04", delphesArrayName="GenJetFinder04/jets")
apply_paths(genJet04SaveTool, out_names["genJets04"])

genJet08SaveTool = DelphesSaveGenJets("genJets08", delphesArrayName="GenJetFinder08/jets")
apply_paths(genJet08SaveTool, out_names["genJets08"])

genJet15SaveTool = DelphesSaveGenJets("genJets15", delphesArrayName="GenJetFinder15/jets")
apply_paths(genJet15SaveTool, out_names["genJets15"])

caloJet02SaveTool = DelphesSaveJets("caloJets02", delphesArrayName="CaloJetFinder02/jets", saveSubstructure=True)
apply_paths(caloJet02SaveTool, out_names["caloJets02"])

caloJet04SaveTool = DelphesSaveJets("caloJets04", delphesArrayName="CaloJetFinder04/jets", saveSubstructure=True)
apply_paths(caloJet04SaveTool, out_names["caloJets04"])

caloJet08SaveTool = DelphesSaveJets("caloJets08", delphesArrayName="CaloJetFinder08/jets", saveSubstructure=True)
apply_paths(caloJet08SaveTool, out_names["caloJets08"])

caloJet15SaveTool = DelphesSaveJets("caloJets15", delphesArrayName="CaloJetFinder15/jets", saveSubstructure=True)
apply_paths(caloJet15SaveTool, out_names["caloJets15"])

pfJet02SaveTool = DelphesSaveJets("pfJets02", delphesArrayName="FastJetFinder02/jets", saveSubstructure=True)
apply_paths(pfJet02SaveTool, out_names["pfJets02"])

pfJet04SaveTool = DelphesSaveJets("pfJets04", delphesArrayName="JetEnergyScale/jets", saveSubstructure=True)
apply_paths(pfJet04SaveTool, out_names["pfJets04"])

pfJet08SaveTool = DelphesSaveJets("pfJets08", delphesArrayName="FastJetFinder08/jets", saveSubstructure=True)
apply_paths(pfJet08SaveTool, out_names["pfJets08"])

pfJet15SaveTool = DelphesSaveJets("pfJets15", delphesArrayName="FastJetFinder15/jets", saveSubstructure=True)
apply_paths(pfJet15SaveTool, out_names["pfJets15"])

trkJet02SaveTool = DelphesSaveJets("trkJets02", delphesArrayName="TrackJetFinder02/jets", saveSubstructure=True)
apply_paths(trkJet02SaveTool, out_names["trkJets02"])

trkJet04SaveTool = DelphesSaveJets("trkJets04", delphesArrayName="TrackJetFinder04/jets", saveSubstructure=True)
apply_paths(trkJet04SaveTool, out_names["trkJets04"])

trkJet08SaveTool = DelphesSaveJets("trkJets08", delphesArrayName="TrackJetFinder08/jets", saveSubstructure=True)
apply_paths(trkJet08SaveTool, out_names["trkJets08"])

trkJet15SaveTool = DelphesSaveJets("trkJets15", delphesArrayName="TrackJetFinder15/jets", saveSubstructure=True)
apply_paths(trkJet15SaveTool, out_names["trkJets15"])

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
                                        "DelphesSaveGenJets/genJets02",
                                        "DelphesSaveGenJets/genJets04",
                                        "DelphesSaveGenJets/genJets08",
                                        "DelphesSaveGenJets/genJets15",
                                        "DelphesSaveJets/caloJets02",
                                        "DelphesSaveJets/caloJets04",
                                        "DelphesSaveJets/caloJets08",
                                        "DelphesSaveJets/caloJets15",
                                        "DelphesSaveJets/pfJets02",
                                        "DelphesSaveJets/pfJets04",
                                        "DelphesSaveJets/pfJets08",
                                        "DelphesSaveJets/pfJets15",
                                        "DelphesSaveJets/trkJets02",
                                        "DelphesSaveJets/trkJets04",
                                        "DelphesSaveJets/trkJets08",
                                        "DelphesSaveJets/trkJets15",
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
