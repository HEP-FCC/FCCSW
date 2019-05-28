from Gaudi.Configuration import *
import GaudiKernel.SystemOfUnits as units

from Configurables import ApplicationMgr
app = ApplicationMgr()
app.EvtSel = "NONE"
app.EvtMax = 100
app.ExtSvc = []
app.TopAlg = []
app.OutputLevel = DEBUG


from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")
app.ExtSvc += [podioevent]

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc")
geoservice.detectors = [
                         'Detector/DetFCCeeIDEA/compact/FCCee_DectMaster.xml',
                       ] 
geoservice.OutputLevel = INFO
app.ExtSvc +=[geoservice]

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
app.ExtSvc += [geantservice]

from Configurables import SimG4SaveTrajectory
savetrajectorytool = SimG4SaveTrajectory("saveTrajectory")
savetrajectorytool.trajectory.Path = "trajectory"
savetrajectorytool.trajectoryPoints.Path = "trajectoryPoints"

from Configurables import SimG4SaveParticleHistory
savehisttool = SimG4SaveParticleHistory("saveHistory")
savehisttool.mcParticles.Path = "simParticles"
savehisttool.genVertices.Path = "simVertices"

from Configurables import SimG4SaveTrackerHits
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits_Barrel")
savetrackertool.readoutNames = ["VertexBarrelCollection"]
savetrackertool.positionedTrackHits.Path = "positionedHits_barrel"
savetrackertool.trackHits.Path = "hits_barrel"
savetrackertool.digiTrackHits.Path = "digiHits_barrel"

from Configurables import SimG4SaveTrackerHits
savetrackertool_endcap = SimG4SaveTrackerHits("saveTrackerHits_Endcap")
savetrackertool_endcap.readoutNames = ["VertexEndcapCollection"]
savetrackertool_endcap.positionedTrackHits.Path = "positionedHits_endcap"
savetrackertool_endcap.trackHits.Path = "hits_endcap"
savetrackertool_endcap.digiTrackHits.Path = "digiHits_endcap"

from Configurables import SimG4SaveTrackerHits
savetrackertool_DCH = SimG4SaveTrackerHits("saveTrackerHits_DCH")
savetrackertool_DCH.readoutNames = ["DriftChamberCollection"]
savetrackertool_DCH.positionedTrackHits.Path = "positionedHits_DCH"
savetrackertool_DCH.trackHits.Path = "hits_DCH"
savetrackertool_DCH.digiTrackHits.Path = "digiHits_DCH"

set_energy = 2400 
from Configurables import SimG4SingleParticleGeneratorTool
pgun = SimG4SingleParticleGeneratorTool("GeantinoGun")
pgun.etaMin=0
pgun.etaMax=0
pgun.phiMin=0
pgun.phiMax=360
pgun.energyMin=set_energy
pgun.energyMax=set_energy
pgun.particleName="mu-"

from Configurables import SimG4Alg
geantsim = SimG4Alg("SimG4Alg")
geantsim.outputs= [
        "SimG4SaveParticleHistory/saveHistory",
        "SimG4SaveTrackerHits/saveTrackerHits_Barrel", 
        "SimG4SaveTrackerHits/saveTrackerHits_Endcap",
        "SimG4SaveTrackerHits/saveTrackerHits_DCH",
        ]
geantsim.eventProvider = pgun
app.TopAlg += [geantsim]

from Configurables import PodioOutput
out = PodioOutput("out")
out.filename = "fccee_idea_pgun.root"
out.outputCommands = ["keep *"]
app.TopAlg += [out]

