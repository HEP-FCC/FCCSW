import os
from Gaudi.Configuration import *

# Workflow Steering
from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = 'NONE'
ApplicationMgr().EvtMax = 100

## Data event model based on Podio
from Configurables import FCCDataSvc
podioEvent = FCCDataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioEvent]
ApplicationMgr().OutputLevel = INFO

## Pythia generator
from Configurables import PythiaInterface
pythia8gentool = PythiaInterface()
pythia8gentool.Filename = os.path.join(os.environ.get("FCCSWSHAREDIR", ""),"Generation/data/Pythia_ttbar.cmd")

## Write the HepMC::GenEvent to the data service
from Configurables import GenAlg
pythia8gen = GenAlg()
pythia8gen.SignalProvider = pythia8gentool
pythia8gen.hepmc.Path = "hepmc"
ApplicationMgr().TopAlg += [pythia8gen]

### Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path = "hepmc"
hepmc_converter.genparticles.Path = "genParticles"
hepmc_converter.genvertices.Path = "genVertices"
ApplicationMgr().TopAlg += [hepmc_converter]

# Define all output tools that convert the Delphes collections to FCC-EDM:

from Configurables import DelphesSaveChargedParticles

muonSaveTool = DelphesSaveChargedParticles("muons")
muonSaveTool.delphesArrayName = "MuonMomentumSmearing/muons"
muonSaveTool.particles.Path      = "muons"
muonSaveTool.mcAssociations.Path = "muonsToMC"
muonSaveTool.isolationTags.Path  = "muonITags"

eleSaveTool = DelphesSaveChargedParticles("electrons")
eleSaveTool.delphesArrayName = "ElectronFilter/electrons"
eleSaveTool.particles.Path      = "electrons"
eleSaveTool.mcAssociations.Path = "electronsToMC"
eleSaveTool.isolationTags.Path  = "electronITags"

chhadSaveTool = DelphesSaveChargedParticles("pfcharged")
chhadSaveTool.delphesArrayName = "ChargedHadronFilter/chargedHadrons"
chhadSaveTool.saveIsolation = False
chhadSaveTool.particles.Path      = "pfcharged"
chhadSaveTool.mcAssociations.Path = "pfchargedToMC"


from Configurables import DelphesSaveNeutralParticles

# Particle-Flow Neutral Hadrons output tool
neuthadSaveTool = DelphesSaveNeutralParticles("pfneutrals")
neuthadSaveTool.delphesArrayName = "HCal/eflowNeutralHadrons"
neuthadSaveTool.saveIsolation = False
neuthadSaveTool.particles.Path      = "pfneutrals"
neuthadSaveTool.mcAssociations.Path = "pfneutralsToMC"

# Particle-Flow Photons output tool
pfphotonsSaveTool = DelphesSaveNeutralParticles("pfphotons")
pfphotonsSaveTool.delphesArrayName="ECal/eflowPhotons"
pfphotonsSaveTool.saveIsolation=False
pfphotonsSaveTool.particles.Path      = "pfphotons"
pfphotonsSaveTool.mcAssociations.Path = "pfphotonsToMC"
pfphotonsSaveTool.isolationTags.Path  = "pfphotonITags"

# Photons output tool
photonsSaveTool = DelphesSaveNeutralParticles("photons")
photonsSaveTool.delphesArrayName = "PhotonEfficiency/photons"
photonsSaveTool.particles.Path      = "photons"
photonsSaveTool.mcAssociations.Path = "photonsToMC"
photonsSaveTool.isolationTags.Path  = "photonITags"


from Configurables import DelphesSaveGenJets
genJetSaveTool = DelphesSaveGenJets("genJets")
genJetSaveTool.delphesArrayName = "GenJetFinder/jets"
genJetSaveTool.genJets.Path             = "genJets"
genJetSaveTool.genJetsFlavorTagged.Path = "genJetsFlavor"


from Configurables import DelphesSaveJets
jetSaveTool = DelphesSaveJets("jets")
jetSaveTool.delphesArrayName = "JetEnergyScale/jets"
jetSaveTool.jets.Path             = "jets"
jetSaveTool.jetConstituents.Path  = "jetParts"
jetSaveTool.jetsFlavorTagged.Path = "jetsFlavor"
jetSaveTool.jetsBTagged.Path      = "bTags"
jetSaveTool.jetsCTagged.Path      = "cTags"
jetSaveTool.jetsTauTagged.Path    = "tauTags"

fatjetSaveTool = DelphesSaveJets("fatjets")
fatjetSaveTool.delphesArrayName = "FatJetFinder/jets"
fatjetSaveTool.saveSubstructure = True
fatjetSaveTool.jets.Path                        = "fatjets"
fatjetSaveTool.jetConstituents.Path             = "fatjetParts"
fatjetSaveTool.jetsOneSubJettinessTagged.Path   = "jetsOneSubJettiness"
fatjetSaveTool.jetsTwoSubJettinessTagged.Path   = "jetsTwoSubJettiness"
fatjetSaveTool.jetsThreeSubJettinessTagged.Path = "jetsThreeSubJettiness"
fatjetSaveTool.subjetsTrimmingTagged.Path       = "subjetsTrimmingTagged"
fatjetSaveTool.subjetsPruningTagged.Path        = "subjetsPruningTagged"
fatjetSaveTool.subjetsPruning.Path              = "subjetsPruning"
fatjetSaveTool.subjetsSoftDropTagged.Path       = "subjetsSoftDropTagged"
fatjetSaveTool.subjetsSoftDrop.Path             = "subjetsSoftDrop"
fatjetSaveTool.subjetsTrimming.Path             = "subjetsTrimming"

from Configurables import DelphesSaveMet
metSaveTool = DelphesSaveMet("met")
metSaveTool.delphesMETArrayName = "MissingET/momentum"
metSaveTool.delphesSHTArrayName = "ScalarHT/energy"
metSaveTool.missingEt.Path = "met"


## Delphes simulator -> define objects to be written out
from Configurables import DelphesSimulation
delphessim = DelphesSimulation()
## Define Delphes card
delphessim.DelphesCard = os.path.join(os.environ.get("FCCSWBASEDIR", ""), "share/FCCSW/Sim/SimDelphesInterface/data/FCChh_DelphesCard_Baseline_v01.tcl")
delphessim.ROOTOutputFile = ""
delphessim.ApplyGenFilter = True
delphessim.outputs = [
                       "DelphesSaveChargedParticles/muons",
                       "DelphesSaveChargedParticles/electrons",
                       "DelphesSaveNeutralParticles/photons",
                       "DelphesSaveChargedParticles/pfcharged",
                       "DelphesSaveNeutralParticles/pfphotons",
                       "DelphesSaveNeutralParticles/pfneutrals",
                       "DelphesSaveGenJets/genJets",
                       "DelphesSaveJets/jets",
                       "DelphesSaveJets/fatjets",                                        
                       "DelphesSaveMet/met",
                     ]
delphessim.hepmc.Path               = "hepmc"
delphessim.genParticles.Path        = "skimmedGenParticles"
delphessim.mcEventWeights.Path      = "mcEventWeights"
ApplicationMgr().TopAlg += [delphessim]



## FCC event-data model output -> define objects to be written out
from Configurables import PodioOutput
out = PodioOutput("out")
out.filename = "FCCDelphesOutput.root"
out.outputCommands = [
                       "keep *", 
                     ]
ApplicationMgr().TopAlg += [out]

