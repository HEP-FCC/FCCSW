

from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")

## create DD4hep geometry
from Configurables import GeoSvc
geoservice = GeoSvc()
geoservice.detectors = ['file:Detector/DetFCChhHCalTile/compact/standalone.xml',]
geoservice.OutputLevel = INFO

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool()
field.FieldOn = True
field.IntegratorStepper = "G4ExactHelixStepper"

from Configurables import ConstPtParticleGun
pgun_tool = ConstPtParticleGun(PdgCodes=[13], EtaMin=-1.5, EtaMax=1.5, PhiMin=0.0, PhiMax=0.0, PtMin=10000, PtMax=10000)

from Configurables import GenAlg
gen = GenAlg("ParticleGun", SignalProvider=pgun_tool, VertexSmearingTool="FlatSmearVertex")
gen.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
## Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"




# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4GeantinoDeposits", actions="SimG4FullSimActions", magneticField=field)

geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4SaveTrajectory,  SimG4GeantinosFromEdmTool
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savetrajectorytool = SimG4SaveTrajectory("saveTrajectory")
savetrajectorytool.trajectory.Path = "trajectory"
savetrajectorytool.trajectoryPoints.Path = "trajectoryPoints"

from Configurables import SimG4SaveCalHits
savehcaltool = SimG4SaveCalHits("saveHCalHits", readoutNames = ["HCalBarrelReadout"])
savehcaltool.positionedCaloHits.Path = "HCalBarrelPositionedHits"
savehcaltool.caloHits.Path = "HCalBarrelHits"

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
g4pconverter = SimG4GeantinosFromEdmTool()

geantsim = SimG4Alg("SimG4Alg",
                    outputs=  [savehcaltool, savetrajectorytool],
                    eventProvider=g4pconverter)

## ouput
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=INFO)
out.outputCommands = ["keep *"]
out.filename = 'output_hcal_geantscan.root'

from Configurables import ChronoAuditor
chronoauditor = ChronoAuditor()

from Configurables import MemStatAuditor
memauditor = MemStatAuditor()

from Configurables import MemoryAuditor
memauditor2 = MemoryAuditor()

from Configurables import AuditorSvc
auditorsvc_handle = AuditorSvc()
auditorsvc_handle.Auditors = [ chronoauditor, memauditor, memauditor2, ]




from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [ gen, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1000,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [auditorsvc_handle, podioevent, geoservice, geantservice,],
                OutputLevel=DEBUG,
)

for svc in ApplicationMgr().ExtSvc:
  svc.AuditInitialize = True

for svc in ApplicationMgr().TopAlg:
  svc.AuditExecute = True

