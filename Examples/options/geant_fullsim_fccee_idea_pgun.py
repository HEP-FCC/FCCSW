from Gaudi.Configuration import *
import GaudiKernel.SystemOfUnits as units

from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = "NONE"
ApplicationMgr().EvtMax = 100
ApplicationMgr().OutputLevel = INFO


from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioevent]

# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
path_to_detectors = os.environ.get("FCCDETECTORS", "")
geoservice = GeoSvc("GeoSvc")
geoservice.detectors = [
                          os.path.join(path_to_detectors, 'Detector/DetFCCeeIDEA/compact/FCCee_DectMaster.xml'),
                       ]
ApplicationMgr().ExtSvc += [geoservice]

from Configurables import SimG4Alg
from Configurables import SimG4SaveTrackerHits
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits_Barrel")
savetrackertool.readoutNames = ["VertexBarrelCollection"]
savetrackertool.SimTrackHits.Path = "TrackerHits_barrel"
SimG4Alg("SimG4Alg").outputs += [savetrackertool]

from Configurables import SimG4SaveTrackerHits
savetrackertool_endcap = SimG4SaveTrackerHits("saveTrackerHits_Endcap")
savetrackertool_endcap.readoutNames = ["VertexEndcapCollection"]
savetrackertool_endcap.SimTrackHits.Path = "positionedHits_endcap"
SimG4Alg("SimG4Alg").outputs += [savetrackertool_endcap]


from Configurables import SimG4SaveTrackerHits
savetrackertool_DCH = SimG4SaveTrackerHits("saveTrackerHits_DCH")
savetrackertool_DCH.readoutNames = ["DriftChamberCollection"]
savetrackertool_DCH.SimTrackHits.Path = "positionedHits_DCH"
SimG4Alg("SimG4Alg").outputs += [savetrackertool_DCH]

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool()
field.FieldOn = True
field.IntegratorStepper = "ClassicalRK4"
field.FieldComponentZ = 2*units.tesla
field.MaximumStep = 10000.0

from Configurables import SimG4UserLimitRegion
regiontool = SimG4UserLimitRegion("limits")
regiontool.volumeNames = ["CDCH"]
regiontool.maxStep = 2*units.mm

from Configurables import SimG4UserLimitPhysicsList
physicslisttool = SimG4UserLimitPhysicsList("Physics")
physicslisttool.fullphysics = "SimG4FtfpBert"

from Configurables import SimG4FullSimActions
actions = SimG4FullSimActions()
actions.enableHistory=True

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc")
geantservice.detector = 'SimG4DD4hepDetector'
geantservice.physicslist = physicslisttool
geantservice.actions = actions
geantservice.regions = [regiontool]
geantservice.magneticField= field
geantservice.g4PostInitCommands +=["/process/eLoss/minKinEnergy 1 MeV"]
geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"]
ApplicationMgr().ExtSvc += [geantservice]

from Configurables import SimG4SaveTrajectory
savetrajectorytool = SimG4SaveTrajectory("saveTrajectory")
savetrajectorytool.TrajectoryPoints.Path = "trajectoryPoints"

from Configurables import SimG4SaveParticleHistory
savehisttool = SimG4SaveParticleHistory("saveHistory")
savehisttool.GenParticles.Path = "SimParticles"



from Configurables import SimG4SingleParticleGeneratorTool
pgun = SimG4SingleParticleGeneratorTool("ParticleGun")
pgun.etaMin = 0
pgun.etaMax = 0
pgun.phiMin = 0
pgun.phiMax = 360
pgun.energyMin = 2400
pgun.energyMax = 2400
pgun.particleName = "mu-"

from Configurables import SimG4Alg
geantsim = SimG4Alg("SimG4Alg")
geantsim.outputs += [
          savehisttool,
          #todo: investigate error
          #savetrajectorytool,
        ]
geantsim.eventProvider = pgun
ApplicationMgr().TopAlg += [geantsim]

from Configurables import PodioOutput
out = PodioOutput("out")
out.filename = "fccee_idea_pgun.root"
out.outputCommands = ["keep *"]
ApplicationMgr().TopAlg += [out]

