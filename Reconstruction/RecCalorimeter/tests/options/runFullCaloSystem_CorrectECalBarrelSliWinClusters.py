# Setup
# Names of cells collections
ecalBarrelCellsName = "ECalBarrelCells"
ecalEndcapCellsName = "ECalEndcapCells"
ecalFwdCellsName = "ECalFwdCells"
hcalBarrelCellsName = "HCalBarrelCells"
hcalExtBarrelCellsName = "HCalExtBarrelCells"
hcalEndcapCellsName = "HCalEndcapCells"
hcalFwdCellsName = "HCalFwdCells"
# Readouts
ecalBarrelReadoutName = "ECalBarrelPhiEta"
ecalEndcapReadoutName = "EMECPhiEta"
ecalFwdReadoutName = "EMFwdPhiEta"
hcalBarrelReadoutName = "HCalBarrelReadout"
hcalExtBarrelReadoutName = "HCalExtBarrelReadout"
hcalBarrelReadoutPhiEtaName = "BarHCal_Readout_phieta"
hcalExtBarrelReadoutPhiEtaName = "ExtBarHCal_Readout_phieta"
hcalEndcapReadoutName = "HECPhiEta"
hcalFwdReadoutName = "HFwdPhiEta"
# Number of events
num_events = 3

from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent = FCCDataSvc("EventDataSvc", input="output_allCalo_reco.root")
# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader",
                        collections = ["GenParticles", "GenVertices", ecalBarrelCellsName, ecalEndcapCellsName, ecalFwdCellsName,
                                       "newHCalBarrelCells", "newHCalExtBarrelCells", hcalEndcapCellsName, hcalFwdCellsName],
                        OutputLevel = DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml' ],
                    OutputLevel = INFO)

#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, CaloTowerTool
from GaudiKernel.PhysicalConstants import pi

towers = CaloTowerTool("towers",
                               deltaEtaTower = 0.01, deltaPhiTower = 2*pi/704.,
                               ecalBarrelReadoutName = ecalBarrelReadoutName,
                               ecalEndcapReadoutName = ecalEndcapReadoutName,
                               ecalFwdReadoutName = ecalFwdReadoutName,
                               hcalBarrelReadoutName = hcalBarrelReadoutPhiEtaName,
                               hcalExtBarrelReadoutName = hcalExtBarrelReadoutPhiEtaName,
                               hcalEndcapReadoutName = hcalEndcapReadoutName,
                               hcalFwdReadoutName = hcalFwdReadoutName,
                               OutputLevel = DEBUG)
towers.ecalBarrelCells.Path = ecalBarrelCellsName
towers.ecalEndcapCells.Path = ecalEndcapCellsName
towers.ecalFwdCells.Path = ecalFwdCellsName
towers.hcalBarrelCells.Path = "newHCalBarrelCells"
towers.hcalExtBarrelCells.Path ="newHCalExtBarrelCells"
towers.hcalEndcapCells.Path = hcalEndcapCellsName
towers.hcalFwdCells.Path = hcalFwdCellsName

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
                                                 ellipse = True,
                                                 attachCells = True,
                                                 OutputLevel = DEBUG)
createClusters.clusters.Path = "CaloClusters"
createClusters.clusterCells.Path = "CaloClusterCells"

# Correct clusters
from Configurables import CorrectECalBarrelSliWinCluster
correct = CorrectECalBarrelSliWinCluster("corr",
                         clusters="CaloClusters",
                         correctedClusters="CaloClustersCorrected",
                         particle = "GenParticles",
                         vertex = "GenVertices",
                         nPhiOptimFinal = [7]*8,
                         nEtaOptimFinal = [19]*8,
                         noiseFileName = "http://fccsw.web.cern.ch/fccsw/testsamples/elecNoise_pileup_cluster_mu200_700files.root")

THistSvc().Output = ["rec DATAFILE='clusterCorrections_histograms.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=False
THistSvc().OutputLevel=INFO

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]

ApplicationMgr(
    TopAlg = [podioinput,
              createClusters,
              correct
              ],
    EvtSel = 'NONE',
    EvtMax   = num_events,
    ExtSvc = [podioevent, geoservice],
 )

