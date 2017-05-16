
### \file
### \ingroup SimulationExamples
### | **input (alg)**               | other algorithms                 |                                                         |                        |                                  | **output (alg)**                              |
### |-------------------------------|----------------------------------|---------------------------------------------------------|------------------------|----------------------------------|-----------------------------------------------|
### | read events from a HepEVT file | convert them to EDM |


from Gaudi.Configuration import *


# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
# from Configurables import HepMCReader
from Configurables import HepEVTReader
reader = HepEVTReader("Reader", Filename="/afs/cern.ch/user/n/nali/public/FCCSW/pairs_top_boosted0.hepevt")
reader.genparticles.Path="allGenParticles"
reader.genvertices.Path="allGenVertices"

    
# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc


geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCCeeBaseline1/compact/FCCee.xml']) 

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, IntegratorStepper="HelixSimpleRunge",
                                       FieldComponentZ=0.002, MinimumEpsilon=1)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions",
                        magneticField=field)

geantservice.g4PostInitCommands += ["/run/setCut 0.7 mm"]
geantservice.g4PostInitCommands +=["/process/eLoss/minKinEnergy 1 MeV"]
geantservice.g4PostInitCommands +=["/score/quantity/energyDeposit edep1kev"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["VertexBarrelCollection", "VertexEndcapCollection"])

savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/saveTrackerHits"],
                    eventProvider=particle_converter)

# PODIO algorithm

from Configurables import PodioOutput
out = PodioOutput("out",
                  filename="output0.root",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]


# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, geantsim, out], #, hepmc_converter],
                EvtSel = "NONE",
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=INFO
                )
