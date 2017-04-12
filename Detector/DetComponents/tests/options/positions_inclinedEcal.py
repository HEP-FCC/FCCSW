from Gaudi.Configuration import *

from Configurables import HepMCReader
reader = HepMCReader("Reader", Filename="testHepMCrandom.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import HepMCDumper
hepmc_dump = HepMCDumper("hepmc")
hepmc_dump.DataInputs.hepmc.Path="hepmc"

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml'
],
                    OutputLevel = INFO)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4TestPhysicsList", actions="SimG4FullSimActions")

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
# and a tool that saves the calorimeter hits
from Configurables import SimG4Alg, SimG4SaveCalHits
saveecaltool = SimG4SaveCalHits("saveECalHits",readoutNames = ["ECalHitsPhiEta"])
saveecaltool.DataOutputs.positionedCaloHits.Path = "ECalPositionedHits"
saveecaltool.DataOutputs.caloHits.Path = "ECalHits"

geantsim = SimG4Alg("SimG4Alg",
                       outputs= ["SimG4SaveCalHits/saveECalHits"],
                       OutputLevel=DEBUG)

from Configurables import CreateVolumePositions
positions = CreateVolumePositions("positions", readoutName = "ECalHitsPhiEta", OutputLevel = DEBUG)
positions.DataInputs.caloCells.Path = "ECalHits"
positions.DataOutputs.caloPositionedHits.Path = "ECalPositions"

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename = "positions_ecalInclinedSim.root"


#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True
positions.AuditExecute = True
out.AuditExecute = True

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, hepmc_converter, hepmc_dump, geantsim, positions, out],
                EvtSel = 'NONE',
                EvtMax   = 5,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice, audsvc],
                OutputLevel=DEBUG
)
