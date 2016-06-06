from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc, G4SingleParticleGeneratorTool
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhTrackerSimple/compact/FCChh_TrackerStandalone.xml'],
                    OutputLevel = DEBUG)


# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import G4SimSvc
# giving the names of tools will initialize the tools of that type
geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist="G4FtfpBert", actions="G4FullSimActions")

geantservice.G4commands += ["/tracking/verbose 1"]

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
pgun = G4SingleParticleGeneratorTool("GeantinoGun", etaMin=0, etaMax=0.01, particleName="geantino")
geantsim = G4SimAlg("G4SimAlg",
                    outputs= ["G4SaveTrackerHits/saveTrackerHits" ],
                    eventGenerator=pgun)



# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 10,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
