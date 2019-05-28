from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import tesla


set_energy = 2400
input_filename = "SimuOutput/pgun/"+str(set_energy)+"MeV_theta90.root"

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc, SimG4SingleParticleGeneratorTool
geoservice = GeoSvc("GeoSvc", detectors=[
                                         'Detector/DetFCCeeIDEA/compact/FCCee_DectMaster.xml',
                                         ], 
                    OutputLevel = INFO)

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, 
                                       IntegratorStepper="ClassicalRK4", FieldComponentZ=2*tesla,
                                       MaximumStep=10000.0)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc, SimG4UserLimitPhysicsList, SimG4UserLimitRegion, SimG4FullSimActions
from GaudiKernel.SystemOfUnits import mm

regiontool = SimG4UserLimitRegion("limits", volumeNames=["CDCH"],
                                  maxStep = 2*mm)
physicslisttool = SimG4UserLimitPhysicsList("Physics", fullphysics="SimG4FtfpBert")

actions = SimG4FullSimActions()
actions.enableHistory=True
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist=physicslisttool, 
                        actions=actions, 
                        regions=["SimG4UserLimitRegion/limits"],
                        magneticField=field)

geantservice.g4PostInitCommands +=["/process/eLoss/minKinEnergy 1 MeV"]
geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4SaveTrajectory, SimG4SaveParticleHistory
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savetrajectorytool = SimG4SaveTrajectory("saveTrajectory")
savetrajectorytool.trajectory.Path = "trajectory"
savetrajectorytool.trajectoryPoints.Path = "trajectoryPoints"

savehisttool = SimG4SaveParticleHistory("saveHistory")
savehisttool.mcParticles.Path = "simParticles"
savehisttool.genVertices.Path = "simVertices"

savetrackertool = SimG4SaveTrackerHits("saveTrackerHits_Barrel", readoutNames = ["VertexBarrelCollection"])
savetrackertool.positionedTrackHits.Path = "positionedHits_barrel"
savetrackertool.trackHits.Path = "hits_barrel"
savetrackertool.digiTrackHits.Path = "digiHits_barrel"

savetrackertool_endcap = SimG4SaveTrackerHits("saveTrackerHits_Endcap", readoutNames = ["VertexEndcapCollection"])
savetrackertool_endcap.positionedTrackHits.Path = "positionedHits_endcap"
savetrackertool_endcap.trackHits.Path = "hits_endcap"
savetrackertool_endcap.digiTrackHits.Path = "digiHits_endcap"


savetrackertool_DCH = SimG4SaveTrackerHits("saveTrackerHits_DCH", readoutNames = ["DriftChamberCollection"])
savetrackertool_DCH.positionedTrackHits.Path = "positionedHits_DCH"
savetrackertool_DCH.trackHits.Path = "hits_DCH"
savetrackertool_DCH.digiTrackHits.Path = "digiHits_DCH"


pgun = SimG4SingleParticleGeneratorTool("GeantinoGun", etaMin=0, etaMax=0, phiMin=0, phiMax=360, 
                                        particleName="mu-", energyMin=set_energy, energyMax=set_energy)


geantsim = SimG4Alg("SimG4Alg",
                    outputs= [
        "SimG4SaveParticleHistory/saveHistory",
        "SimG4SaveTrackerHits/saveTrackerHits_Barrel", 
        "SimG4SaveTrackerHits/saveTrackerHits_Endcap",
        "SimG4SaveTrackerHits/saveTrackerHits_DCH"
        ],
                    eventProvider=pgun)

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                  filename = input_filename,
                   OutputLevel=INFO)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=INFO
 )
