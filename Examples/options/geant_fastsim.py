
### \file
### \ingroup SimulationExamples
### | **input (alg)**                 | other algorithms                   |                                                                           |                                                                           |                                               | **output (alg)**                                |
### | ------------------------------- | ---------------------------------- | ---------------------------------------------------------                 | ------------------------                                                  | ----------------------------------            | ----------------------------------------------- |
### | read events from a HepMC file   | convert `HepMC::GenEvent` to EDM   | geometry parsed from XML (ParametricSimTracker.xml by DD4hep using GeoSvc | FTFP_BERT physics list + sim::FastSimPhysics with parametrisation process | action initialisation creates fast sim models | write the EDM output to ROOT file using PODIO   |

from Gaudi.Configuration import *

from Configurables import FCCDataSvc
## Data service
podioevent = FCCDataSvc("EventDataSvc")

## reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import HepMCFileReader, GenAlg
readertool = HepMCFileReader("ReaderTool", Filename="/eos/project/f/fccsw-web/testsamples/FCC_minbias_100TeV.dat")
reader = GenAlg("Reader", SignalProvider=readertool)
reader.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
## reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                         'file:Detector/DetFCChhBaseline1/compact/FCChh_TrackerAir.xml',
                                         'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Gflash.xml'],
                                        OutputLevel=INFO )

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc, SimG4FastSimPhysicsList, SimG4ParticleSmearFormula, SimG4FastSimTrackerRegion, SimG4GflashSamplingCalo, SimG4FastSimCalorimeterRegion
## create particle smearing tool, used for smearing in the tracker
smeartool = SimG4ParticleSmearFormula("smear", resolutionMomentum = "0.013")
## create region and a parametrisation model, pass smearing tool
regiontooltracker = SimG4FastSimTrackerRegion("modelTracker", volumeNames=["TrackerEnvelopeBarrel"], smearing=smeartool)
## create parametrisation of the calorimeter
gflash = SimG4GflashSamplingCalo("gflash",
                                 materialActive = "G4_lAr",
                                 materialPassive = "G4_Pb",
                                 thicknessActive = 4,
                                 thicknessPassive = 2)
regiontoolcalo = SimG4FastSimCalorimeterRegion("modelCalo", volumeNames=["ECalBarrel"], parametrisation = gflash)
## create overlay on top of FTFP_BERT physics list, attaching fast sim/parametrization process
physicslisttool = SimG4FastSimPhysicsList("Physics", fullphysics="SimG4FtfpBert")
## attach those tools to the G4 service
geantservice = SimG4Svc("SimG4Svc", physicslist=physicslisttool, regions=["SimG4FastSimTrackerRegion/modelTracker", "SimG4FastSimCalorimeterRegion/modelCalo"])

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveSmearedParticles, SimG4SaveCalHits, SimG4PrimariesFromEdmTool
# first, create a tool that saves the smeared particles
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveSmearedParticles")
# and YY is the given name ("saveSmearedParticles")
saveparticlestool = SimG4SaveSmearedParticles("saveSmearedParticles")
saveparticlestool.particles.Path = "smearedParticles"
saveparticlestool.particlesMCparticles.Path = "particleMCparticleAssociation"
savecaltool = SimG4SaveCalHits("saveCalHits", readoutNames = ["ECalHitsPhiEta"])
savecaltool.positionedCaloHits.Path = "positionedCaloHits"
savecaltool.caloHits.Path = "caloHits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveSmearedParticles/saveSmearedParticles", "SimG4SaveCalHits/saveCalHits"],
                    eventProvider=particle_converter)

from Configurables import SimG4FastSimHistograms
hist = SimG4FastSimHistograms("fastHist")
hist.particlesMCparticles.Path = "particleMCparticleAssociation"
THistSvc().Output = ["rec DATAFILE='histFormula.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

from Configurables import PodioOutput
## PODIO algorithm
out = PodioOutput("out", filename = "out_fast_tracker_formula_calo_gflash.root")
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, hist, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=INFO)
