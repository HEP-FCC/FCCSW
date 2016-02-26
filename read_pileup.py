from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import HepMCReader
reader = HepMCReader("Reader", Filename="FCC_min_bias.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

# reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:DetectorDescription/Detectors/compact/TestTracker.xml'],
                    OutputLevel = DEBUG)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import G4SimSvc
# giving the names of tools will initialize the tools of that type
geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist="G4FtfpBert", actions="G4FullSimActions")

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import G4SimAlg, G4SaveTrackerHits
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("G4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savetrackertool = G4SaveTrackerHits("saveTrackerHits")
savetrackertool.DataOutputs.trackClusters.Path = "clusters"
savetrackertool.DataOutputs.trackHits.Path = "hits"
savetrackertool.DataOutputs.trackHitsClusters.Path = "hitClusterAssociation"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
geantsim = G4SimAlg("G4SimAlg",
                     outputs= ["G4SaveTrackerHits/saveTrackerHits" ])
geantsim.DataInputs.genParticles.Path="allGenParticles"

# PODIO algorithm
from Configurables import PodioOutput, PodioInput
out = PodioOutput("out",
                  filename="output.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

pilein = PodioInput("in", OutputLevel=DEBUG)
pilein.inputCommands = ["keep * "]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [pilein, out],
                EvtSel = 'NONE',
                EvtMax   = 10,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent],
                OutputLevel=DEBUG
 )
