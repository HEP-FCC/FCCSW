from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc, SimG4SingleParticleGeneratorTool
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCCeeBaseline1/compact/FCCee.xml'],         
                    OutputLevel = DEBUG)




# Geant4 service
from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, IntegratorStepper="ClassicalRK4", 
                                       FieldComponentZ=0.002, 
                                       MinimumEpsilon=5e-05, MaximumEpsilon=0.001, MaximumStep=10000.0)

# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", 
                        actions="SimG4FullSimActions",
                        magneticField=field)

geantservice.g4PostInitCommands += ["/run/setCut 0.7 mm"]
geantservice.g4PostInitCommands +=["/process/eLoss/minKinEnergy 1 MeV"]
geantservice.g4PostInitCommands +=["/score/quantity/energyDeposit edep1kev"]



#geantservice.G4commands += ["/tracking/verbose 1"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")

savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", 
                                       readoutNames = ["VertexBarrelCollection", "VertexEndcapCollection"])

savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
pgun = SimG4SingleParticleGeneratorTool("GeantinoGun", etaMin=0.087, etaMax=0.087, phiMin=0, phiMax=360, particleName="mu-",
                                        energyMin=5000, energyMax=5000) #energy [MeV]
geantsim = SimG4Alg("SimG4Alg",
                    outputs= ["SimG4SaveTrackerHits/saveTrackerHits" ],
                    eventProvider=pgun)

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                  filename="test.root",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1000,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )

