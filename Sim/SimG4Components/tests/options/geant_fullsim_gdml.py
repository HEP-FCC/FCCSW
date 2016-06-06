

### \file
### \ingroup SimulationTests
### | **input (alg)**                 | other algorithms                   |                                                           |                          |                                    | **output (alg)**                                |
### | ------------------------------- | ---------------------------------- | --------------------------------------------------------- | ------------------------ | ---------------------------------- | ----------------------------------------------- |
### | read events from a HepMC file   | convert `HepMC::GenEvent` to EDM   | geometry taken from GDML (no sensitive detectors!) | FTFP_BERT physics list   | no action initialization list      | write the EDM output to ROOT file using PODIO   |


from Gaudi.Configuration import *

from Configurables import FCCDataSvc
## Data service
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import HepMCReader
## reads HepMC text file and write the HepMC::GenEvent to the data service
reader = HepMCReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.7/testsamples/FCC_minbias_100TeV.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
## reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import G4SimSvc, G4GdmlDetector
## Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
# first create tool that builds geometry from GDML
det = G4GdmlDetector("G4GdmlDetector", gdml = "Sim/SimG4Common/gdml/example.xml")
geantservice = G4SimSvc("G4SimSvc", detector=det, physicslist="G4FtfpBert", actions="G4FullSimActions")

from Configurables import G4SimAlg, G4PrimariesFromEdmTool
## Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4
particle_converter = G4PrimariesFromEdmTool("EdmConverter")
particle_converter.DataInputs.genParticles.Path = "allGenParticles"
geantsim = G4SimAlg("G4SimAlg",
                    eventGenerator=particle_converter)

from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[reader, hepmc_converter, geantsim, out],
                EvtSel='NONE',
                EvtMax=1,
                ## order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc=[podioevent, geantservice],
                OutputLevel=DEBUG)
