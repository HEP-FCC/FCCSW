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
ecalEndcapReadoutName = "EMECPhiEtaReco"
ecalFwdReadoutName = "EMFwdPhiEta"
hcalBarrelReadoutName = "HCalBarrelReadout"
hcalExtBarrelReadoutName = "HCalExtBarrelReadout"
hcalBarrelReadoutPhiEtaName = "BarHCal_Readout_phieta"
hcalExtBarrelReadoutPhiEtaName = "ExtBarHCal_Readout_phieta"
hcalEndcapReadoutName = "HECPhiEtaReco"
hcalFwdReadoutName = "HFwdPhiEta"
# Number of events
num_events = 3

from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent = FCCDataSvc("EventDataSvc", input="output_fullCalo_SimAndDigi_e50GeV_"+str(num_events)+"events.root")
# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", 
                        collections = ["GenParticles", "GenVertices", ecalBarrelCellsName, ecalEndcapCellsName, ecalFwdCellsName, 
                                       hcalBarrelCellsName, hcalExtBarrelCellsName, hcalEndcapCellsName, hcalFwdCellsName], 
                        OutputLevel = DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           # Tracker disabled to save cpu time
                                           #'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalExtendedBarrel_TileCal.xml',
                                           'file:Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml',
                                           'file:Detector/DetFCChhCalDiscs/compact/Forward_coneCryo.xml'
                                           ],
                    OutputLevel = INFO)

# additionally for HCal
from Configurables import RewriteBitfield
# Use Phi-Eta segmentation in Hcal barrel
rewriteHcal = RewriteBitfield("RewriteHCal",
                                # old bitfield (readout)
                                oldReadoutName = "HCalBarrelReadout",
                                # specify which fields are going to be deleted 
                                removeIds = ["row"],
                                # new bitfield (readout), with new segmentation
                                newReadoutName = "BarHCal_Readout_phieta",
                                debugPrint = 10,
                                OutputLevel= INFO)
# clusters are needed, with deposit position and cellID in bits
rewriteHcal.inhits.Path = "HCalBarrelCells"
rewriteHcal.outhits.Path = "newHCalBarrelCells"

rewriteExtHcal = RewriteBitfield("RewriteExtHcal",
                                # old bitfield (readout)
                                 oldReadoutName = hcalExtBarrelReadoutName,
                                # specify which fields are going to be altered (deleted/rewritten)
                                removeIds = ["row"],
                                # new bitfield (readout), with new segmentation
                                 newReadoutName = hcalExtBarrelReadoutPhiEtaName,
                                 debugPrint = 10,
                                 OutputLevel = INFO,
                                 inhits = "HCalExtBarrelCells",
                                 outhits = "newHCalExtBarrelCells")

##############################################################################################################
#######                                       REWRITE ENDCAP BITFIELD                            #############
##############################################################################################################

from Configurables import RewriteBitfield
rewriteECalEC = RewriteBitfield("RewriteECalEC",
                                # old bitfield (readout)
                                oldReadoutName = "EMECPhiEta",
                                # specify which fields are going to be deleted
                                removeIds = ["sublayer"],
                                # clusters are needed, with deposit position and cellID in bits
                                newReadoutName = ecalEndcapReadoutName,
                                debugPrint = 10,
                                OutputLevel = INFO)
rewriteECalEC.inhits.Path = "ECalEndcapCells"
rewriteECalEC.outhits.Path = "newECalEndcapCells"

rewriteHCalEC = RewriteBitfield("RewriteHCalEC",
                                # old bitfield (readout)
                                oldReadoutName = "HECPhiEta",
                                # specify which fields are going to be deleted
                                removeIds = ["sublayer"],
                                # new bitfield (readout), with new segmentation
                                newReadoutName = hcalEndcapReadoutName,
                                debugPrint = 10,
                                OutputLevel = INFO)
# clusters are needed, with deposit position and cellID in bits
rewriteHCalEC.inhits.Path = "HCalEndcapCells"
rewriteHCalEC.outhits.Path = "newHCalEndcapCells"

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
towers.ecalEndcapCells.Path = "newECalEndcapCells"
towers.ecalFwdCells.Path = ecalFwdCellsName
towers.hcalBarrelCells.Path = "newHCalBarrelCells"
towers.hcalExtBarrelCells.Path ="newHCalExtBarrelCells"
towers.hcalEndcapCells.Path = "newHCalEndcapCells"
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
                                                 OutputLevel = DEBUG)
createClusters.clusters.Path = "CaloClusters"
createClusters.clusterCells.Path = "CaloClusterCells"

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
              rewriteHcal,
              rewriteExtHcal,
              rewriteECalEC,
              rewriteHCalEC,
              createClusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = num_events,
    ExtSvc = [podioevent, geoservice],
 )

