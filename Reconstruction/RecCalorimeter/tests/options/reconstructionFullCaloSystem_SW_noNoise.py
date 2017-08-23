# Setup
# Names of cells collections
ecalBarrelCellsName = "ECalBarrelCells"
calEndcapCellsName = "CalEndcapCells"
calFwdCellsName = "CalFwdCells"
hcalBarrelCellsName = "newHCalCells"
hcalExtBarrelCellsName = "newExtHCalCells"
# Readouts
ecalBarrelReadoutName = "ECalBarrelPhiEta"
calEndcapReadoutName = "EMECPhiEta"
calFwdReadoutName = "EMFwdPhiEta"
hcalBarrelReadoutName = "BarHCal_Readout_phieta_offset"
hcalExtBarrelReadoutName = "ExtBarHCal_Readout_phieta"
# Number of events
numEvents = 5

from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent = FCCDataSvc("EventDataSvc", input="output.root")
# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections = [ecalBarrelCellsName, calEndcapCellsName, calFwdCellsName, hcalBarrelCellsName, hcalExtBarrelCellsName], OutputLevel = DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalExtendedBarrel_TileCal.xml',
                                           'file:Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml',
                                           'file:Detector/DetFCChhCalDiscs/compact/Forward_coneCryo.xml'
                                           ],
                    OutputLevel = INFO)


from Configurables import CreateDummyCellsCollection
createdummycells = CreateDummyCellsCollection("CreateDummyCells")
createdummycells.cells.Path = "DummyCaloCells"

#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, SingleCaloTowerTool, CombinedCaloTowerTool
from GaudiKernel.PhysicalConstants import pi

towersEcal = SingleCaloTowerTool("towersEcal",
                                 deltaEtaTower = 0.01, deltaPhiTower = 2*pi/704.,
                                 readoutName = ecalBarrelReadoutName,
                                 OutputLevel = DEBUG)
towersEcal.cells.Path = ecalBarrelCellsName

towers = CombinedCaloTowerTool("towers",
                               deltaEtaTower = 0.01, deltaPhiTower = 2*pi/704.,
                               ecalBarrelReadoutName = ecalBarrelReadoutName,
                               hcalBarrelReadoutName = hcalBarrelReadoutName,
                               hcalExtBarrelReadoutName = hcalExtBarrelReadoutName,
                               calEndcapReadoutName = calEndcapReadoutName,
                               calFwdReadoutName = calFwdReadoutName,
#                               hcalReadoutName = "",
                               OutputLevel = DEBUG)
towers.ecalBarrelCells.Path = ecalBarrelCellsName
towers.calEndcapCells.Path = calEndcapCellsName
towers.calFwdCells.Path = calFwdCellsName
towers.hcalBarrelCells.Path = hcalBarrelCellsName
towers.hcalExtBarrelCells.Path = hcalExtBarrelCellsName
#towers.hcalCells.Path = "DummyCaloCells"

windE = 9
windP = 17
posE = 5
posP = 11
dupE = 7
dupP = 13
finE = 9
finP = 17
thresholdE = 12
thresholdH = 5
createOneCollClusters = CreateCaloClustersSlidingWindow("CreateOneCollClusters",
                                                 towerTool = towersEcal,
                                                 nEtaWindow = windE, nPhiWindow = windP,
                                                 nEtaPosition = posE, nPhiPosition = posP,
                                                 nEtaDuplicates = dupE, nPhiDuplicates = dupP,
                                                 nEtaFinal = finE, nPhiFinal = finP,
                                                 energyThreshold = thresholdE,
                                                 OutputLevel = DEBUG)
createOneCollClusters.clusters.Path = "OneCollClusters"


createMoreCollClusters = CreateCaloClustersSlidingWindow("CreateMoreCollClusters",
                                                 towerTool = towers,
                                                 nEtaWindow = windE, nPhiWindow = windP,
                                                 nEtaPosition = posE, nPhiPosition = posP,
                                                 nEtaDuplicates = dupE, nPhiDuplicates = dupP,
                                                 nEtaFinal = finE, nPhiFinal = finP,
                                                 energyThreshold = thresholdE,
                                                 OutputLevel = DEBUG)
createMoreCollClusters.clusters.Path = "MoreCollClusters"

out = PodioOutput("out", filename="output_allCalo_reco.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createOneCollClusters.AuditExecute = True
createMoreCollClusters.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              createdummycells,
              createOneCollClusters,
              createMoreCollClusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = numEvents,
    ExtSvc = [podioevent, geoservice],
 )

