
### \file
### \ingroup SimulationExamples
### | **input (alg)**                 | other algorithms                   |                                                                           |                                                                           |                                               | **output (alg)**                                |
### | ------------------------------- | ---------------------------------- | ---------------------------------------------------------                 | ------------------------                                                  | ----------------------------------            | ----------------------------------------------- |
### | read events from a HepMC file   | convert `HepMC::GenEvent` to EDM   | geometry parsed from XML (ParametricSimTracker.xml by DD4hep using GeoSvc | FTFP_BERT physics list + sim::FastSimPhysics with parametrisation process | action initialisation creates fast sim models | write the EDM output to ROOT file using PODIO   |

from Gaudi.Configuration import *

from Configurables import FCCDataSvc
## Data service
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import HepMCReader
reader = HepMCReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.6/testsamples/example_MyPythia.dat")
## reads HepMC text file and write the HepMC::GenEvent to the data service
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
## reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
geoservice = GeoSvc("GeoSvc", detectors=['file:DetectorDescription/Detectors/compact/ParametricSimTracker.xml'],
                    OutputLevel = DEBUG)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import G4SimSvc, G4FastSimPhysicsList, G4FastSimActions, G4ParticleSmearSimple
## create particle smearing tool, used for smearing in the tracker
smeartool = G4ParticleSmearSimple("Smear", sigma = 0.015)
## create actions initialization tool
actionstool = G4FastSimActions("Actions", smearing=smeartool)
## create overlay on top of FTFP_BERT physics list, attaching fast sim/parametrization process
physicslisttool = G4FastSimPhysicsList("Physics", fullphysics="G4FtfpBert")
## attach those tools to the G4 service
geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist=physicslisttool, actions=actionstool)

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import G4SimAlg, G4SaveSmearedParticles
# first, create a tool that saves the smeared particles
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("G4SaveSmearedParticles")
# and YY is the given name ("saveSmearedParticles")
saveparticlestool = G4SaveSmearedParticles("saveSmearedParticles")
saveparticlestool.DataOutputs.particles.Path = "smearedParticles"
saveparticlestool.DataOutputs.particlesMCparticles.Path = "particleMCparticleAssociation"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
geantsim = G4SimAlg("G4SimAlg", outputs = ["G4SaveSmearedParticles/saveSmearedParticles"])
geantsim.DataInputs.genParticles.Path="allGenParticles"


from Configurables import PodioOutput
## PODIO algorithm
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[reader, hepmc_converter, geantsim, out],
                EvtSel='NONE',
                EvtMax=1,
                ## order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc=[podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
