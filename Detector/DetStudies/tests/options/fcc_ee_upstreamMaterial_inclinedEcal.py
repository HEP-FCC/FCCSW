from Gaudi.Configuration import *

from GaudiKernel.SystemOfUnits import MeV, GeV

# Electron momentum in GeV
momentum = 50
# Theta min and max in degrees                                                                                                      
thetaMin = 85.
thetaMax = 95.

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

################## Particle gun setup
_pi = 3.14159

from Configurables import  MomentumRangeParticleGun
pgun = MomentumRangeParticleGun("ParticleGun_Electron")
pgun.PdgCodes = [11]
pgun.MomentumMin = momentum * GeV
pgun.MomentumMax = momentum * GeV
pgun.PhiMin = 0
pgun.PhiMax = 2 * _pi
# theta = 90 degrees (eta = 0)
pgun.ThetaMin = thetaMin * _pi / 180.       
pgun.ThetaMax = thetaMax * _pi / 180.       

from Configurables import GenAlg
genalg_pgun = GenAlg()
genalg_pgun.SignalProvider = pgun 
genalg_pgun.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter()
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="GenParticles"
hepmc_converter.genvertices.Path="GenVertices"

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCCeeIDEA-LAr/compact/FCCee_DectEmptyMaster.xml', 'file:Detector/DetFCCeeECalInclined/compact/FCCee_ECalBarrel_upstream.xml'],
                    OutputLevel = WARNING)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")
geantservice.g4PostInitCommands += ["/run/setCut 0.1 mm"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
# and a tool that saves the calorimeter hits
from Configurables import SimG4Alg, SimG4SaveCalHits
saveecaltool = SimG4SaveCalHits("saveECalBarrelHits",readoutNames = ["ECalBarrelEta"])
saveecaltool.positionedCaloHits.Path = "ECalBarrelPositionedHits"
saveecaltool.caloHits.Path = "ECalBarrelHits"

from Configurables import SimG4PrimariesFromEdmTool
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "GenParticles"

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
geantsim = SimG4Alg("SimG4Alg",
                    outputs= ["SimG4SaveCalHits/saveECalBarrelHits"],
                    eventProvider = particle_converter,
                    OutputLevel = DEBUG)

from Configurables import CreateCaloCells
createcellsBarrel = CreateCaloCells("CreateCaloCellsBarrel",
                                    doCellCalibration=False,
                                    addCellNoise=False, filterCellNoise=False)
createcellsBarrel.hits.Path="ECalBarrelHits"
createcellsBarrel.cells.Path="ECalBarrelCells"

from Configurables import UpstreamMaterial
hist = UpstreamMaterial("histsPresampler",
                        energyAxis=momentum,
                        phiAxis=0.1,
                        readoutName = "ECalBarrelEta",
                        layerFieldName = "layer",
                        numLayers = 8,
                        # sampling fraction is given as the upstream correction will be applied on calibrated cells
                         samplingFraction =  [0.24833] * 1 + [0.09482] * 1  +  [0.12242] * 1  +  [0.14182] * 1  +  [0.15667] * 1  +  [0.16923] * 1  +  [0.17980] * 1  +  [0.20085] * 1,
                        OutputLevel = DEBUG)
hist.deposits.Path="ECalBarrelCells"
hist.particle.Path="GenParticles"

THistSvc().Output = ["det DATAFILE='histUpstream_fccee_hits.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True
hist.AuditExecute = True

from Configurables import PodioOutput
### PODIO algorithm
out = PodioOutput("out",OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename = "fccee_upstreamMaterial_inclinedEcal.root"

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [genalg_pgun, hepmc_converter, geantsim, createcellsBarrel, hist, out],
                EvtSel = 'NONE',
                EvtMax = 10,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice, audsvc],
                OutputLevel = DEBUG
)
