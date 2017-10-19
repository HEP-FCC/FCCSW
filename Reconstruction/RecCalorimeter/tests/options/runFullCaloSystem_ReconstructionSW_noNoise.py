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
hcalBarrelReadoutName = "BarHCal_Readout_phieta"
hcalExtBarrelReadoutName = "ExtBarHCal_Readout_phieta"
# Number of events
num_events = 3

from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent = FCCDataSvc("EventDataSvc", input="output_fullCalo_SimAndDigi_e50GeV_"+str(num_events)+"events.root")
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

#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, CombinedCaloTowerTool
from GaudiKernel.PhysicalConstants import pi

towers = CombinedCaloTowerTool("towers",
                               deltaEtaTower = 0.01, deltaPhiTower = 2*pi/704.,
                               ecalBarrelReadoutName = ecalBarrelReadoutName,
                               hcalBarrelReadoutName = hcalBarrelReadoutName,
                               hcalExtBarrelReadoutName = hcalExtBarrelReadoutName,
                               calEndcapReadoutName = calEndcapReadoutName,
                               calFwdReadoutName = calFwdReadoutName,
                               OutputLevel = DEBUG)
towers.ecalBarrelCells.Path = ecalBarrelCellsName
towers.calEndcapCells.Path = calEndcapCellsName
towers.calFwdCells.Path = calFwdCellsName
towers.hcalBarrelCells.Path = hcalBarrelCellsName
towers.hcalExtBarrelCells.Path = hcalExtBarrelCellsName

# Cluster variables
windE = 9
windP = 17
posE = 5
posP = 11
dupE = 7
dupP = 13
finE = 9
finP = 17
threshold = 12

createClusters = CreateCaloClustersSlidingWindow("CreateClusters",
                                                 towerTool = towers,
                                                 nEtaWindow = windE, nPhiWindow = windP,
                                                 nEtaPosition = posE, nPhiPosition = posP,
                                                 nEtaDuplicates = dupE, nPhiDuplicates = dupP,
                                                 nEtaFinal = finE, nPhiFinal = finP,
                                                 energyThreshold = threshold,
                                                 OutputLevel = DEBUG)
createClusters.clusters.Path = "CaloClusters"

out = PodioOutput("out", filename="output_allCalo_reco.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createClusters.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              createClusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = num_events,
    ExtSvc = [podioevent, geoservice],
 )

