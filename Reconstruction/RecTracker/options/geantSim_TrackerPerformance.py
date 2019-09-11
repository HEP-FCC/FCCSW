
import os
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants



from Gaudi.Configuration import * 

from Configurables import ApplicationMgr
main = ApplicationMgr()
main.EvtSel = 'NONE'
main.EvtMax = 10
main.TopAlg = []
main.ExtSvc = []

# PODIO algorithm
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")
main.ExtSvc += [podioevent]


### Generation ################################################# 

from Configurables import GaussSmearVertex
smeartool = GaussSmearVertex()
smeartool.xVertexSigma =   0.5*units.mm
smeartool.yVertexSigma =   0.5*units.mm
smeartool.zVertexSigma =  40.0*units.mm
smeartool.tVertexSigma = 180.0*units.picosecond

from Configurables import  ConstPtParticleGun
pgun_tool = ConstPtParticleGun()
pgun_tool.PdgCodes = [13]
pgun_tool.PhiMin = 0.0
pgun_tool.PhiMax = constants.twopi

from Configurables import GenAlg
genalg_pgun = GenAlg()
genalg_pgun.SignalProvider =  ConstPtParticleGun('ToolSvc.ConstPtParticleGun')
genalg_pgun.VertexSmearingTool = smeartool
genalg_pgun.hepmc.Path = "hepmc"
ApplicationMgr().TopAlg += [genalg_pgun]

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter()
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="GenParticlesAll"
hepmc_converter.genvertices.Path="GenVertices"
ApplicationMgr().TopAlg += [hepmc_converter]

from Configurables import GenParticleFilter
### Filters generated particles
# accept is a list of particle statuses that should be accepted
genfilter = GenParticleFilter("StableParticleFilter")
genfilter.accept = [1]
genfilter.OutputLevel = DEBUG 
genfilter.allGenParticles.Path = "GenParticlesAll"
genfilter.filteredGenParticles.Path = "GenParticles"
ApplicationMgr().TopAlg += [genfilter]


from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc")
# if FCC_DETECTORS is empty, this should use relative path to working directory
path_to_detector = os.environ.get("FCC_DETECTORS", "")
detectors_to_use=[
                   'Detector/DetFCChhTrackerTkLayout/compact/FCChh_DectEmptyTracker.xml',
                   'Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                  ]
geoservice.detectors = [os.path.join(path_to_detector, _det) for _det in detectors_to_use]
geoservice.OutputLevel = WARNING




### Simulation ################################################# 

# Setting random seed, will be propagated to Geant
from Configurables import  RndmGenSvc
from GaudiSvc.GaudiSvcConf import HepRndm__Engine_CLHEP__RanluxEngine_
randomEngine = eval('HepRndm__Engine_CLHEP__RanluxEngine_')
randomEngine = randomEngine('RndmGenSvc.Engine')
randomEngine.Seeds = [0123456]

geant_output_tool_list = []

from Configurables import SimG4SaveParticleHistory
savehisttool = SimG4SaveParticleHistory("saveHistory")
savehisttool.mcParticles.Path = "SimParticles"
savehisttool.genVertices.Path = "SimVertices"
geant_output_tool_list += [savehisttool]

from Configurables import SimG4SaveTrackerHits
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits")
savetrackertool.readoutNames = [
                                "TrackerBarrelReadout", 
                                "TrackerEndcapReadout",
                                ]
savetrackertool.positionedTrackHits.Path = "TrackerHitsPosition"
savetrackertool.trackHits.Path = "TrackerHits"
savetrackertool.digiTrackHits.Path = "TrackerHitsDigiPostPoint"
geant_output_tool_list += [savetrackertool]

from Configurables import SimG4FullSimActions
actions = SimG4FullSimActions()
actions.enableHistory=True

# set up output tools for included detectors
# the algorithm itself is configured in the sim file
from Configurables import SimG4Alg
geantsim = SimG4Alg("SimG4Alg")
geantsim.outputs += geant_output_tool_list

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("bField")
field.FieldOn = True
field.FieldZMax = 20*units.m
field.IntegratorStepper = "ClassicalRK4"

from Configurables import SimG4DD4hepDetector
geant_detector_tool = SimG4DD4hepDetector("SimG4DD4hepDetector")

from Configurables import SimG4FtfpBert
geant_physics_list = SimG4FtfpBert("PhysicsList")

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc")
geantservice.detector = geant_detector_tool
geantservice.physicslist = geant_physics_list
geantservice.actions = actions
geantservice.magneticField = field
# range cut
geantservice.g4PostInitCommands += ["/run/setCut 0.1 mm"]
geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"]
ApplicationMgr().ExtSvc +=  [geantservice]

from Configurables import SimG4PrimariesFromEdmTool
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "GenParticles"

from Configurables import SimG4Alg
geantsim = SimG4Alg("SimG4Alg")
geantsim.outputs = geant_output_tool_list
geantsim.eventProvider = particle_converter
main.TopAlg += [geantsim]

from Configurables import PodioOutput
out = PodioOutput("out")
out.outputCommands = ["keep *"]
out.filename = "muons_for_seeding.root"
ApplicationMgr().TopAlg += [out]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True
out.AuditExecute = True

