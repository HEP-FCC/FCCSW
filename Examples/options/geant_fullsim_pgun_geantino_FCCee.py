from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc, SimG4SingleParticleGeneratorTool

"""
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCCeeBaseline1/compact/FCCee_o3_v03.xml'],         
                    OutputLevel = DEBUG)
"""
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/FCCeeOriginal/compact/FCCee_o3_v03.xml']) 

# Geant4 service
from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, IntegratorStepper="ClassicalRK4", FieldComponentZ=0.002)

# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", 
                        actions="SimG4FullSimActions",
                        magneticField=field)




#geantservice.G4commands += ["/tracking/verbose 1"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
# savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["VertexBarrelCollection", "VertexEndcapCollection"])

savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", 
                                       readoutNames = ["VertexBarrelCollection", "VertexEndcapCollection"])

#, "InnerTrackerBarrelCollection", "InnerTrackerEndcapCollection", "OuterTrackerBarrelCollection", "OuterTrackerEndcapCollection"])

savetrackertool.DataOutputs.positionedTrackHits.Path = "positionedHits"
savetrackertool.DataOutputs.trackHits.Path = "hits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
pgun = SimG4SingleParticleGeneratorTool("GeantinoGun", etaMin=0, etaMax=0, phiMin=0, phiMax=0, particleName="e-",
                                        energyMin=60, energyMax=60)
geantsim = SimG4Alg("SimG4Alg",
                    outputs= ["SimG4SaveTrackerHits/saveTrackerHits" ],
                    eventProvider=pgun)

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                  #filename="muon_simulation/muon_100GeV_NoB.root",
                  #filename="muon_simulation/muon_24MeV_2T.root",
                  #filename="muon_simulation/muon_200MeV_NoB.root",
                  filename="muon_simulation/electron_60MeV_2T.root",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 100,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
