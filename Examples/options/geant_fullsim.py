
### \file
### \ingroup SimulationExamples
### | **input (alg)**               | other algorithms                 |                                                         |                        |                                  | **output (alg)**                              |
### |-------------------------------|----------------------------------|---------------------------------------------------------|------------------------|----------------------------------|-----------------------------------------------|
### | read events from a HepMC file | convert `HepMC::GenEvent` to EDM | geometry taken from GDML file (no sensitive detectors!) | FTFP_BERT physics list | empty action initialisation list | write the EDM output to ROOT file using PODIO |


from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import HepMCReader
reader = HepMCReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.7/testsamples/FCC_minbias_100TeV.dat")
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
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhTrackerSimple/compact/FCChh_TrackerStandalone.xml'],
                    OutputLevel = DEBUG)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import G4SimSvc
# giving the names of tools will initialize the tools of that type
geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist="G4FtfpBert", actions="G4FullSimActions")

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import G4SimAlg, G4SaveTrackerHits, G4PrimariesFromEdmTool
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("G4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savetrackertool = G4SaveTrackerHits("saveTrackerHits")
savetrackertool.DataOutputs.trackClusters.Path = "clusters"
savetrackertool.DataOutputs.trackHits.Path = "hits"
savetrackertool.DataOutputs.trackHitsClusters.Path = "hitClusterAssociation"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = G4PrimariesFromEdmTool("EdmConverter")
particle_converter.DataInputs.genParticles.Path = "allGenParticles"
geantsim = G4SimAlg("G4SimAlg",
                    outputs = ["G4SaveTrackerHits/saveTrackerHits"],
                    eventGenerator=particle_converter)

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
