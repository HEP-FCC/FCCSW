from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc, SimG4SingleParticleGeneratorTool
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'],
                    OutputLevel = DEBUG)


# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4GeantinoDeposits", actions="SimG4FullSimActions")

geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4SaveTrajectory
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savetrajectorytool = SimG4SaveTrajectory("saveTrajectory")
savetrajectorytool.trajectory.Path = "trajectory"
savetrajectorytool.trajectoryPoints.Path = "trajectoryPoints"
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
pgun = SimG4SingleParticleGeneratorTool("GeantinoGun", etaMin=-5, etaMax=5, particleName="geantino")
geantsim = SimG4Alg("SimG4Alg",
                    outputs= ["SimG4SaveTrackerHits/saveTrackerHits", "SimG4SaveTrajectory/saveTrajectory" ],
                    eventProvider=pgun)

from Configurables import CompareTrackHitPositionAndCellId
comparison = CompareTrackHitPositionAndCellId()
comparison.hits.Path = "hits"
comparison.positionedHits.Path="positionedHits"

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, comparison, out],
                EvtSel = 'NONE',
                EvtMax   = 1000,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
