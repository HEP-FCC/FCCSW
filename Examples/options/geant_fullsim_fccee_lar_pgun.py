import os
from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from GaudiKernel.SystemOfUnits import MeV, GeV, tesla

from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = 'NONE' 
ApplicationMgr().EvtMax = 2
ApplicationMgr().OutputLevel = INFO
ApplicationMgr().StopOnSignal = True
ApplicationMgr().ExtSvc += ['RndmGenSvc']

from Configurables import FCCDataSvc
## Data service
podioevent = FCCDataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioevent]

from Configurables import MomentumRangeParticleGun
guntool = MomentumRangeParticleGun()
guntool.ThetaMin = 45 * constants.pi / 180.
guntool.ThetaMax = 135 * constants.pi / 180.
guntool.PhiMin = 0.
guntool.PhiMax = 2. * constants.pi
guntool.MomentumMin = 1. *units.GeV
guntool.MomentumMax = 1. *units.GeV
guntool.PdgCodes = [11] # 11 electron, 13 muon, 22 photon, 111 pi0, 211 pi+

from Configurables import GenAlg
gen = GenAlg()
gen.SignalProvider=guntool
gen.hepmc.Path = "hepmc"
ApplicationMgr().TopAlg += [gen]

from Configurables import HepMCToEDMConverter
## reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.GenParticles.Path="GenParticles"
ApplicationMgr().TopAlg += [hepmc_converter]



################## Simulation setup
# Detector geometry
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc")
# if FCC_DETECTORS is empty, this should use relative path to working directory
path_to_detector = os.environ.get("FCCDETECTORS", "")
detectors_to_use=[
                    'Detector/DetFCCeeIDEA-LAr/compact/FCCee_DectMaster.xml',
                  ]
# prefix all xmls with path_to_detector
geoservice.detectors = [os.path.join(path_to_detector, _det) for _det in detectors_to_use]
geoservice.OutputLevel = INFO
ApplicationMgr().ExtSvc += [geoservice]

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions

# Uncomment if history from Geant4 decays is needed (e.g. to get the photons from pi0) and set actions=actions in SimG4Svc
#from Configurables import SimG4FullSimActions
#actions = SimG4FullSimActions()
#actions.enableHistory=True
#actions.energyCut = 0.2 * GeV 
# from Configurables import SimG4SaveParticleHistory
#saveHistTool = SimG4SaveParticleHistory("saveHistory")

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc")
geantservice.detector =     "SimG4DD4hepDetector"
geantservice.physicslist =  "SimG4FtfpBert"
geantservice.actions =      "SimG4FullSimActions"
# Fixed seed to have reproducible results, change it for each job if you split one production into several jobs
# Mind that if you leave Gaudi handle random seed and some job start within the same second (very likely) you will have duplicates
geantservice.randomNumbersFromGaudi = False
geantservice.seedValue = 4242
# Range cut
geantservice.g4PreInitCommands += ["/run/setCut 0.1 mm"]
ApplicationMgr().ExtSvc += [geantservice]



# Magnetic field
from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool")
field.FieldComponentZ = -2 * units.tesla
field.FieldOn = True
field.IntegratorStepper="ClassicalRK4"

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
# and a tool that saves the calorimeter hits

# Detector readouts
# ECAL
ecalBarrelReadoutName = "ECalBarrelEta"
ecalBarrelReadoutNamePhiEta = "ECalBarrelPhiEta"
# HCAL
hcalReadoutName = "HCalBarrelReadout"
extHcalReadoutName = "HCalExtBarrelReadout"

from Configurables import SimG4Alg
from Configurables import SimG4SaveCalHits
saveECalBarrelTool = SimG4SaveCalHits("saveECalBarrelHits")
saveECalBarrelTool.readoutNames = ["ECalBarrelEta"]
saveECalBarrelTool.CaloHits.Path = "ECalBarrelPositionedHits"
SimG4Alg("SimG4Alg").outputs += [saveECalBarrelTool]


from Configurables import SimG4SaveCalHits
saveHCalTool = SimG4SaveCalHits("saveHCalBarrelHits")
saveHCalTool.readoutNames = ["HCalBarrelReadout"]
saveHCalTool.CaloHits.Path = "HCalBarrelPositionedHits"
SimG4Alg("SimG4Alg").outputs += [saveHCalTool]

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
from Configurables import SimG4PrimariesFromEdmTool
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.GenParticles.Path = "GenParticles"

from Configurables import SimG4Alg
geantsim = SimG4Alg("SimG4Alg")
geantsim.eventProvider = particle_converter
ApplicationMgr().TopAlg += [geantsim]

############## Digitization (Merging hits into cells, EM scale calibration)
# EM scale calibration (sampling fraction)
from Configurables import CalibrateInLayersTool

calibEcalBarrel = CalibrateInLayersTool("CalibrateECalBarrel")
calibEcalBarrel.samplingFraction = [0.36571381189697705] * 1 + [0.09779064189677973] * 1 + [0.12564152224404024] * 1 + [0.14350599973146283] * 1 + [0.1557126972314961] * 1 + [0.16444759076233928] * 1 + [0.17097165096847836] * 1 + [0.17684775359805122] * 1 + [0.18181154293837265] * 1 + [0.18544247938196395] * 1 + [0.18922747431624687] * 1 + [0.21187001375505543] * 1
calibEcalBarrel.readoutName = "ECalBarrelEta"
calibEcalBarrel.layerFieldName = "layer"

from Configurables import CalibrateCaloHitsTool
calibHcells = CalibrateCaloHitsTool("CalibrateHCal")
calibHcells.invSamplingFraction = "41.66"

# Create cells in ECal barrel
# 1. step - merge hits into cells with Eta and module segmentation (phi module is a 'physical' cell i.e. lead + LAr + PCB + LAr +lead)
# 2. step - rewrite the cellId using the Eta-Phi segmentation (merging several modules into one phi readout cell)
from Configurables import CreateCaloCells
createEcalBarrelCellsStep1 = CreateCaloCells("CreateECalBarrelCellsStep1")
createEcalBarrelCellsStep1.doCellCalibration = True
createEcalBarrelCellsStep1.calibTool = calibEcalBarrel
createEcalBarrelCellsStep1.addCellNoise = False
createEcalBarrelCellsStep1.filterCellNoise = False
# todo: add when update on cvmfs
createEcalBarrelCellsStep1.addPosition = True
createEcalBarrelCellsStep1.hits = "ECalBarrelPositionedHits"
createEcalBarrelCellsStep1.cells = "ECalBarrelCellsStep1"
ApplicationMgr().TopAlg += [createEcalBarrelCellsStep1]

## Use Phi-Theta segmentation in ECal barrel
from Configurables import RedoSegmentation
resegmentEcalBarrel = RedoSegmentation("ReSegmentationEcal")
# old bitfield (readout)
resegmentEcalBarrel.oldReadoutName = "ECalBarrelEta"
# specify which fields are going to be altered (deleted/rewritten)
resegmentEcalBarrel.oldSegmentationIds = ["module"]
# new bitfield (readout), with new segmentation
resegmentEcalBarrel.newReadoutName = "ECalBarrelPhiEta"
resegmentEcalBarrel.inhits = "ECalBarrelCellsStep1"
resegmentEcalBarrel.outhits = "ECalBarrelCellsStep2"
ApplicationMgr().TopAlg += [resegmentEcalBarrel]

EcalBarrelCellsName = "ECalBarrelCells"
from Configurables import CreateCaloCells
createEcalBarrelCells = CreateCaloCells("CreateECalBarrelCells")
createEcalBarrelCells.doCellCalibration = False
createEcalBarrelCells.addCellNoise = False
createEcalBarrelCells.filterCellNoise = False
createEcalBarrelCells.hits = "ECalBarrelCellsStep2"
createEcalBarrelCells.cells = "ECalBarrelCells"
ApplicationMgr().TopAlg += [createEcalBarrelCells]

# Ecal barrel cell positions (good for physics, all coordinates set properly)
from Configurables import CellPositionsECalBarrelTool
cellPositionEcalBarrelTool = CellPositionsECalBarrelTool("CellPositionsECalBarrel")
cellPositionEcalBarrelTool.readoutName = "ECalBarrelPhiEta"

from Configurables import CreateCaloCellPositionsFCCee
createEcalBarrelPositionedCells = CreateCaloCellPositionsFCCee("ECalBarrelPositionedCells")
createEcalBarrelPositionedCells.positionsECalBarrelTool = cellPositionEcalBarrelTool
createEcalBarrelPositionedCells.hits.Path = "ECalBarrelCells"
createEcalBarrelPositionedCells.positionedHits.Path = "ECalBarrelPositionedCells"
ApplicationMgr().TopAlg += [createEcalBarrelPositionedCells]

# Create cells in HCal
# 1. step - merge hits into cells with the default readout
createHcalBarrelCells = CreateCaloCells("CreateHCaloCells")
createHcalBarrelCells.doCellCalibration = True
createHcalBarrelCells.calibTool = calibHcells
createHcalBarrelCells.addPosition = True
createHcalBarrelCells.addCellNoise = False
createHcalBarrelCells.filterCellNoise = False
createHcalBarrelCells.hits = "HCalBarrelPositionedHits"
createHcalBarrelCells.cells = "HCalBarrelCells"
ApplicationMgr().TopAlg += [createHcalBarrelCells]

# sliding window clustering #FIXME not yet ready for key4hep
#Empty cells for parts of calorimeter not implemented yet
from Configurables import CreateEmptyCaloCellsCollection
createemptycells = CreateEmptyCaloCellsCollection("CreateEmptyCaloCells")
createemptycells.cells.Path = "emptyCaloCells"
ApplicationMgr().TopAlg += [createemptycells]

from Configurables import CaloTowerTool
towers = CaloTowerTool("towers")
towers.deltaEtaTower = 0.01
towers.deltaPhiTower = 2 * constants.pi / 768.
towers.ecalBarrelReadoutName = ecalBarrelReadoutNamePhiEta
towers.ecalEndcapReadoutName = ""
towers.ecalFwdReadoutName = ""
towers.hcalBarrelReadoutName = ""
towers.hcalExtBarrelReadoutName = ""
towers.hcalEndcapReadoutName = ""
towers.hcalFwdReadoutName = ""
towers.ecalBarrelCells.Path = EcalBarrelCellsName
towers.ecalEndcapCells.Path = "emptyCaloCells"
towers.ecalFwdCells.Path = "emptyCaloCells"
towers.hcalBarrelCells.Path = "emptyCaloCells"
towers.hcalExtBarrelCells.Path = "emptyCaloCells"
towers.hcalEndcapCells.Path = "emptyCaloCells"
towers.hcalFwdCells.Path = "emptyCaloCells"



from Configurables import CreateCaloClustersSlidingWindow
createClusters = CreateCaloClustersSlidingWindow("CreateClusters")
# Cluster variables
createClusters.towerTool = towers
createClusters.nEtaWindow =      9
createClusters.nPhiWindow =     17
createClusters.nEtaPosition =    5
createClusters.nPhiPosition =   11
createClusters.nEtaDuplicates =  7
createClusters.nPhiDuplicates = 13
createClusters.nEtaFinal =       9
createClusters.nPhiFinal =      17
# Minimal energy to create a cluster in GeV (FCC-ee detectors have to reconstruct low energy particles)
createClusters.energyThreshold = 0.1
createClusters.attachCells = True
createClusters.clusters.Path = "CaloClusters"
createClusters.clusterCells.Path = "CaloClusterCells"
ApplicationMgr().TopAlg += [createClusters]


################ Output
from Configurables import PodioOutput
out = PodioOutput("out")
out.outputCommands = ["keep *", "drop ECalBarrelHits", "drop HCal*", "drop ECalBarrelCellsStep*", "drop ECalBarrelPositionedHits", "drop emptyCaloCells", "drop CaloClusterCells"]
import uuid
out.filename = "output_fullCalo_SimAndDigi.root"
ApplicationMgr().TopAlg += [out]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
gen.AuditExecute = True
hepmc_converter.AuditExecute = True
geantsim.AuditExecute = True
createEcalBarrelCellsStep1.AuditExecute = True
resegmentEcalBarrel.AuditExecute = True
createEcalBarrelCells.AuditExecute = True
createHcalBarrelCells.AuditExecute = True
out.AuditExecute = True
#ApplicationMgr().ExtSvc += [audsvc]

from Configurables import EventCounter
event_counter = EventCounter('event_counter')
event_counter.Frequency = 10
ApplicationMgr().TopAlg += [event_counter]

