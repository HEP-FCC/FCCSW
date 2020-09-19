import sys, os
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
pythia8gentool.Filename = os.path.join(os.environ.get("FCCSWSHAREDIR", ""),"Generation/data/ee_Z_ddbar.cmd")


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
muonSaveTool.delphesArrayName = "MuonFilter/muons"
muonSaveTool.particles.Path      = "muons"
muonSaveTool.particles_trkCov.Path      = "muons_trkCov"
muonSaveTool.mcAssociations.Path = "muonsToMC"
muonSaveTool.isolationTags.Path  = "muonITags"

eleSaveTool = DelphesSaveChargedParticles("electrons")
eleSaveTool.delphesArrayName = "ElectronFilter/electrons"
eleSaveTool.particles.Path      = "electrons"
eleSaveTool.particles_trkCov.Path      = "electrons_trkCov"
eleSaveTool.mcAssociations.Path = "electronsToMC"
eleSaveTool.isolationTags.Path  = "electronITags"

chhadSaveTool = DelphesSaveChargedParticles("efcharged")
chhadSaveTool.delphesArrayName = "Calorimeter/eflowTracks"
chhadSaveTool.saveIsolation = False
chhadSaveTool.particles.Path      = "efcharged"
chhadSaveTool.particles_trkCov.Path      = "efcharged_trkCov"
chhadSaveTool.mcAssociations.Path = "efchargedToMC"


from Configurables import DelphesSaveNeutralParticles

# Particle-Flow Photons output tool
pfphotonsSaveTool = DelphesSaveNeutralParticles("efphotons")
pfphotonsSaveTool.delphesArrayName="Calorimeter/eflowPhotons"
pfphotonsSaveTool.saveIsolation=False
pfphotonsSaveTool.particles.Path      = "efphotons"
pfphotonsSaveTool.mcAssociations.Path = "efphotonsToMC"
pfphotonsSaveTool.isolationTags.Path  = "efphotonITags"

# Photons output tool
photonsSaveTool = DelphesSaveNeutralParticles("photons")
photonsSaveTool.delphesArrayName = "PhotonEfficiency/photons"
photonsSaveTool.particles.Path      = "photons"
photonsSaveTool.mcAssociations.Path = "photonsToMC"
photonsSaveTool.isolationTags.Path  = "photonITags"

# Particle-Flow Neutral Hadrons output tool
neuthadSaveTool = DelphesSaveNeutralParticles("efneutrals")
neuthadSaveTool.delphesArrayName = "Calorimeter/eflowNeutralHadrons"
neuthadSaveTool.saveIsolation = False
neuthadSaveTool.particles.Path      = "efneutrals"
neuthadSaveTool.mcAssociations.Path = "efneutralsToMC"


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


from Configurables import DelphesSaveMet

metSaveTool = DelphesSaveMet("met")
metSaveTool.delphesMETArrayName = "MissingET/momentum"
metSaveTool.delphesSHTArrayName = "ScalarHT/energy"
metSaveTool.missingEt.Path = "met"


## Delphes simulator -> define objects to be written out
from Configurables import DelphesSimulation
delphessim = DelphesSimulation()
## Define Delphes card
delphessim.DelphesCard = os.path.join(os.environ.get("DELPHES_DIR", ""), "cards/delphes_card_IDEAtrkCov.tcl")
delphessim.ROOTOutputFile = ""
delphessim.ApplyGenFilter = True
delphessim.outputs = [
                       "DelphesSaveChargedParticles/muons",
                       "DelphesSaveChargedParticles/electrons",
                       "DelphesSaveNeutralParticles/photons",
                       "DelphesSaveChargedParticles/efcharged",
                       "DelphesSaveNeutralParticles/efphotons",
                       "DelphesSaveNeutralParticles/efneutrals",
                       "DelphesSaveGenJets/genJets",
                       "DelphesSaveJets/jets",
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
